
#include "midiFrame.h"

#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <fstream>


wxDEFINE_EVENT(CANVAS_RECT_ADDED, wxCommandEvent);
wxDEFINE_EVENT(CANVAS_RECT_REMOVED, wxCommandEvent);
wxDEFINE_EVENT(UPDATE_NOTE, wxCommandEvent);
wxDEFINE_EVENT(FINISH_UPDATE_NOTE, wxCommandEvent);
wxDEFINE_EVENT(REMOVED_NOTE, wxCommandEvent);
wxDEFINE_EVENT(RELATIVE_POSITION, wxCommandEvent);



MidiFrame::MidiFrame(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxScrolled<wxPanel>(parent, id, pos, size)
{
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);

	this->Bind(wxEVT_PAINT, &MidiFrame::OnPaint, this);
	this->Bind(wxEVT_LEFT_DOWN, &MidiFrame::OnMouseDown, this);
	this->Bind(wxEVT_MOTION, &MidiFrame::OnMouseMove, this);
	this->Bind(wxEVT_LEFT_UP, &MidiFrame::OnMouseUp, this);
	this->Bind(wxEVT_LEAVE_WINDOW, &MidiFrame::OnMouseLeave, this);
	this->Bind(wxEVT_RIGHT_DOWN, &MidiFrame::OnMouseDown, this);
	this->Bind(wxEVT_RIGHT_UP, &MidiFrame::OnMouseEvent, this);

	this->draggedObj = nullptr;
	this->shouldExtend = false;

	this->selected = false;
}


void MidiFrame::addNote(int width, int height, int centerX, int centerY, wxColor color)
{
	GraphicMIDIEvent obj{
		{static_cast<double>(centerX),
		static_cast<double>(centerY),
		static_cast<double>(width),
		static_cast<double>(height)},
		color, 0,
		{}
	};
	//obj.transform.Translate(static_cast<double>(centerX), static_cast<double>(centerY));
	
	
	this->noteList.push_back(obj);
	
	sendNoteAddedEvent();
	Refresh();
}

//overloaded function that takes in a note ID as well
void MidiFrame::addNote(int width, int height, int centerX, int centerY, wxColor color, int ID)
{
	GraphicMIDIEvent obj{
		{static_cast<double>(centerX),
		static_cast<double>(centerY),
		static_cast<double>(width),
		static_cast<double>(height)},
		color, ID,
		{}
	};
	//obj.transform.Translate(static_cast<double>(centerX), static_cast<double>(centerY));


	this->noteList.push_back(obj);

	sendNoteAddedEvent();
	Refresh();
}

void MidiFrame::removeTopNote()
{
	if (!this->noteList.empty() && draggedObj == nullptr)
	{
			
		this->noteList.pop_back();
		
		Refresh();
	}
}

//Removes Note when the note is right clicked
void MidiFrame::OnMouseEvent(wxMouseEvent &evt)
{

	sendNoteRemovedEvent(lastDraggedID);
	finishDrag();
	finishExtend();
	removeTopNote();
}

void MidiFrame::OnPaint(wxPaintEvent& evt)
{
	
	wxAutoBufferedPaintDC dc(this);
	dc.Clear();

	wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

	if (gc)
	{
		if (gridFlag)
		{
			if (tempo != 0)
			{ 
				gc->SetBrush(wxBrush(wxColor(0, 0, 0)));
				for (int i = 24; i < 10000; i += ((tempo+25)/8))
				{
					
					gc->DrawRectangle(i, 0, 1, 600);
					
				}
				for (int i = -12; i < 10000; i += 20)
				{
					
					gc->DrawRectangle(0, i - 170, 1200, 1);
				}
			}
			
		}
		for (const auto& object : noteList) 
		{
			
			gc->SetTransform(gc->CreateMatrix(object.transform));
			
			gc->SetBrush(wxBrush(object.color));
			
			gc->DrawRectangle(object.note.m_x, object.note.m_y, object.note.m_width, object.note.m_height);
			
		}
		
		
	}
	delete gc;
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
		lastDraggedID = draggedObj->noteID;
		
		lastDragOrigin = event.GetPosition();
		shouldExtend = wxGetKeyState(WXK_ALT);

		Refresh(); // for z order reversal
		sendRelativePositionEvent();
		if (event.GetButton() == wxMOUSE_BTN_LEFT)
			shouldMove = true;
		else
			shouldMove = false;
	}
	
}

void MidiFrame::OnMouseMove(wxMouseEvent& event)
{
	
	if (shouldMove)
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
			}
			lastDragOrigin = event.GetPosition();
			Refresh();
		
		}
	}
	
}


void MidiFrame::OnMouseUp(wxMouseEvent& event)
{
	
	if(draggedObj != nullptr)
	{ 
		sendUpdateNoteEvent();
		finishDrag();
		finishExtend();
		finishUpdateNoteEvent();
	}
	
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

void MidiFrame::sendUpdateNoteEvent()
{
	wxCommandEvent event(UPDATE_NOTE, GetId());

	event.SetEventObject(this);

	ProcessWindowEvent(event);
}

void MidiFrame::finishUpdateNoteEvent()
{
	wxCommandEvent event(FINISH_UPDATE_NOTE, GetId());

	event.SetEventObject(this);

	ProcessWindowEvent(event);
}



void MidiFrame::sendNoteAddedEvent()
{
	wxCommandEvent event(CANVAS_RECT_ADDED, GetId());
	event.SetEventObject(this);
	
	//event.SetString(rectTitle);

	ProcessWindowEvent(event);
}

void MidiFrame::sendNoteRemovedEvent(int ID)
{
	if (ID != -1)
	{
		wxCommandEvent event(REMOVED_NOTE, GetId());
		event.SetEventObject(this);
		event.SetInt(ID);

		ProcessWindowEvent(event);
	}
}

void MidiFrame::sendRelativePositionEvent()
{
	wxCommandEvent event(RELATIVE_POSITION, GetId());
	event.SetEventObject(this);

	ProcessWindowEvent(event);
}

void MidiFrame::FlipGridFlag()
{
	if (gridFlag)
		gridFlag = false;
	else
		gridFlag = true;
}

void MidiFrame::SetTempo(int newtempo)
{
	tempo = newtempo;
}

int MidiFrame::GetTempo()
{
	return tempo;
}

int MidiFrame::GetCurrentID()
{
	return draggedObj->noteID;
}

wxRealPoint MidiFrame::GetCoords()
{
	wxRealPoint coords(lastDragOrigin.m_x, lastDragOrigin.m_y);
	return coords;
}