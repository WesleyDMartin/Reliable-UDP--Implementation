/*
*	FILE		  : crc.cpp
*   PROJECT		  :	IAD - Assignment 2
*   PROGRAMMER	  : Wes Martin And Ricardo Mohammed
*	FIRST VERSION : 12/3/18
*	DESCRIPTION	  : This file contains the
*/

#include "crc.h"
//https://barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code

/*
* The width of the CRC calculation and result.
* Modify the typedef for a 16 or 32-bit CRC standard.
*/

#define WIDTH  (8 * sizeof(unsigned char))
#define TOPBIT (1 << (WIDTH - 1))
#define POLYNOMIAL 0xD8  /* 11011 followed by 0's */
bool crc::initRun = false;
unsigned char crc::crcTable[256] = {0};

int crc::crcFast(unsigned char message[], int nBytes)
{
	if (!initRun)
	{
		crcInit();
	}

	int data;
	int remainder = 0;


	/*
	* Divide the message by the polynomial, a byte at a time.
	*/
	for (int byte = 0; byte < nBytes; ++byte)
	{
		data = message[byte] ^ (remainder >> (WIDTH - 8));
		remainder = crcTable[data] ^ (remainder << 8);
	}

	/*
	* The final remainder is the CRC.
	*/
	return (remainder);
} /* crcFast() */


void crc::crcInit(void)
{
	int remainder;

	/*
	* Compute the remainder of each possible dividend.
	*/
	for (int dividend = 0; dividend < 256; ++dividend)
	{
		/*
		* Start with the dividend followed by zeros.
		*/
		remainder = dividend << (WIDTH - 8);

		/*
		* Perform modulo-2 division, a bit at a time.
		*/
		for (int bit = 8; bit > 0; --bit)
		{
			/*
			* Try to divide the current data bit.
			*/
			if (remainder & TOPBIT)
			{
				remainder = (remainder << 1) ^ POLYNOMIAL;
			}
			else
			{
				remainder = (remainder << 1);
			}
		}

		/*
		* Store the result into the table.
		*/
		crcTable[dividend] = remainder;
	}
	initRun = true;
} /* crcInit() */


unsigned char crc::crcSlow(unsigned char const message[], int nBytes)
{
	unsigned char remainder = 0;


	/*
	* Perform modulo-2 division, a byte at a time.
	*/
	for (int byte = 0; byte < nBytes; ++byte)
	{
		/*
		* Bring the next byte into the remainder.
		*/
		remainder ^= (message[byte] << (WIDTH - 8));

		/*
		* Perform modulo-2 division, a bit at a time.
		*/
		for (unsigned char bit = 8; bit > 0; --bit)
		{
			/*
			* Try to divide the current data bit.
			*/
			if (remainder & TOPBIT)
			{
				remainder = (remainder << 1) ^ POLYNOMIAL;
			}
			else
			{
				remainder = (remainder << 1);
			}
		}
	}

	/*
	* The final remainder is the CRC result.
	*/
	return (remainder);
} /* crcSlow() */
bool crc::checkCrcs(std::vector<int> clientCrcs, std::vector<int> serverCrcs)
{
	bool ret = true;
	if(clientCrcs.size() != serverCrcs.size())
	{
		ret = false;
	}
	else
	{
		for(int i = 0; i < (int)clientCrcs.size(); i++)
		{
			if(clientCrcs[i] != serverCrcs[i])
			{
				ret = false;
				break;
			}
		}
	}
	return ret;
}
