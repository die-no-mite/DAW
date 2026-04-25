
#include "midiFrame.h"

#include <wx/graphics.h>
#include <wx/dcbuffer.h>

#include <fstream>
#include <algorithm>


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
	file.open("output.txt");
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

	setShiftedCoords(obj.note.m_x, obj.note.m_y);

	this->noteList.push_back(obj);

	sendNoteAddedEvent();
	Refresh();
}

void MidiFrame::setShiftedCoords(double x, double y)
{
	shiftedCoords.x = x;
	shiftedCoords.y = y;
}

wxRealPoint MidiFrame::GetShiftedCoords()
{
	return shiftedCoords;
}

int MidiFrame::GetShiftY()
{
	return (yShift * yStep);
}

int MidiFrame::GetShiftX()
{
	return (xShift * xStep);
}

void MidiFrame::removeTopNote()
{
	if (!this->noteList.empty() && draggedObj == nullptr)
	{
			
		this->noteList.pop_back();
		
		Refresh();
	}
}

void MidiFrame::removeNoteByID(int ID)
{
	auto it = std::find_if(noteList.begin(), noteList.end(),
		[ID](const GraphicMIDIEvent& obj)
		{
			return obj.noteID == ID;
		});

	if (it != noteList.end())
	{
		if (draggedObj == &(*it))
			draggedObj = nullptr;

		noteList.erase(it);
		Refresh();
	}
}

//Removes Note when the note is right clicked
void MidiFrame::OnMouseEvent(wxMouseEvent &evt)
{
	evt.Skip();
	sendNoteRemovedEvent(lastDraggedID);
	finishDrag();
	finishExtend();
	removeTopNote();
}

wxPoint MidiFrame::GetWorldMousePosition(const wxMouseEvent& event) const
{
	int worldX = 0;
	int worldY = 0;
	const_cast<MidiFrame*>(this)->CalcUnscrolledPosition(event.GetX(), event.GetY(), &worldX, &worldY);
	return wxPoint(worldX, worldY);
}

void MidiFrame::OnPaint(wxPaintEvent& evt)
{
	
	wxAutoBufferedPaintDC dc(this);
	PrepareDC(dc);
	dc.Clear();

	wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
	if (!gc)
	{
		return;
	}

	if (gridFlag && division != 0 && hasGridAnchor)
	{
		const int stepX = std::max(1, (division / 20));
		const int stepY = 17;
		const wxSize virtualSize = GetVirtualSize();

		gc->SetBrush(wxBrush(wxColor(0, 0, 0)));

		for (int x = gridAnchorX; x < virtualSize.GetWidth(); x += stepX)
		{
			gc->DrawRectangle(FromDIP(x), 0, 1, FromDIP(virtualSize.GetHeight()));
		}
		for (int x = gridAnchorX - stepX; x >= 0; x -= stepX)
		{
			gc->DrawRectangle(FromDIP(x), 0, 1, FromDIP(virtualSize.GetHeight()));
		}

		for (int y = gridAnchorY; y < virtualSize.GetHeight(); y += stepY)
		{
			gc->DrawRectangle(0, FromDIP(y), FromDIP(virtualSize.GetWidth()), 1);
		}
		for (int y = gridAnchorY - stepY; y >= 0; y -= stepY)
		{
			gc->DrawRectangle(0, FromDIP(y), FromDIP(virtualSize.GetWidth()), 1);
		}
	}

	for (const auto& object : noteList)
	{
		gc->SetTransform(gc->CreateMatrix(object.transform));
		gc->SetBrush(wxBrush(object.color));
		gc->DrawRectangle(FromDIP((object.note.m_x + 1) + (xShift * xStep)), FromDIP((object.note.m_y + 1) + (yShift * yStep)), FromDIP(object.note.m_width - 1), FromDIP(object.note.m_height - 1));
	}

	delete gc;
}

