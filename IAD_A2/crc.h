/*
*	FILE		  : crc.h
*   PROJECT		  :	IAD - Assignment 2
*   PROGRAMMER	  : Wes Martin And Ricardo Mohammed
*	FIRST VERSION : 12/3/18
*	DESCRIPTION	  : This file contains the
*/

#pragma once
#include <vector>


static class crc
{
	static unsigned char crcTable[256];
	static bool initRun;

public:

	static int crcFast(unsigned char message[], int nBytes);

	static void crcInit(void);

	static unsigned char crcSlow(unsigned char const message[], int nBytes);

	static bool checkCrcs(std::vector<int> clientCrcs, std::vector<int> serverCrcs);
};
