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

#include "GOrgueControl.h"
#include "IniFileConfig.h"

GOrgueControl::GOrgueControl() :
	Displayed(false),
	DispKeyLabelOnLeft(false),
	ObjectNumber(0),
	ShortcutKey(0),
	DispLabelFontSize(0),
	DispLabelColour(0,0,0),
	Name()
{

}

GOrgueControl::~GOrgueControl()
{

}

void GOrgueControl::Load(IniFileConfig& cfg, wxString group)
{

	Name = cfg.ReadString(group, wxT("Name"), 64);
	ShortcutKey = cfg.ReadInteger(group, wxT("ShortcutKey"), 0, 255, false);
	Displayed = cfg.ReadBoolean(group, wxT("Displayed"));
	DispKeyLabelOnLeft = cfg.ReadBoolean(group, wxT("DispKeyLabelOnLeft"));
	DispLabelColour = cfg.ReadColor(group, wxT("DispLabelColour"));
	DispLabelFontSize = cfg.ReadFontSize(group, wxT("DispLabelFontSize"));

	if (group.length() >= 3)
		ObjectNumber = atoi(group.mb_str() + group.length() - 3);

}