#include <vector>
#include <wx/wx.h>

#include "TrackManager.h"

TrackManager::TrackManager()
{
	
	
}

void TrackManager::addTrack(TrackFrame* frame)
{
	trackList.push_back(frame);
}

TrackFrame* TrackManager::getTrackFrame()
{
	
	return trackList[index];
}

void TrackManager::setIndex(int index)
{
	this->index = index;
}

int TrackManager::getIndex()
{
	return index;
}