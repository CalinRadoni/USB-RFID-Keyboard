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

#include "simpleTextOutput.h"

// -----------------------------------------------------------------------------

SimpleTextOutput simpleTextOutput;

// -----------------------------------------------------------------------------

SimpleTextOutput::SimpleTextOutput(void)
{
    //
}

// -----------------------------------------------------------------------------

void SimpleTextOutput::Flush(void)
{
    Serial.flush();
}

// -----------------------------------------------------------------------------

void SimpleTextOutput::FormatAndOutputString(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	while(*fmt) {
		if (*fmt == '%') {
			switch (*(++fmt)) {
			case 'c':
				Serial.write((uint8_t)va_arg(va, int));
				break;
			case 'd':
			case 'i':
			{
				signed int val = va_arg(va, signed int);
                Serial.print(val, 10);
			}
			break;
			case 's':
			{
				char * arg = va_arg(va, char *);
				while (*arg) {
					Serial.write(*arg++);
				}
			}
			break;
			case 'u':
				Serial.print((unsigned int)(va_arg(va, unsigned int)), 10);
				break;
			case 'x':
			case 'X':
				Serial.print((unsigned int)(va_arg(va, int)), 16);
				break;
			case '%':
                Serial.write('%');
				break;
			}
			fmt++;
		}
		else {
			Serial.write(*fmt++);
		}
	}

	va_end(va);
}

void SimpleTextOutput::OutputBuffer(uint8_t* buffIn, uint8_t lenIn)
{
	for(uint8_t i = 0; i < lenIn; i++)
        Serial.write(buffIn[i]);
}

// -----------------------------------------------------------------------------
