/*
*	FILE		  : fileProcessing.cpp
*   PROJECT		  :	IAD - Assignment 2
*   PROGRAMMER	  : Wes Martin And Ricardo Mohammed
*	FIRST VERSION : 12/3/18
*	DESCRIPTION	  : This file contains the
*/



#include "fileProcessing.h"
#include "Constants.h"
#include <vector>
#include <string>
using namespace std;

/**
 * \brief 
 * \param filePath The file to be read in
 * \param fileData The char array that will contain the file
 * \param fileSize The size of the file
 * \return The string that contains data relevant to the file to be sent to the server
 */
char* FileProcessor::LoadFile(char * filePath, char ** fileData, int *fileSize)
{
	ifstream inputStream(filePath, ifstream::binary);	// Attempts to construct the filestream from the
														// the file path given as a binary file.
	string path(filePath);
	string fileName = path.substr(path.find_last_of("\\") + kFIX_OFF_BY_ONE_ERROR);

	if (inputStream)
	{
		inputStream.seekg(0, inputStream.end);
		*fileSize = (unsigned int)inputStream.tellg();
		inputStream.seekg(0, inputStream.beg);
	}

	// Initializations
	string size = to_string(*fileSize);
	*fileData = new char[*fileSize];

	// Read in the entire file
	inputStream.read(*fileData, *fileSize);

	// Display error messages
	if (!inputStream)
	{
		cout << "Failed to read the file.\n";
	}
	else
	{
		cout << "Success reading the file.\n";
	}

	// Close the stream
	inputStream.close();

	// Set up the return string
	static string ret = "INITIATION_STRING|" + fileName + "|" + size;
	static char * cstyleRet = (char *)ret.c_str();
	return cstyleRet;
}

/**
 * \brief This function parses out the initialization string that was sent by the client
 * \param c_message The c style message
 * \param filePath The buffer to store the file path
 * \param fileSize Pointer to the file size
 */
void FileProcessor::BeginTransfer(char * c_message, char ** filePath, int *fileSize)
{
	string message(c_message);
	vector<string> strings = split(c_message, '|');
	*filePath = (char *)malloc(strings[kFILE_NAME].size() + kFIX_OFF_BY_ONE_ERROR);
	memset(*filePath, kEMPTY, strings[kFILE_NAME].size() + kFIX_OFF_BY_ONE_ERROR);
	memcpy(*filePath, strings[kFILE_NAME].c_str(), strings[kFILE_NAME].size());
	string s_size = message.substr(message.find_last_of("|") + kFIX_OFF_BY_ONE_ERROR);
	*fileSize = stoi(strings[kFILE_SIZE]);
}


/**
 * \brief Split up a string on a delimeter
 * \param str The string to be split up
 * \param c The deliminator
 * \return The deliminated string
 */
vector<string> FileProcessor::split(const char *str, char c = ' ')
{
	vector<string> result;

	do
	{
		const char *begin = str;

		while (*str != c && *str)
			str++;

		result.push_back(string(begin, str));
	} while (kEMPTY != *str++);

	return result;
}