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

#ifndef SimpleTextOutputH
#define SimpleTextOutputH

#include "Arduino.h"

class SimpleTextOutput{
protected:
public:
    SimpleTextOutput(void);

	void Flush(void);

	/* Format options:
	 * - c    char
	 * - d,i  integer
	 * - s    string, NULL terminated
	 * - u    unsigned integer
	 * - x,X  unsigned integer - hexadecimal
	 * */
	void FormatAndOutputString(const char *fmt, ...);
	void OutputBuffer(uint8_t*, uint8_t);
};

extern SimpleTextOutput simpleTextOutput;

#endif
