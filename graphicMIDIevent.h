#pragma once


#include "objectspace.h"
#include "transformations.h"
#include "rect.h"

struct GraphicMIDIEvent
{
	wxRect2DDouble note;
	wxColor color;
	wxAffineMatrix2D transform;
	Rect rect;
	Transformation transformation;
	
	GraphicMIDIEvent(wxRect2DDouble tnote, wxColor tcolor, wxAffineMatrix2D ttransform, Transformation transformation = {})
		: boundingBox{ rect.rect }, transformation{ transformation }
	{
		note = tnote;
		color = tcolor;
		transform = ttransform;
	}

	const wxRect2DDouble boundingBox;
	
	
};