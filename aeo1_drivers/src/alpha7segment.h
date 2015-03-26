/*
 * alpha7segment.cpp
 *
 *  Created on: 26. mars 2015
 *      Author: Anders Emil Olsen
 */
//--------------------------------
namespace aeo1 {
//--------------------------------
enum {
	alpha7segment_space = 0x00,
	alpha7segment_dot = 0x80,
	alpha7segment_mark = 0x53
};
//--------------------------------
uint8_t Alpha7Segment(char cSymbol);
//--------------------------------
}
//--------------------------------
