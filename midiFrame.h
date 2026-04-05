#pragma once

#include <wx/wx.h>

#include <list>

#include "graphicMIDIevent.h"

wxDECLARE_EVENT(CANVAS_RECT_ADDED, wxCommandEvent);
wxDECLARE_EVENT(CANVAS_RECT_REMOVED, wxCommandEvent);
wxDECLARE_EVENT(UPDATE_NOTE, wxCommandEvent);
wxDECLARE_EVENT(FINISH_UPDATE_NOTE, wxCommandEvent);


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
	void SetTempo(int newtempo);
	int GetCurrentID();
	wxRealPoint GetCoords();
	int GetTempo();
	void sendUpdateTrack();
	void sendNoteAddedEvent();



private:
	
	void OnPaint(wxPaintEvent& evt);
	void OnMouseDown(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseUp(wxMouseEvent& event);
	void OnMouseLeave(wxMouseEvent& event);
	void OnMouseEvent(wxMouseEvent& event);

	void finishDrag();
	void finishExtend();
	void SnapToGrid();
	void removeSecondNote();

	void sendNoteRemovedEvent();
	void sendUpdateNoteEvent();
	void finishUpdateNoteEvent();


	int tempo = 0;

	std::list<GraphicMIDIEvent> noteList;
	
	GraphicMIDIEvent *draggedObj;

	bool shouldExtend;
	bool selected;
	bool gridFlag = false;
	bool gridSnap = false;

	wxPoint2DDouble lastDragOrigin;
	
};