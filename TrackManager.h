#pragma once

#include "trackFrame.h"

#include <wx/wx.h>

class TrackManager
{
public:
	TrackManager(int totalTracks);
	

	void addTrack(TrackFrame* frame);
	TrackFrame* getTrackFrame();
	void setIndex(int index);
	int getIndex();


private:
	int totalTracks;
	int index;

	std::vector<TrackFrame*> trackList;

};