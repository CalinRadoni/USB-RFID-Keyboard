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

#ifndef DefinesH
#define DefinesH

#include <Arduino.h>

#define VER_HI 1
#define VER_LO 6

/* Packet definition:
	PacketHeader byte SS_HeaderID_0
				 byte SS_HeaderID_1
				 byte SS_HeaderID_2
	Command		 byte
	Data         byte
*/

#define SS_HeaderLen    3
#define SS_HeaderID_0  67	// C
#define SS_HeaderID_1 109	// m
#define SS_HeaderID_2 100	// d

#define SS_Cmd_Ver         118	// v
#define SS_Cmd_GetCardInfo 105	// i
#define SS_Cmd_WriteCard   119	// w

#define SS_CmdFullColorA    99	// c
#define SS_CmdFullColor     67	// C
#define SS_CmdSetPixelA    112	// p
#define SS_CmdSetPixel      80	// P
#define SS_CmdSetFrame      70	// F

/* Escape sequences for nonprintable chars:
	## the # char
	#t KEY_TAB
	#n KEY_ENTER
	#e KEY_ESC
	#U KEY_UP
	#D KET_DOWN
	#L KEY_LEFT
	#R KEY_RIGHT
	#x Win+r
	#0 delay 50 ms
	#1 delay 100 ms
	...
	#9 delay 900 ms
*/

#endif
