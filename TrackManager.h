#pragma once

#include "trackFrame.h"

#include <wx/wx.h>

class TrackManager
{
public:
	TrackManager();
	

	void addTrack(TrackFrame* frame);
	TrackFrame* getTrackFrame();
	void setIndex(int index);
	int getIndex();


private:
	
	int index;

	std::vector<TrackFrame*> trackList;

};