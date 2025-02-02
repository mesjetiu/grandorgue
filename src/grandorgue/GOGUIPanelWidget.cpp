/*
 * GrandOrgue - free pipe organ simulator
 *
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2015 GrandOrgue contributors (see AUTHORS)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "GOGUIPanelWidget.h"

#include "GOGUIControl.h"
#include "GOGUIPanel.h"
#include "GOrgueDC.h"
#include "GOrgueKeyConvert.h"
#include <wx/dcclient.h>
#include <wx/dcmemory.h>

DEFINE_LOCAL_EVENT_TYPE(wxEVT_GOCONTROL)

BEGIN_EVENT_TABLE(GOGUIPanelWidget, wxPanel)
	EVT_ERASE_BACKGROUND(GOGUIPanelWidget::OnErase)
	EVT_PAINT(GOGUIPanelWidget::OnPaint)
	EVT_COMMAND(0, wxEVT_GOCONTROL, GOGUIPanelWidget::OnGOControl)
	EVT_MOTION(GOGUIPanelWidget::OnMouseMove)
	EVT_LEFT_DOWN(GOGUIPanelWidget::OnMouseLeftDown)
	EVT_LEFT_DCLICK(GOGUIPanelWidget::OnMouseLeftDown)
	EVT_RIGHT_DOWN(GOGUIPanelWidget::OnMouseRightDown)
	EVT_RIGHT_DCLICK(GOGUIPanelWidget::OnMouseRightDown)
	EVT_MOUSEWHEEL(GOGUIPanelWidget::OnMouseScroll)
	EVT_KEY_DOWN(GOGUIPanelWidget::OnKeyCommand)
	EVT_KEY_UP(GOGUIPanelWidget::OnKeyUp)
END_EVENT_TABLE()

GOGUIPanelWidget::GOGUIPanelWidget(GOGUIPanel* panel, wxWindow* parent, wxWindowID id) :
	wxPanel(parent, id),
	m_panel(panel),
	m_BGInit(false),
	m_Background(&m_BGImage),
	m_Scale(1)
{
	SetLabel(m_panel->GetName());
	m_ClientBitmap.Create(m_panel->GetWidth() * m_Scale, m_panel->GetHeight() * m_Scale);
	m_panel->PrepareDraw(m_Scale, NULL);
	OnUpdate();
	m_BGImage = m_ClientBitmap.ConvertToImage();
	m_Background.PrepareBitmap(m_Scale, wxRect(0, 0, 0, 0), NULL);
	m_BGInit = true;
	SetFocus();
}

GOGUIPanelWidget::~GOGUIPanelWidget()
{
}

wxSize GOGUIPanelWidget::UpdateSize(wxSize size)
{
	double scaleX = size.GetWidth() / (double)m_panel->GetWidth();
	double scaleY = size.GetHeight() / (double)m_panel->GetHeight();
	if (scaleX > scaleY)
		m_Scale = scaleX;
	else
		m_Scale = scaleY;
	if (m_Scale > 2)
		m_Scale = 2;
	m_panel->PrepareDraw(m_Scale, m_BGInit ? &m_Background : NULL);
	OnUpdate();
	Refresh();
	return GetSize();
}

void GOGUIPanelWidget::OnDraw(wxDC* dc)
{
	dc->DrawBitmap(m_ClientBitmap, 0, 0, false);
}

void GOGUIPanelWidget::OnErase(wxEraseEvent& event)
{
	wxDC* dc = event.GetDC();
	OnDraw(dc);
}

void GOGUIPanelWidget::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	OnDraw(&dc);
}

void GOGUIPanelWidget::OnUpdate()
{
	if (m_BGInit)
		m_ClientBitmap = (wxBitmap)m_BGImage.Scale(m_panel->GetWidth() * m_Scale + 0.5, m_panel->GetHeight() * m_Scale + 0.5, wxIMAGE_QUALITY_BICUBIC);
	else
		m_ClientBitmap = wxBitmap(m_panel->GetWidth() * m_Scale + 0.5, m_panel->GetHeight() * m_Scale + 0.5);
	wxMemoryDC dc;
	dc.SelectObject(m_ClientBitmap);
	GOrgueDC DC(&dc, m_Scale);

	m_panel->Draw(DC);
	SetSize(m_ClientBitmap.GetWidth(), m_ClientBitmap.GetHeight());
}

void GOGUIPanelWidget::CopyToScreen(wxDC* mdc, const wxRect& rect)
{
	wxClientDC dc(this);
	
	dc.Blit(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight(), mdc, rect.GetX(), rect.GetY());
}

void GOGUIPanelWidget::OnGOControl(wxCommandEvent& event)
{
	GOGUIControl* control = static_cast<GOGUIControl*>(event.GetClientData());

	wxMemoryDC mdc;
	mdc.SelectObject(m_ClientBitmap);
	GOrgueDC DC(&mdc, m_Scale);

	control->Draw(DC);
	CopyToScreen(&mdc, DC.ScaleRect(control->GetBoundingRect()));
	event.Skip();
}

bool GOGUIPanelWidget::ForwardMouseEvent(wxMouseEvent& event)
{
	if (GetClientRect().Contains(event.GetPosition()))
		return false;
	wxPoint pos = ClientToScreen(event.GetPosition());
	wxWindow* window = wxFindWindowAtPoint(pos);
	if (window)
	{
		wxMouseEvent e = event;
		e.SetPosition(window->ScreenToClient(pos));
		window->HandleWindowEvent(e);
	}
	return true;
}

void GOGUIPanelWidget::OnMouseMove(wxMouseEvent& event)
{
	if (!event.LeftIsDown())
	{
		m_panel->HandleMouseRelease(false);
		return;
	}

	if (ForwardMouseEvent(event))
		return;
	m_panel->HandleMousePress(event.GetX() / m_Scale, event.GetY() / m_Scale, false);
	event.Skip();
}

void GOGUIPanelWidget::OnMouseLeftDown(wxMouseEvent& event)
{
	if (ForwardMouseEvent(event))
		return;

	m_panel->HandleMouseRelease(false);

	m_panel->HandleMousePress(event.GetX() / m_Scale, event.GetY() / m_Scale, false);
	event.Skip();
}

void GOGUIPanelWidget::OnMouseRightDown(wxMouseEvent& event)
{
	m_panel->HandleMousePress(event.GetX() / m_Scale, event.GetY() / m_Scale, true);
	event.Skip();
}

void GOGUIPanelWidget::OnMouseScroll(wxMouseEvent& event)
{
	if (event.GetWheelRotation())
		m_panel->HandleMouseScroll(event.GetX() / m_Scale, event.GetY() / m_Scale, event.GetWheelRotation());
	event.Skip();
}

void GOGUIPanelWidget::OnKeyCommand(wxKeyEvent& event)
{
	int k = event.GetKeyCode();
	if ( !event.AltDown())
	{
		if (!event.ShiftDown())
			m_panel->HandleKey(259); /* Disable setter */
		if (event.ShiftDown() || event.GetKeyCode() == WXK_SHIFT)
			m_panel->HandleKey(260); /* Enable setter */

		k = WXKtoVK(k);
		if (k)
			m_panel->HandleKey(k);
	}
	event.ResumePropagation(wxEVENT_PROPAGATE_MAX);
	event.Skip();
}

void GOGUIPanelWidget::OnKeyUp(wxKeyEvent& event)
{
	int k = event.GetKeyCode();
	if (k == WXK_SHIFT)
		m_panel->HandleKey(259); /* Disable setter */
	event.ResumePropagation(wxEVENT_PROPAGATE_MAX);
	event.Skip();
}
	
