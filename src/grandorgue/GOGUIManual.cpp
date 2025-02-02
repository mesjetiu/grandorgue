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

#include "GOGUIManual.h"

#include "GOGUIDisplayMetrics.h"
#include "GOGUILayoutEngine.h"
#include "GOGUIMouseState.h"
#include "GOGUIPanel.h"
#include "GOrgueConfigReader.h"
#include "GOrgueDC.h"
#include "GOrgueManual.h"
#include <wx/intl.h>
#include <wx/log.h>

GOGUIManual::GOGUIManual(GOGUIPanel* panel, GOrgueManual* manual, unsigned manual_number):
	GOGUIControl(panel, manual),
	m_manual(manual),
	m_ManualNumber(manual_number),
	m_Keys()
{
	m_layout->RegisterManual(this);
}

bool GOGUIManual::IsSharp(unsigned key)
{
	return m_Keys[key].IsSharp;
}

unsigned GOGUIManual::GetKeyCount()
{
	return m_Keys.size();
}

void GOGUIManual::Init(GOrgueConfigReader& cfg, wxString group)
{
	const wxChar* keyNames[12] = { wxT("C"), wxT("Cis"), wxT("D"), wxT("Dis"), wxT("E"), wxT("F"), wxT("Fis"), wxT("G"),
				       wxT("Gis"), wxT("A"), wxT("Ais"), wxT("B") };

	GOGUIControl::Init(cfg, group);
	wxString type = m_ManualNumber ? wxT("Manual") : wxT("Pedal");
	unsigned first_midi_note = m_manual->GetFirstAccessibleKeyMIDINoteNumber();

	unsigned x = 0, y = 0;
	int width = 0;
	int height = 1;

	m_Keys.resize(m_manual->GetNumberOfAccessibleKeys());
	for(unsigned i = 0; i < m_Keys.size(); i++)
	{
		unsigned key_nb = i + first_midi_note;
		m_Keys[i].MidiNumber = key_nb;
		m_Keys[i].IsSharp = (((key_nb % 12) < 5 && !(key_nb & 1)) || ((key_nb % 12) >= 5 && (key_nb & 1))) ? false : true;

		wxString off_mask_file, on_mask_file;
		wxString on_file, off_file;
		wxString bmp_type;
		unsigned key_width, key_yoffset;
		int key_offset;
		wxString base = keyNames[key_nb % 12];
		if (!i)
			base = wxT("First") + base;
		else if (i + 1 == m_Keys.size())
			base = wxT("Last") + base;

		if (!m_ManualNumber)
		{
			bmp_type = m_Keys[i].IsSharp ? wxT("Sharp") : wxT("Natural");
		}
		else
		{
			if (m_Keys[i].IsSharp)
				bmp_type = wxT("Sharp");
			else if (i == 0)
			{
				switch(key_nb %12)
				{
				case 4:
				case 11:
					bmp_type = wxT("Natural");
					break;
				default:
					bmp_type = wxT("C");
				}
			}
			else if (i + 1 == m_Keys.size())
			{
				switch(key_nb %12)
				{
				case 0:
				case 5:
					bmp_type = wxT("Natural");
					break;
				default:
					bmp_type = wxT("E");
				}
			}
			else 
			{
				switch(key_nb %12)
				{
				case 0:
				case 5:
					bmp_type = wxT("C");
					break;
				case 4:
				case 11:
					bmp_type = wxT("E");
					break;
				default:
					bmp_type = wxT("D");
				}
			}
		}
		off_file = wxT("GO:") + type + wxT("Off_") + bmp_type;
		on_file = wxT("GO:") + type + wxT("On_") + bmp_type;

		on_mask_file = wxEmptyString;
		off_mask_file = on_mask_file;

		m_Keys[i].OnBitmap = m_panel->LoadBitmap(on_file, on_mask_file);
		m_Keys[i].OffBitmap = m_panel->LoadBitmap(off_file, off_mask_file);

		if (m_Keys[i].OnBitmap.GetWidth() != m_Keys[i].OffBitmap.GetWidth() ||
		    m_Keys[i].OnBitmap.GetHeight() != m_Keys[i].OffBitmap.GetHeight())
			throw wxString::Format(_("bitmap size does not match for '%s'"), group.c_str());

		key_width = m_Keys[i].OnBitmap.GetWidth();
		key_offset = 0;
		key_yoffset = 0;
		if (m_Keys[i].IsSharp && m_ManualNumber)
		{
			key_width = 0;
			key_offset = - ((int)m_Keys[i].OnBitmap.GetWidth()) / 2;
		} else if (!m_ManualNumber && ((key_nb % 12) == 4 || (key_nb % 12) == 11))
		{
			key_width *= 2;
		}

		m_Keys[i].Rect = wxRect(x + key_offset, y + key_yoffset, m_Keys[i].OnBitmap.GetWidth(), m_Keys[i].OnBitmap.GetHeight());
		if (x + key_offset < 0)
			wxLogWarning(_("Manual key %d outside of the bounding box"), m_Keys[i].MidiNumber);

		unsigned mouse_x = 0;
		unsigned mouse_y = 0;
		unsigned mouse_w = m_Keys[i].Rect.GetWidth() - mouse_x;
		unsigned mouse_h = m_Keys[i].Rect.GetHeight() - mouse_y;
		m_Keys[i].MouseRect = wxRect(m_Keys[i].Rect.GetX() + mouse_x, m_Keys[i].Rect.GetY() + mouse_y, mouse_w, mouse_h);

		if (height < m_Keys[i].Rect.GetBottom())
			height = m_Keys[i].Rect.GetBottom();
		if (width < m_Keys[i].Rect.GetRight())
			width = m_Keys[i].Rect.GetRight();
		x += key_width;
	}

	m_BoundingRect = wxRect(-1, -1, width, height);
}

