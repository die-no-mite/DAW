
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
	evt.Skip();
	if (!this->noteList.empty())
	{
		this->noteList.pop_back();

		Refresh();
	}
}

void TrackFrame::ClearTrack()
{
	this->noteList.clear();
	Refresh();
}