#pragma once


struct GraphicMIDIEvent
{
	wxRect2DDouble note;
	wxColor color;
	int noteID;
	wxAffineMatrix2D transform;
	
	
	GraphicMIDIEvent(wxRect2DDouble tnote, wxColor tcolor, int ID, wxAffineMatrix2D ttransform)
		
	{
		note = tnote;
		color = tcolor;
		transform = ttransform;
		noteID = ID;
	}

	const wxRect2DDouble boundingBox;
	
	
};