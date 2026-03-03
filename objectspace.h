#pragma once

struct GraphicMIDIEvent;
class wxPoint2DDouble;
class wxAffineMatrix2D;

namespace ObjectSpace
{
    wxPoint2DDouble ToObjectCoordinates(const GraphicMIDIEvent& object, wxPoint2DDouble point);
    wxPoint2DDouble ToObjectDistance(const GraphicMIDIEvent& object, wxPoint2DDouble point);
    wxPoint2DDouble ToScreenCoordinates(const GraphicMIDIEvent& object, wxPoint2DDouble point);
    wxPoint2DDouble ToScreenDistance(const GraphicMIDIEvent& object, wxPoint2DDouble point);

    wxAffineMatrix2D GetTransformationMatrix(const GraphicMIDIEvent& object);
    wxAffineMatrix2D GetInverseTransformationMatrix(const GraphicMIDIEvent& object);
}