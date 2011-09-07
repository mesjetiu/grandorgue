/*
 * GrandOrgue - free pipe organ simulator based on MyOrgan
 *
 * MyOrgan 1.0.6 Codebase - Copyright 2006 Milan Digital Audio LLC
 * MyOrgan is a Trademark of Milan Digital Audio LLC
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#include "GOGUIControl.h"
#include "GOGUIPanel.h"
#include "IniFileConfig.h"

GOGUIControl::GOGUIControl(GOGUIPanel* panel,void* control) :
	m_panel(panel),
	m_group(wxT("---")),
	m_control(control),
	m_BoundingRect(0, 0, 0, 0)
{
	m_metrics = panel->GetDisplayMetrics();
}

GOGUIControl::~GOGUIControl()
{
}

void GOGUIControl::Load(IniFileConfig& cfg, wxString group)
{
	m_group = group;
}

void GOGUIControl::Save(IniFileConfig& cfg, bool prefix)
{
}

void GOGUIControl::ControlChanged(void* control)
{
	if (control == m_control)
		m_panel->AddEvent(this);
}

void GOGUIControl::Draw(wxDC* dc)
{
}

void GOGUIControl::HandleKey(int key)
{
}

void GOGUIControl::HandleMousePress(int x, int y, bool right)
{
}

void GOGUIControl::HandleMouseScroll(int x, int y, int amount)
{
}

const wxRect& GOGUIControl::GetBoundingRect()
{
	return m_BoundingRect;
}