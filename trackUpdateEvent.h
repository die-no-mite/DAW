#pragma once

#include <wx/wx.h>

class TrackUpdateEvent;

wxDECLARE_EVENT(EVT_UPDATE_TRACK, TrackUpdateEvent);

class TrackUpdateEvent : public wxCommandEvent
{
public:
	TrackUpdateEvent(wxEventType commandType = EVT_UPDATE_TRACK, int id = 0)
		: wxCommandEvent(commandType, id) {}

	TrackUpdateEvent(const TrackUpdateEvent& event)
		: wxCommandEvent(event), trackNumber(event.trackNumber) {}

	wxEvent* Clone() const override { return new TrackUpdateEvent(*this); }

	int GetTrackNumber() const { return trackNumber; }
	void SetTrackNumber(int value) { trackNumber = value; }

private:
	int trackNumber = 0;
};