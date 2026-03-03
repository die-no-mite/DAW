#pragma once

#include <optional>

#include <wx/wx.h>

#include "graphicMIDIEvent.h"
#include "objectspace.h"

struct SelectionBox
{
    SelectionBox(GraphicMIDIEvent& object, double handleW) : object{ object }, handleWidth(handleW) {}

    std::reference_wrapper<GraphicMIDIEvent> object;


    void StartDragIfClicked(wxPoint2DDouble pt);
    bool IsDragging() const;
    void Drag(wxPoint2DDouble pt);
    void FinishDrag();

private:
    wxPoint2DDouble GetRotationHandleStart() const;
    wxPoint2DDouble GetRotationHandleCenter() const;

    wxPoint2DDouble GetTopLeftHandleCenter() const;
    wxPoint2DDouble GetTopRightHandleCenter() const;
    wxPoint2DDouble GetBottomRightHandleCenter() const;
    wxPoint2DDouble GetBottomLeftHandleCenter() const;

    

    bool HandleHitTest(wxPoint2DDouble pt, wxPoint2DDouble handleCenter) const;
    bool FullBoxHitTest(wxPoint2DDouble pt) const;

    void ScaleUsingHandleMovement(wxPoint2DDouble dragStart, wxPoint2DDouble dragEnd, wxPoint2DDouble handleCenter);
    void RotateUsingMovement(wxPoint2DDouble dragStart, wxPoint2DDouble dragEnd);
    void TranslateUsingMovement(wxPoint2DDouble dragStart, wxPoint2DDouble dragEnd);

    enum class DraggableElement;

    std::optional<DraggableElement> draggedElement{};
    wxPoint2DDouble lastDragPoint{};
    double handleWidth;
};
