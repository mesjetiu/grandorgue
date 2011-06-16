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

#ifndef GORGUEMANUAL_H
#define GORGUEMANUAL_H

#include <wx/wx.h>
#include <vector>

class GOrgueCoupler;
class GOrgueDisplayMetrics;
class GOrgueDivisional;
class GOrgueStop;
class GOrgueTremulant;
class IniFileConfig;

class GOrgueManual 
{

private:

	int m_ManualNumber;
	wxInt16 m_MIDI[85];

	int NumberOfLogicalKeys;
	int FirstAccessibleKeyLogicalKeyNumber;
	int FirstAccessibleKeyMIDINoteNumber;
	int NumberOfAccessibleKeys;
	int MIDIInputNumber;

	unsigned NumberOfStops;
	unsigned NumberOfCouplers;
	unsigned NumberOfDivisionals;
	unsigned NumberOfTremulants;
	wxInt16 tremulant[10];

	wxString Name;

	std::vector<GOrgueStop*> stop;
	GOrgueCoupler* coupler;
	GOrgueDivisional* divisional;


public:

	bool Displayed : 1;
	int m_Width, m_Height, m_X, m_Y, m_KeysY, m_PistonY;
	bool DispKeyColourInverted : 1;
	bool DispKeyColourWooden : 1;

	GOrgueManual();
	void Load(IniFileConfig& cfg, const char* group, GOrgueDisplayMetrics* displayMetrics, int manualNumber);
	void Set(int note, bool on, bool pretend = false, int depth = 0, GOrgueCoupler* prev = 0);
	void MIDI(void);
	~GOrgueManual(void);

	int GetNumberOfAccessibleKeys();
	int GetFirstAccessibleKeyMIDINoteNumber();
	int GetMIDIInputNumber();
	int GetLogicalKeyCount();
	void AllNotesOff();
	void MIDIPretend(bool on);
	bool IsKeyDown(int midiNoteNumber);

	int GetStopCount();
	GOrgueStop* GetStop(unsigned index);
	int GetCouplerCount();
	GOrgueCoupler* GetCoupler(unsigned index);
	int GetDivisionalCount();
	GOrgueDivisional* GetDivisional(unsigned index);
	int GetTremulantCount();
	GOrgueTremulant* GetTremulant(unsigned index);

};

#endif