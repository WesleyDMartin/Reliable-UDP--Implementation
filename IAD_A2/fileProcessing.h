/*
*	FILE		  : fileProcessing.h
*   PROJECT		  :	IAD - Assignment 2
*   PROGRAMMER	  : Wes Martin And Ricardo Mohammed
*	FIRST VERSION : 12/3/18
*	DESCRIPTION	  : This file contains the
*/

#pragma once


#include <iostream>
#include <fstream>
#include <string>
#include <vector>


using namespace std;


// Class	   : FileProcessor
// Description : Contains methods to load files into its unsigned bytes.
class FileProcessor
{
private:
	static vector<string> split(const char * str, char c);


public:
	static char* LoadFile(char * filePath, char ** fileData, int *fileSize);
	static void BeginTransfer(char * message, char ** filePath, int *fileSize);

};

