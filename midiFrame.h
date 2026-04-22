#pragma once

#include <wx/wx.h>

#include <list>

#include "graphicMIDIevent.h"

wxDECLARE_EVENT(CANVAS_RECT_ADDED, wxCommandEvent);
wxDECLARE_EVENT(CANVAS_RECT_REMOVED, wxCommandEvent);
wxDECLARE_EVENT(UPDATE_NOTE, wxCommandEvent);
wxDECLARE_EVENT(FINISH_UPDATE_NOTE, wxCommandEvent);
wxDECLARE_EVENT(REMOVED_NOTE, wxCommandEvent);
wxDECLARE_EVENT(RELATIVE_POSITION, wxCommandEvent);
wxDECLARE_EVENT(SCALE_EVENT, wxCommandEvent);



class MidiFrame : public wxScrolled<wxPanel>
{
public:
	MidiFrame(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size);
	
	virtual ~MidiFrame(){}

	void addNote(int width, int height, int centerX, int centerY, wxColor color);
	void addNote(int width, int height, int centerX, int centerY, wxColor color, int ID);
	void removeTopNote();

	int getObjectCount() { return noteList.size(); }

	void FlipGridFlag();
	void SetDivision(int newdivision);
	int GetCurrentID();
	wxRealPoint GetCoords();
	int GetDivision();
	void sendUpdateTrack();
	void sendNoteAddedEvent();

	void SetGridAnchor(int x, int y);
	void ClearGridAnchor();
	bool HasGridAnchor() const;
	int GetGridAnchorX() const;
	int GetGridAnchorY() const;
	void ShiftNotes(char direction, int stepx, int stepy);
	wxRealPoint GetShiftedCoords();

	bool shouldExtend;

private:
	
	void OnPaint(wxPaintEvent& evt);
	void OnMouseDown(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseUp(wxMouseEvent& event);
	void OnMouseLeave(wxMouseEvent& event);
	void OnMouseEvent(wxMouseEvent& event);

	void finishDrag();
	void finishExtend();

	void sendNoteRemovedEvent(int ID);
	void sendUpdateNoteEvent();
	void finishUpdateNoteEvent();
	void sendRelativePositionEvent();
	void setShiftedCoords(double x, double y);


	int division = 0;

	std::list<GraphicMIDIEvent> noteList;
	
	GraphicMIDIEvent *draggedObj;

	bool selected;
	bool gridFlag = false;
	bool gridSnap = false;
	bool shouldMove;

	bool hasGridAnchor = false;
	int gridAnchorX = 0;
	int gridAnchorY = 0;
	int xShift = 0;
	int yShift = 0;
	int xStep = 0;
	int yStep = 0;
	wxRealPoint shiftedCoords;

	wxPoint2DDouble lastDragOrigin;
	int lastDraggedID = -1;
	
};