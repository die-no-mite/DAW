#include "midiFrame.h"
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <time.h>

wxDEFINE_EVENT(CANVAS_RECT_ADDED, wxCommandEvent);
wxDEFINE_EVENT(CANVAS_RECT_REMOVED, wxCommandEvent);

MidiFrame::MidiFrame(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxPanel(parent, id, pos, size)
{
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);

	this->Bind(wxEVT_PAINT, &MidiFrame::OnPaint, this);
	this->Bind(wxEVT_LEFT_DOWN, &MidiFrame::OnMouseDown, this);
	this->Bind(wxEVT_MOTION, &MidiFrame::OnMouseMove, this);
	this->Bind(wxEVT_LEFT_UP, &MidiFrame::OnMouseUp, this);
	this->Bind(wxEVT_LEAVE_WINDOW, &MidiFrame::OnMouseLeave, this);
	this->Bind(wxEVT_RIGHT_DOWN, &MidiFrame::OnMouseEvent, this);


	this->draggedObj = nullptr;
	this->shouldExtend = false;

	this->selected = false;
}


void MidiFrame::addNote(int width, int height, int centerX, int centerY, wxColor color)
{
	GraphicMIDIEvent obj{
		{-width / 2.0,
		-height / 2.0,
		static_cast<double>(width),
		static_cast<double>(height)},
		color,
		{}
	};
	obj.transform.Translate(static_cast<double>(centerX), static_cast<double>(centerY));
	
	
	this->noteList.push_back(obj);
	
	sendNoteAddedEvent();
	Refresh();
}

void MidiFrame::removeTopNote()
{
	if (!this->noteList.empty() && draggedObj == nullptr)
	{
		this->noteList.pop_back();

		sendNoteRemovedEvent();
		Refresh();
	}
}

//Removes Note when the note is right clicked
void MidiFrame::OnMouseEvent(wxMouseEvent &evt)
{
	removeTopNote();
}

void MidiFrame::OnPaint(wxPaintEvent& evt)
{
	wxAutoBufferedPaintDC dc(this);
	dc.Clear();

	wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

	if (gc)
	{
		for (const auto& object : noteList) {
			
			gc->SetTransform(gc->CreateMatrix(object.transform));
			
			gc->SetBrush(wxBrush(object.color));
			gc->DrawRectangle(object.note.m_x, object.note.m_y, object.note.m_width, object.note.m_height);

		}
		delete gc;
		
	}
}

void MidiFrame::OnMouseDown(wxMouseEvent& event)
{
	auto clickedObjectIter = std::find_if(noteList.rbegin(), noteList.rend(), [event](const GraphicMIDIEvent& o)
		{
			wxPoint2DDouble clickPos = event.GetPosition();
			auto inv = o.transform;
			inv.Invert();
			clickPos = inv.TransformPoint(clickPos);
			return o.note.Contains(clickPos);
		});

	if (clickedObjectIter != noteList.rend())
	{
		auto forwardIt = std::prev(clickedObjectIter.base());

		noteList.push_back(*forwardIt);
		noteList.erase(forwardIt);

		draggedObj = &(*std::prev(noteList.end()));

		lastDragOrigin = event.GetPosition();
		shouldExtend = wxGetKeyState(WXK_ALT);

		Refresh(); // for z order reversal
	}
}

void MidiFrame::OnMouseMove(wxMouseEvent& event)
{
	if (draggedObj != nullptr)
	{
		if (shouldExtend == false)
		{
			auto dragVector = event.GetPosition() - lastDragOrigin;

			auto inv = draggedObj->transform;
			inv.Invert();
			dragVector = inv.TransformDistance(dragVector);

			draggedObj->transform.Translate(dragVector.m_x, dragVector.m_y);


			lastDragOrigin = event.GetPosition();
			Refresh();
		}
		else
		{
			
			
			auto dragVector = event.GetPosition() - lastDragOrigin;
			auto inv = draggedObj->transform;
			
			

			if (lastDragOrigin.m_x < event.GetPosition().x)
			{
				inv.Invert();
				dragVector = inv.TransformDistance(dragVector);

				//draggedObj->transform.Translate(dragVector.m_x, dragVector.m_y);
				
				draggedObj->transform.Scale(1.01, 1);
			}
			else if (lastDragOrigin.m_x > event.GetPosition().x)
			{
				inv.Invert();
				
				
				draggedObj->transform.Scale(0.99, 1);
			}
			
			Refresh();
			lastDragOrigin = event.GetPosition();

			
		}
	}
}

void MidiFrame::OnMouseUp(wxMouseEvent& event)
{
	finishDrag();
	finishExtend();
}

void MidiFrame::OnMouseLeave(wxMouseEvent& event)
{
	finishDrag();
	finishExtend();
}

void MidiFrame::finishDrag()
{
	draggedObj = nullptr;
}

void MidiFrame::finishExtend()
{
	shouldExtend = false;
}

void MidiFrame::sendNoteAddedEvent()
{
	wxCommandEvent event(CANVAS_RECT_ADDED, GetId());
	event.SetEventObject(this);
	//event.SetString(rectTitle);

	ProcessWindowEvent(event);
}

void MidiFrame::sendNoteRemovedEvent()
{
	wxCommandEvent event(CANVAS_RECT_REMOVED, GetId());
	event.SetEventObject(this);
	//event.SetString(rectTitle);

	ProcessWindowEvent(event);
}