#include "midiFrame.h"
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <fstream>


wxDEFINE_EVENT(CANVAS_RECT_ADDED, wxCommandEvent);
wxDEFINE_EVENT(CANVAS_RECT_REMOVED, wxCommandEvent);
wxDEFINE_EVENT(UPDATE_NOTE, wxCommandEvent);

MidiFrame::MidiFrame(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxScrolled<wxPanel>(parent, id, pos, size)
{
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);

	this->Bind(wxEVT_PAINT, &MidiFrame::OnPaint, this);
	this->Bind(wxEVT_LEFT_DOWN, &MidiFrame::OnMouseDown, this);
	this->Bind(wxEVT_RIGHT_DOWN, &MidiFrame::OnMouseDown, this);
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
		color, 0,
		{}
	};
	obj.transform.Translate(static_cast<double>(centerX), static_cast<double>(centerY));
	
	
	this->noteList.push_back(obj);
	
	sendNoteAddedEvent();
	Refresh();
}

//overloaded function that takes in a note ID as well
void MidiFrame::addNote(int width, int height, int centerX, int centerY, wxColor color, int ID)
{
	GraphicMIDIEvent obj{
		{-width / 2.0,
		-height / 2.0,
		static_cast<double>(width),
		static_cast<double>(height)},
		color, ID,
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
		if (gridFlag)
		{
			if (tempo != 0)
			{ 
				gc->SetBrush(wxBrush(wxColor(0, 0, 0)));
				for (int i = 10; i < 10000; i += tempo)
				{
					
					gc->DrawRectangle(i, 0, 1, 600);
					
				}
				for (int i = 20; i < 10000; i += 20)
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
		std::ofstream file;
		file.open("output.txt");
		file << event.GetPosition().x << " " << event.GetPosition().y << std::endl;
		
		file.close();
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

void MidiFrame::SnapToGrid()
{
	if(draggedObj != nullptr)
	{ 
		/*
		wxDouble snapX = draggedObj->note.m_x;
		wxDouble snapY = draggedObj->note.m_y;
		wxPoint2DDouble snapVector(snapX, snapY);

		wxDouble targetX = std::round(lastDragOrigin.m_x / (tempo / 16)) * (tempo / 16);
		wxDouble targetY = std::round(lastDragOrigin.m_y / 50) * 50;
		wxPoint2DDouble targetVector(targetX, targetY);

		

		snapVector = targetVector - snapVector;
		
		auto inv = draggedObj->transform;
		inv.Invert();

		targetVector = inv.TransformDistance(targetVector);

		draggedObj->transform.Translate(targetVector.m_x, targetVector.m_y);
		*/

		wxDouble width = draggedObj->note.m_x * 2;
		wxDouble height = draggedObj->note.m_y * 2;

		removeTopNote();
		addNote(width, height, std::round(lastDragOrigin.m_x / (tempo / 8)) * (tempo / 8), std::round(lastDragOrigin.m_y / 20) * 20, wxColor(255,255,255));


		Refresh();
		
	}
	
}

void MidiFrame::OnMouseUp(wxMouseEvent& event)
{
	SnapToGrid();
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

void MidiFrame::sendUpdateNoteEvent()
{
	wxCommandEvent event(CANVAS_RECT_ADDED, GetId());
	event.SetEventObject(this);
	event.SetInt(draggedObj->noteID);
	ProcessWindowEvent(event);
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