void GOGUIManual::Load(GOrgueConfigReader& cfg, wxString group)
{
	const wxChar* keyNames[12] = { wxT("C"), wxT("Cis"), wxT("D"), wxT("Dis"), wxT("E"), wxT("F"), wxT("Fis"), wxT("G"),
				       wxT("Gis"), wxT("A"), wxT("Ais"), wxT("B") };

	GOGUIControl::Load(cfg, group);
	bool color_inverted = cfg.ReadBoolean(ODFSetting, group, wxT("DispKeyColourInverted"), false, false);
	bool color_wooden = cfg.ReadBoolean(ODFSetting, group, wxT("DispKeyColourWooden"), false, false);
	wxString type = m_ManualNumber ? wxT("Manual") : wxT("Pedal");
	if (color_inverted)
		type += wxT("Inverted");
	if (color_wooden && m_ManualNumber)
		type += wxT("Wood");
	unsigned first_midi_note = cfg.ReadInteger(ODFSetting, group, wxT("DisplayFirstNote"), 0, 127, false, m_manual->GetFirstAccessibleKeyMIDINoteNumber());

	unsigned x = 0, y = 0;
	int width = 0;
	int height = 1;

	m_Keys.resize(cfg.ReadInteger(ODFSetting, group, wxT("DisplayKeys"), 1, m_manual->GetNumberOfAccessibleKeys(), false, m_manual->GetNumberOfAccessibleKeys()));
	for(unsigned i = 0; i < m_Keys.size(); i++)
	{
		unsigned key_nb = i + first_midi_note;
		m_Keys[i].MidiNumber = cfg.ReadInteger(ODFSetting, group, wxString::Format(wxT("DisplayKey%03d"), i + 1), 0, 127, false, key_nb);
		m_Keys[i].IsSharp = (((key_nb % 12) < 5 && !(key_nb & 1)) || ((key_nb % 12) >= 5 && (key_nb & 1))) ? false : true;

		wxString off_mask_file, on_mask_file;
		wxString on_file, off_file;
		wxString bmp_type;
		unsigned key_width, key_yoffset;
		int key_offset;
		wxString base = keyNames[key_nb % 12];
		if (!i)
			base = wxT("First") + base;
		else if (i + 1 == m_Keys.size())
			base = wxT("Last") + base;

		if (!m_ManualNumber)
		{
			bmp_type = m_Keys[i].IsSharp ? wxT("Sharp") : wxT("Natural");
		}
		else
		{
			if (m_Keys[i].IsSharp)
				bmp_type = wxT("Sharp");
			else if (i == 0)
			{
				switch(key_nb %12)
				{
				case 4:
				case 11:
					bmp_type = wxT("Natural");
					break;
				default:
					bmp_type = wxT("C");
				}
			}
			else if (i + 1 == m_Keys.size())
			{
				switch(key_nb %12)
				{
				case 0:
				case 5:
					bmp_type = wxT("Natural");
					break;
				default:
					bmp_type = wxT("E");
				}
			}
			else 
			{
				switch(key_nb %12)
				{
				case 0:
				case 5:
					bmp_type = wxT("C");
					break;
				case 4:
				case 11:
					bmp_type = wxT("E");
					break;
				default:
					bmp_type = wxT("D");
				}
			}
		}
		off_file = wxT("GO:") + type + wxT("Off_") + bmp_type;
		on_file = wxT("GO:") + type + wxT("On_") + bmp_type;

		on_file = cfg.ReadStringTrim(ODFSetting, group, wxT("ImageOn_") + base, false, on_file);
		off_file = cfg.ReadStringTrim(ODFSetting, group, wxT("ImageOff_") + base, false, off_file);
		on_mask_file = cfg.ReadStringTrim(ODFSetting, group, wxT("MaskOn_") + base, false, wxEmptyString);
		off_mask_file = cfg.ReadStringTrim(ODFSetting, group, wxT("MaskOff_") + base, false, on_mask_file);

		on_file = cfg.ReadStringTrim(ODFSetting, group, wxString::Format(wxT("Key%03dImageOn"), i + 1), false, on_file);
		off_file = cfg.ReadStringTrim(ODFSetting, group, wxString::Format(wxT("Key%03dImageOff"), i + 1), false, off_file);
		on_mask_file = cfg.ReadStringTrim(ODFSetting, group, wxString::Format(wxT("Key%03dMaskOn"), i + 1), false, on_mask_file);
		off_mask_file = cfg.ReadStringTrim(ODFSetting, group, wxString::Format(wxT("Key%03dMaskOff"), i + 1), false, off_mask_file);

		m_Keys[i].OnBitmap = m_panel->LoadBitmap(on_file, on_mask_file);
		m_Keys[i].OffBitmap = m_panel->LoadBitmap(off_file, off_mask_file);

		if (m_Keys[i].OnBitmap.GetWidth() != m_Keys[i].OffBitmap.GetWidth() ||
		    m_Keys[i].OnBitmap.GetHeight() != m_Keys[i].OffBitmap.GetHeight())
			throw wxString::Format(_("bitmap size does not match for '%s'"), group.c_str());

		key_width = m_Keys[i].OnBitmap.GetWidth();
		key_offset = 0;
		key_yoffset = 0;
		if (m_Keys[i].IsSharp && m_ManualNumber)
		{
			key_width = 0;
			key_offset = - ((int)m_Keys[i].OnBitmap.GetWidth()) / 2;
		} else if (!m_ManualNumber && ((key_nb % 12) == 4 || (key_nb % 12) == 11))
		{
			key_width *= 2;
		}

		key_width = cfg.ReadInteger(ODFSetting, group, wxT("Width_") + base, 0, 500, false, key_width);
		key_offset = cfg.ReadInteger(ODFSetting, group, wxT("Offset_") + base, -500, 500, false, key_offset);
		key_yoffset = cfg.ReadInteger(ODFSetting, group, wxT("YOffset_") + base, 0, 500, false, key_yoffset);

		key_width = cfg.ReadInteger(ODFSetting, group, wxString::Format(wxT("Key%03dWidth"), i + 1), 0, 500, false, key_width);
		key_offset = cfg.ReadInteger(ODFSetting, group, wxString::Format(wxT("Key%03dOffset"), i + 1), -500, 500, false, key_offset);
		key_yoffset = cfg.ReadInteger(ODFSetting, group, wxString::Format(wxT("Key%03dYOffset"), i + 1), 0, 500, false, key_yoffset);

		m_Keys[i].Rect = wxRect(x + key_offset, y + key_yoffset, m_Keys[i].OnBitmap.GetWidth(), m_Keys[i].OnBitmap.GetHeight());
		if (x + key_offset < 0)
			wxLogWarning(_("Manual key %d outside of the bounding box"), m_Keys[i].MidiNumber);

		unsigned mouse_x = cfg.ReadInteger(ODFSetting, group, wxString::Format(wxT("Key%03dMouseRectLeft"), i + 1), 0, m_Keys[i].Rect.GetWidth() - 1, false, 0);
		unsigned mouse_y = cfg.ReadInteger(ODFSetting, group, wxString::Format(wxT("Key%03dMouseRectTop"), i + 1),  0, m_Keys[i].Rect.GetHeight() - 1, false, 0);
		unsigned mouse_w = cfg.ReadInteger(ODFSetting, group, wxString::Format(wxT("Key%03dMouseRectWidth"), i + 1),  1, m_Keys[i].Rect.GetWidth() - mouse_x, false, m_Keys[i].Rect.GetWidth() - mouse_x);
		unsigned mouse_h = cfg.ReadInteger(ODFSetting, group, wxString::Format(wxT("Key%03dMouseRectHeight"), i + 1),  1, m_Keys[i].Rect.GetHeight() - mouse_y, false, m_Keys[i].Rect.GetHeight() - mouse_y);
		m_Keys[i].MouseRect = wxRect(m_Keys[i].Rect.GetX() + mouse_x, m_Keys[i].Rect.GetY() + mouse_y, mouse_w, mouse_h);

		if (height < m_Keys[i].Rect.GetBottom())
			height = m_Keys[i].Rect.GetBottom();
		if (width < m_Keys[i].Rect.GetRight())
			width = m_Keys[i].Rect.GetRight();
		x += key_width;
	}

	x = cfg.ReadInteger(ODFSetting, group, wxT("PositionX"), 0, m_metrics->GetScreenWidth(), false, -1);
	y = cfg.ReadInteger(ODFSetting, group, wxT("PositionY"), 0, m_metrics->GetScreenHeight(), false, -1);

	m_BoundingRect = wxRect(x, y, width, height);
}

