#include "midiFrame.h"
#include <wx/graphics.h>
#include <wx/dcbuffer.h>

wxDEFINE_EVENT(CANVAS_RECT_ADDED, wxCommandEvent);
wxDEFINE_EVENT(CANVAS_RECT_REMOVED, wxCommandEvent);

MidiFrame::MidiFrame(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxWindow(parent, id, pos, size)
{
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);

	this->Bind(wxEVT_PAINT, &MidiFrame::OnPaint, this);
	this->Bind(wxEVT_LEFT_DOWN, &MidiFrame::OnMouseDown, this);
	this->Bind(wxEVT_MOTION, &MidiFrame::OnMouseMove, this);
	this->Bind(wxEVT_LEFT_UP, &MidiFrame::OnMouseUp, this);
	this->Bind(wxEVT_LEAVE_WINDOW, &MidiFrame::OnMouseLeave, this);
	this->Bind(wxEVT_LEFT_DCLICK, &MidiFrame::OnMouseEvent, this);

	
	addNote(this->FromDIP(100), this->FromDIP(80), this->FromDIP(210), this->FromDIP(140), *wxRED, "Note #1");
	addNote(this->FromDIP(130), this->FromDIP(110), this->FromDIP(280), this->FromDIP(210), *wxBLUE, "Note #2");
	addNote(this->FromDIP(110), this->FromDIP(110), this->FromDIP(300), this->FromDIP(120), wxColor(255, 0, 255, 120), "Note #3");

	this->draggedObj = nullptr;
	this->shouldExtend = false;

	this->selected = false;
}


void MidiFrame::addNote(int width, int height, int centerX, int centerY, wxColor color, const std::string& text)
{
	GraphicMIDIEvent obj{
		{-width / 2.0,
		-height / 2.0,
		static_cast<double>(width),
		static_cast<double>(height)},
		color,
		text,
		{}
	};
	obj.transform.Translate(static_cast<double>(centerX), static_cast<double>(centerY));
	
	
	this->noteList.push_back(obj);
	
	sendNoteAddedEvent(text);
	Refresh();
}

void MidiFrame::removeTopNote()
{
	if (!this->noteList.empty() && draggedObj == nullptr)
	{
		auto text = this->noteList.back().text;
		this->noteList.pop_back();

		sendNoteRemovedEvent(text);
		Refresh();
	}
}

//Removes Note when the note is double clicked
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

			gc->SetFont(*wxNORMAL_FONT, *wxWHITE);

			
			double textWidth, textHeight;
			gc->GetTextExtent(object.text, &textWidth, &textHeight);

			gc->DrawText(object.text, object.note.m_x + object.note.m_width / 2.0 - textWidth / 2.0, object.note.m_y + object.note.m_height / 2.0 - textHeight / 2.0);

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
			//doesnt work
			auto extendVector = event.GetPosition() - lastDragOrigin;
			auto inv = draggedObj->transform;

			inv.Invert();
			extendVector = inv.TransformDistance(extendVector);
			draggedObj.get().transformation.scaleX(extendVector.m_x/2, extendVector.m_y/2);
			
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

void MidiFrame::sendNoteAddedEvent(const wxString& rectTitle)
{
	wxCommandEvent event(CANVAS_RECT_ADDED, GetId());
	event.SetEventObject(this);
	event.SetString(rectTitle);

	ProcessWindowEvent(event);
}

void MidiFrame::sendNoteRemovedEvent(const wxString& rectTitle)
{
	wxCommandEvent event(CANVAS_RECT_REMOVED, GetId());
	event.SetEventObject(this);
	event.SetString(rectTitle);

	ProcessWindowEvent(event);
}