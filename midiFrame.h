#pragma once

#include <wx/wx.h>
#include <list>

#include "graphicMIDIevent.h"

wxDECLARE_EVENT(CANVAS_RECT_ADDED, wxCommandEvent);
wxDECLARE_EVENT(CANVAS_RECT_REMOVED, wxCommandEvent);

class MidiFrame : public wxWindow
{
public:
	MidiFrame(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size);
	
	virtual ~MidiFrame(){}

	void addNote(int width, int height, int centerX, int centerY, wxColor color, const std::string& text);
	void removeTopNote();

	int getObjectCount() { return noteList.size(); }
private:
	
	void OnPaint(wxPaintEvent& evt);
	void OnMouseDown(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseUp(wxMouseEvent& event);
	void OnMouseLeave(wxMouseEvent& event);
	void OnMouseEvent(wxMouseEvent& event);

	void finishDrag();
	void finishExtend();

	void sendNoteAddedEvent(const wxString& noteTitle);
	void sendNoteRemovedEvent(const wxString& noteTitle);

	std::list<GraphicMIDIEvent> noteList;
	
	GraphicMIDIEvent* draggedObj;
	bool shouldExtend;
	bool selected;

	wxPoint2DDouble lastDragOrigin;
};