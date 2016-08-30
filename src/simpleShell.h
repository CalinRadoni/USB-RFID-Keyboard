/* Copyright (C) 2016 Calin Radoni

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef simpleShellH
#define simpleShellH

#include "Arduino.h"
#include "defines.h"
#include "simpleTextOutput.h"

#define SS_BUFFER_LEN 250

class SimpleShell
{
private:
	byte buffer[SS_BUFFER_LEN];
	uint16_t bufLen;

	void CleanupBuffer(void);
	void ShiftAndCleanBuffer(uint16_t);
	bool CheckHeader(void);

	void CmdVersion(void);
	byte Hex2Bin(byte, byte);
	uint32_t HexColor2Bin(byte*);
	uint16_t Dec2Bin(byte*, byte);
	void CmdSetColorA(void);
	void CmdSetPixelColorA(void);

public:
	SimpleShell(void);

	void AddData(byte);
	void ProcessData(void);
};

extern SimpleShell simpleShell;

#endif
