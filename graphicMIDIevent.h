#pragma once


struct GraphicMIDIEvent
{
	wxRect2DDouble note;
	wxColor color;
	wxAffineMatrix2D transform;
	
	
	GraphicMIDIEvent(wxRect2DDouble tnote, wxColor tcolor, wxAffineMatrix2D ttransform)
		
	{
		note = tnote;
		color = tcolor;
		transform = ttransform;
	}

	const wxRect2DDouble boundingBox;
	
	
};