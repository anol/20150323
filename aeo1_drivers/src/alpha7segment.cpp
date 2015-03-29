/*
 * alpha7segment.cpp
 *
 *  Created on: 26. mars 2015
 *      Author: Anders Emil Olsen
 */
//--------------------------------
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
//--------------------------------
#include "alpha7segment.h"
//--------------------------------
namespace aeo1 {
//--------------------------------
struct alpha7segment_entry {
	char cSymbol;
	uint8_t nCode;
};
//--------------------------------
const alpha7segment_entry alpha7segment_table[] = { { '0', 0x3F },
		{ '1', 0x06 }, { '2', 0x5B }, { '3', 0x4F }, { '4', 0x66 },
		{ '5', 0x6D }, { '6', 0x7D }, { '7', 0x07 }, { '8', 0x7F },
		{ '9', 0x6F }, { 'A', 0x77 }, { 'B', 0x7C }, { 'C', 0x39 },
		{ 'D', 0x5E }, { 'E', 0x79 }, { 'F', 0x71 }, { 'G', 0x3D },
		{ 'H', 0x76 }, { 'I', 0x06 }, { 'J', 0x0E }, { 'K', 0x70 },
		{ 'L', 0x38 }, { 'M', 0x55 }, { 'N', 0x37 }, { 'O', 0x3F },
		{ 'P', 0x73 }, { 'Q', 0x67 }, { 'R', 0x50 }, { 'S', 0x6D },
		{ 'T', 0x78 }, { 'U', 0x3E }, { 'V', 0x1C }, { 'W', 0x1D },
		{ 'X', 0x49 }, { 'Y', 0x72 }, { 'Z', 0x52 }, { ' ', 0x00 },
		{ '.', 0x80 }, { '/', 0x01 }, { '-', 0x40 }, { '_', 0x08 },
		{ '=', 0x48 }, { '|', 0x30 }, { '?', 0x53 }, { 0, 0x01 } };
//--------------------------------
uint8_t Alpha7Segment(char cSymbol) {
	bool bSearch = true;
	uint8_t nCode = alpha7segment_mark;
	for (int nEntry = 0; bSearch && alpha7segment_table[nEntry].cSymbol;
			nEntry++) {
		if (alpha7segment_table[nEntry].cSymbol == cSymbol) {
			nCode = alpha7segment_table[nEntry].nCode;
			bSearch = false;
		}
	}
	return nCode;
}
//--------------------------------
}
//--------------------------------

