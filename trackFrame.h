#pragma once

#include <wx/wx.h>

#include <list>

#include "graphicMIDIevent.h"

class TrackFrame : public wxPanel
{
public:
	TrackFrame(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size);

	virtual ~TrackFrame() {}
private:

	void OnDoubleClick(wxMouseEvent& evt);
	void OnPaint(wxPaintEvent& evt);
	void addNote(int width, int height, int x, int y);
	void removeTopNote(wxMouseEvent& evt);
	void OnMouseDown(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);

	GraphicMIDIEvent* draggedObj;

	std::list<GraphicMIDIEvent> noteList;
	wxPoint2DDouble lastDragOrigin;
};