
#include "trackFrame.h"
#include "editorFrame.h"

#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <wx/popupwin.h>


TrackFrame::TrackFrame(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxPanel(parent, id, pos, size)
{
	this->SetBackgroundStyle(wxBG_STYLE_PAINT);

	this->Bind(wxEVT_PAINT, &TrackFrame::OnPaint, this);
	this->Bind(wxEVT_RIGHT_DOWN, &TrackFrame::removeTopNote, this);

}

void TrackFrame::OnDoubleClick(wxCommandEvent& evt)
{
	
}

void TrackFrame::OnPaint(wxPaintEvent& evt)
{
	
	wxAutoBufferedPaintDC dc(this);
	dc.Clear();

	wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

	if (gc)
	{
		for (const auto& object : noteList) {

			gc->SetTransform(gc->CreateMatrix(object.transform));

			gc->SetBrush(wxBrush(object.color));
			gc->DrawRectangle(object.note.m_x, object.note.m_y, object.note.m_width, object.note.m_height);
			

		}

		gc->SetBrush(wxBrush(wxColor(0, 0, 0)));
		gc->DrawRectangle(10, 0, 5, 300);
		delete gc;

	}
	
}

void TrackFrame::addNote(int width, int height, int centerX, int centerY)
{
	
	GraphicMIDIEvent obj{
		{-width / 2.0,
		-height / 2.0,
		static_cast<double>(width),
		static_cast<double>(height)},
		wxColor(255,255,255), 0,
		{}
	};
	obj.transform.Translate(static_cast<double>(centerX), static_cast<double>(centerY));


	this->noteList.push_back(obj);

	Refresh();
}

void TrackFrame::removeTopNote(wxMouseEvent& evt)
{
	if (!this->noteList.empty())
	{
		this->noteList.pop_back();

		Refresh();
	}
}

void TrackFrame::OnMouseDown(wxMouseEvent& event)
{
	auto clickedObjectIter = std::find_if(noteList.rbegin(), noteList.rend(), [event](const GraphicMIDIEvent& o)
		{
			wxPoint2DDouble clickPos = event.GetPosition();
			auto inv = o.transform;
			inv.Invert();
			clickPos = inv.TransformPoint(clickPos);
			return o.note.Contains(clickPos);
		});

	if (clickedObjectIter != noteList.rend())
	{
		auto forwardIt = std::prev(clickedObjectIter.base());

		noteList.push_back(*forwardIt);
		noteList.erase(forwardIt);

		draggedObj = &(*std::prev(noteList.end()));

		lastDragOrigin = event.GetPosition();


		Refresh(); // for z order reversal
	}
}

void TrackFrame::OnMouseMove(wxMouseEvent& event)
{
	if (draggedObj != nullptr)
	{
	
		auto dragVector = event.GetPosition() - lastDragOrigin;

		auto inv = draggedObj->transform;
		inv.Invert();
		dragVector = inv.TransformDistance(dragVector);

		draggedObj->transform.Translate(dragVector.m_x, dragVector.m_y);


		lastDragOrigin = event.GetPosition();
		Refresh();
		
	}
}

void TrackFrame::OnMouseUp(wxMouseEvent& event)
{
	finishDrag();
	
}

void TrackFrame::OnMouseLeave(wxMouseEvent& event)
{
	finishDrag();
	
}

void TrackFrame::finishDrag()
{
	draggedObj = nullptr;
}