void MidiFrame::ShiftNotes(char direction, int stepx, int stepy)
{
	if (direction == 'l')
	{
		if (xShift < 0)
			xShift++;
	}
	else if (direction == 'r')
	{
		
		xShift--;
	}
	else if (direction == 'u')
	{
		yShift++;
	}
	else if (direction == 'd')
	{
		yShift--;
	}
	xStep = stepx;
	yStep = stepy;
	
	Refresh();
}

void MidiFrame::OnMouseDown(wxMouseEvent& event)
{
	event.Skip();

	const wxPoint worldPos = GetWorldMousePosition(event);

	auto clickedObjectIter = std::find_if(noteList.rbegin(), noteList.rend(), [worldPos, this](const GraphicMIDIEvent& o)
		{
			wxPoint2DDouble clickPos(worldPos.x, worldPos.y);
			auto inv = o.transform;
			inv.Invert();
			clickPos = inv.TransformPoint(clickPos);
			clickPos.m_x -= (xShift * xStep);
			clickPos.m_y -= (yShift * yStep);
			return o.note.Contains(clickPos);
		});

	if (clickedObjectIter != noteList.rend())
	{
		auto forwardIt = std::prev(clickedObjectIter.base());

		noteList.push_back(*forwardIt);
		noteList.erase(forwardIt);

		draggedObj = &(*std::prev(noteList.end()));
		lastDraggedID = draggedObj->noteID;

		lastDragOrigin = wxPoint2DDouble(worldPos.x, worldPos.y);
		shouldExtend = wxGetKeyState(WXK_ALT);

		Refresh();

		setShiftedCoords(worldPos.x - (xShift * xStep), worldPos.y - (yShift * yStep));
		sendRelativePositionEvent();

		shouldMove = (event.GetButton() == wxMOUSE_BTN_LEFT);
	}
}

void MidiFrame::OnMouseMove(wxMouseEvent& event)
{
	event.Skip();

	if (shouldMove && draggedObj != nullptr)
	{
		const wxPoint worldPos = GetWorldMousePosition(event);

		setShiftedCoords(worldPos.x - (xShift * xStep), worldPos.y - (yShift * yStep));

		if (!shouldExtend)
		{
			wxPoint2DDouble dragVector(
				static_cast<double>(worldPos.x) - lastDragOrigin.m_x,
				static_cast<double>(worldPos.y) - lastDragOrigin.m_y);

			auto inv = draggedObj->transform;
			inv.Invert();
			dragVector = inv.TransformDistance(dragVector);

			draggedObj->transform.Translate(dragVector.m_x, dragVector.m_y);
		}

		lastDragOrigin = wxPoint2DDouble(worldPos.x, worldPos.y);
		Refresh();
	}
}


void MidiFrame::OnMouseUp(wxMouseEvent& event)
{
	event.Skip();

	if (draggedObj != nullptr)
	{
		const wxPoint worldPos = GetWorldMousePosition(event);
		setShiftedCoords(worldPos.x - (xShift * xStep), worldPos.y - (yShift * yStep));

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

void MidiFrame::SetDivision(int newdivision)
{
	division = newdivision;
}

int MidiFrame::GetDivision()
{
	return division;
}

int MidiFrame::GetCurrentID()
{
	return lastDraggedID;
}

wxRealPoint MidiFrame::GetCoords()
{
	wxRealPoint coords(lastDragOrigin.m_x, lastDragOrigin.m_y);
	return coords;
}

void MidiFrame::SetGridAnchor(int x, int y)
{
	hasGridAnchor = true;
	gridAnchorX = x;
	gridAnchorY = y;
	Refresh();
}

void MidiFrame::ClearGridAnchor()
{
	hasGridAnchor = false;
	gridAnchorX = 0;
	gridAnchorY = 0;
	Refresh();
}

bool MidiFrame::HasGridAnchor() const
{
	return hasGridAnchor;
}

int MidiFrame::GetGridAnchorX() const
{
	return gridAnchorX;
}

int MidiFrame::GetGridAnchorY() const
{
	return gridAnchorY;
}