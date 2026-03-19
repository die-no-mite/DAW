#include <vector>
#include <wx/wx.h>

#include "TrackManager.h"

TrackManager::TrackManager(int totalTracks)
{
	this->totalTracks = totalTracks;
	
}

void TrackManager::addTrack(TrackFrame* frame)
{

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