void GOGUIManual::Layout()
{
	const GOGUILayoutEngine::MANUAL_RENDER_INFO &mri = m_layout->GetManualRenderInfo(m_ManualNumber);
	if (m_BoundingRect.GetX() == -1)
		m_BoundingRect.SetX(mri.x + 1);
	if (m_BoundingRect.GetY() == -1)
		m_BoundingRect.SetY(mri.keys_y);

	for(unsigned i = 0; i < m_Keys.size(); i++)
	{
		m_Keys[i].Rect.Offset(m_BoundingRect.GetX(), m_BoundingRect.GetY());
		m_Keys[i].MouseRect.Offset(m_BoundingRect.GetX(), m_BoundingRect.GetY());
	}
}

void GOGUIManual::PrepareDraw(double scale, GOrgueBitmap* background)
{
	for (unsigned i = 0; i < m_Keys.size(); i++)
	{
		m_Keys[i].OnBitmap.PrepareBitmap(scale, m_Keys[i].Rect, background);
		m_Keys[i].OffBitmap.PrepareBitmap(scale, m_Keys[i].Rect, background);
	}
}

void GOGUIManual::Draw(GOrgueDC& dc)
{
	for (unsigned i = 0; i < m_Keys.size(); i++)
	{
		GOrgueBitmap& bitmap = m_manual->IsKeyDown(m_Keys[i].MidiNumber) ? m_Keys[i].OnBitmap : m_Keys[i].OffBitmap;
		dc.DrawBitmap(bitmap, m_Keys[i].Rect);
	}
	GOGUIControl::Draw(dc);
}

bool GOGUIManual::HandleMousePress(int x, int y, bool right, GOGUIMouseState& state)
{
	if (!m_BoundingRect.Contains(x, y))
		return false;

	if (right)
	{
		m_manual->ShowConfigDialog();
		return true;
	}
	else
	{
		for(unsigned i = 0; i < m_Keys.size(); i++)
		{
			if (m_Keys[i].MouseRect.Contains(x, y))
			{
				if (i + 1 < m_Keys.size() && m_Keys[i + 1].IsSharp && m_Keys[i + 1].MouseRect.Contains(x, y))
					continue;
				if (state.GetControl() == this && state.GetIndex() == i)
					return true;
				state.SetControl(this);
				state.SetIndex(i);

				m_manual->Set(m_Keys[i].MidiNumber, m_manual->IsKeyDown(m_Keys[i].MidiNumber) ? 0 : 0x7f);
				return true;
			}
		}
		return false;
	}
}
