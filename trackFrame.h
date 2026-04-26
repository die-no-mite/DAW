#pragma once

#include <wx/wx.h>

#include <list>

#include "graphicMIDIevent.h"

class TrackFrame : public wxPanel
{
public:
	TrackFrame(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size);

	virtual ~TrackFrame() {}

	void addNote(int width, int height, int x, int y);
	void ClearTrack();

private:

	void OnDoubleClick(wxCommandEvent& evt);
	void OnPaint(wxPaintEvent& evt);
	void removeTopNote(wxMouseEvent& evt);
	
	
	GraphicMIDIEvent* draggedObj;

	std::list<GraphicMIDIEvent> noteList;
	wxPoint2DDouble lastDragOrigin;
};