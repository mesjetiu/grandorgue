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

#ifndef GORGUEDRAWSTOP_H
#define GORGUEDRAWSTOP_H

#include <wx/wx.h>
#include "GOrgueControl.h"
#include "GOrgueDrawable.h"
#include "IniFileConfig.h"

class GOrgueDisplayMetrics;

class GOrgueDrawstop : public GOrgueControl, GOrgueDrawable
{

public:
	bool DefaultToEngaged;
	bool DisplayInInvertedState;
	int DispDrawstopRow;
	int DispDrawstopCol;
	int DispImageNum;
	int StopControlMIDIKeyNumber;
	GOrgueDisplayMetrics* DisplayMetrics;

	GOrgueDrawstop();
	virtual ~GOrgueDrawstop();
	void Load(IniFileConfig& cfg, const char* group, GOrgueDisplayMetrics* displayMetrics);
	void Save(IniFileConfig& cfg, bool prefix, wxString group);
	virtual bool Draw(int xx, int yy, wxDC* dc = 0, wxDC* dc2 = 0);
	void Push(void);
	void MIDI(void);
	virtual bool Set(bool on);

};

#endif