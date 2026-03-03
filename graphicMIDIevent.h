#pragma once


#include "objectspace.h"
#include "transformations.h"

struct GraphicMIDIEvent
{
	
	

	GraphicMIDIEvent(wxRect2DDouble note, wxColor color, wxString text, wxAffineMatrix2D transform, Transformation transformation = {})
		: boundingbox{ rect.rect }, transformation{ transformation } {}

	const wxRect2DDouble boundingbox;
	Transformation transformation;
};