/*
*	FILE		  : A2.cpp
*   PROJECT		  :	IAD - Assignment 2
*   PROGRAMMER	  : Wes Martin And Ricardo Mohammed
*	FIRST VERSION : 12/3/18
*	DESCRIPTION	  : This file contains the
*/
/*
	Reliability and Flow Control Example
	From "Networking for Game Programmers" - http://www.gaffer.org/networking-for-game-programmers
	Author: Glenn Fiedler <gaffer@gaffer.org>
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>

#include "Net.h"
#include "crc.h"
#include "FlowControl.h"
#include "fileProcessing.h"
#include <ctime>
#include "Constants.h"

//#define SHOW_ACKS

using namespace std;
using namespace net;

int ServerPort = 30001;
int ClientPort = 30001;
const int ProtocolId = 0x11223344;
const float DeltaTime = 1.0f / 30.0f;
const float TimeOut = 10.0f;
const int PacketSize = 1024;
const int crcSize = 4;


int main(int argc, char* argv[])
{
	bool verbose = false;
	// parse command line
	enum Mode
	{
		Client,
		Server
	};

	Mode mode = Server;
	Address address;

	// Prepare command line arguments for client
	if (argc >= 4)
	{
		if (sscanf(argv[kSECOND_ARG], "%d", &ServerPort))
		{
			printf(argv[kSECOND_ARG]);
			int a, b, c, d;
			if (sscanf(argv[kFIRST_ARG], "%d.%d.%d.%d", &a, &b, &c, &d))
			{
				mode = Client;
				address = Address(a, b, c, d, ServerPort);
				ClientPort = ServerPort;
			}
		}

	}

	// Verbose version of argument prep for client
	else if (argc == kTHIRD_ARG)
	{
		sscanf(argv[kFIRST_ARG], "%d", &ServerPort);
		if (strcmp(argv[kSECOND_ARG], "-v") == kSUCCESS)
		{
			verbose = true;
		}
	}
	// Non verbose version of argumnt prep for client
	else if (argc == kSECOND_ARG)
	{
		sscanf(argv[kFIRST_ARG], "%d", &ServerPort);
	}

	// Initialize Sockets
	if (!InitializeSockets())
	{
		printf("failed to initialize sockets\n");
		return kFAILURE;
	}

	ReliableConnection connection(ProtocolId, TimeOut);

	const int port = mode == Server ? ServerPort : ClientPort;

	if (!connection.Start(port))
	{
		printf("could not start connection on port %d\n", port);
		return kFAILURE;
	}

	// Connect as client
	if (mode == Client)
	{
		connection.Connect(address);
	}
	// Listen as server
	else
	{
		connection.Listen();
	}


	bool connected = false;

	FlowControl flowControl;


	// update flow control

	if (connection.IsConnected())
		flowControl.Update(DeltaTime, connection.GetReliabilitySystem().GetRoundTripTime() * 1000.0f);

	const float sendRate = flowControl.GetSendRate();

	// detect changes in connection state

	if (mode == Server && connected && !connection.IsConnected())
	{
		flowControl.Reset();
		printf("reset flow control\n");
		connected = false;
	}

	if (!connected && connection.IsConnected())
	{
		printf("client connected to server\n");
		connected = true;
	}

	if (!connected && connection.ConnectFailed())
	{
		printf("connection failed\n");
	}

	// Zero out the crc values
	int serverCRC = kDEFAULT;
	int clientCRC = kDEFAULT;

	// Command sequence for client
	if (mode == Client)
	{
		printf("client");
		// Initialize necessary variables
		char *fakeFile = NULL;
		int fileSize = kDEFAULT;

		// Setup the receive buffer
		char packet[PacketSize + crcSize + kFIX_OFF_BY_ONE_ERROR];
		memset(packet, 0, PacketSize + crcSize + kFIX_OFF_BY_ONE_ERROR);

		// Setup the crc buffer
		char crc[crcSize + kFIX_OFF_BY_ONE_ERROR];

		// Get initial file string to send to server, and crc of that file
		static char * initiationString = FileProcessor::LoadFile(argv[kTHIRD_ARG], &fakeFile, &fileSize);
		memcpy(packet + crcSize, initiationString, strlen(initiationString));
		snprintf(crc, crcSize + kFIX_OFF_BY_ONE_ERROR, "%04d", 
			crc::crcSlow((unsigned char *)initiationString + crcSize, strlen(initiationString)));
		memcpy(packet, crc, crcSize);

		// Send the initialization packet
		connection.SendPacket((unsigned char *)packet, strlen(packet));

		// More initializations
		int totalSize = fileSize;
		int lastPacketSize = kDEFAULT;
		char *fakeFileptr = fakeFile;

		bool stillReading = true;
		bool isQuitMessage = false;

		// This char array will be used to properly send the last packet.
		char* lastPacket = NULL;
		bool isLastPacket = false;

		// Keep doing this until the end of the file is reached
		while (stillReading)
		{
			// Zero out the packet to send
			memset(packet,kEMPTY, PacketSize + crcSize + kFIX_OFF_BY_ONE_ERROR);

			// If a full packet size was read
			if (fileSize >= PacketSize)
			{
				// Prepare full packet to send out, update file size and pointer
				memcpy(packet + crcSize, fakeFileptr, PacketSize);
				fakeFileptr += PacketSize;
				fileSize -= PacketSize;
			}
			// If some was read, but not a full packet size
			else if (fileSize > kEMPTY)
			{
				// Set last packet flag
				isLastPacket = true;

				// Prepare partial packet to send out, update file size and pointer
				lastPacket = (char*)malloc(fileSize + crcSize + kFIX_OFF_BY_ONE_ERROR);
				memset(lastPacket, kEMPTY, fileSize + crcSize + kFIX_OFF_BY_ONE_ERROR);
				memcpy(lastPacket + crcSize, fakeFileptr, fileSize);
				fakeFileptr += fileSize;
				lastPacketSize = fileSize;
				fileSize = kEMPTY;
			}
			else
			{
				// Prepare quit message to be sent out
				memcpy(packet + crcSize, "quit", strlen("quit"));

				// Flag end of transmission
				fakeFileptr = nullptr;
				stillReading = false;
				isQuitMessage = true;
				isLastPacket = false;
			}

			// If this is not the last packet, calculate the regular CRC value
			if (!isLastPacket)
			{
				snprintf(crc, crcSize + kFIX_OFF_BY_ONE_ERROR, "%04d", 
					crc::crcSlow((unsigned char *)packet + crcSize, sizeof(packet) - crcSize));
				memcpy(packet, crc, crcSize);
			}

			// If this is the last packet, calculate the CRC for this size
			else
			{
				snprintf(crc, crcSize + kFIX_OFF_BY_ONE_ERROR, "%04d",
					crc::crcSlow((unsigned char *)lastPacket + crcSize, lastPacketSize + kFIX_OFF_BY_ONE_ERROR));
				memcpy(lastPacket, crc, crcSize);
			}

			// Print the loading bar/ update it
			//		1000 is the  scale to get a good level of precision
			//		20 is used to not over update the bar
			if ((int)((((totalSize - fileSize) / (float)totalSize)) * 1000) % 20 == kSUCCESS)
			{
				system("CLS");
				float temp = ((float)((totalSize - fileSize) / (float)totalSize)) * kPERCENT_SCALE;
				printf("Transferring: [");
				for (int i = kEMPTY; i < temp / kCUT_IN_HALF; i++)
				{
					printf("%c", 219); // 219 is the 'all pixels filled' character
				}
				for (int i = kEMPTY; i < (kPERCENT_SCALE - (int)ceil(temp)) / kCUT_IN_HALF; i++)
				{
					printf(" ");
				}
				printf("](%2.2f)\n", temp);
			}

			// Send out the the last packet
			if (isLastPacket)
			{
				connection.SendPacket((unsigned char*)lastPacket, lastPacketSize + crcSize + kFIX_OFF_BY_ONE_ERROR);
			}
			// Send out a regular packet
			else
			{
				connection.SendPacket((unsigned char *)packet, sizeof(packet));
			}
		}
	}

	// This is the sequence of commands to execute for the server, it loops forever
	while (mode == Server)
	{
		printf("server");
		// Initializations
		vector<string> results;
		clock_t start = kDEFAULT;	// The clock that will be used to time the transfer.
		double duration = 0.0;		// Will hold the duration of the transfer.
		vector<int> clientCrcs;		// Store client crcs
		vector<int> serverCrcs;		// Store server crcs

		// Prepare the packet to recieve from client
		char packet[PacketSize + crcSize + kFIX_OFF_BY_ONE_ERROR];
		memset(packet, kEMPTY, PacketSize + crcSize + kFIX_OFF_BY_ONE_ERROR);
		int fileSize = kEMPTY;
		char * fileName = NULL;

		// Do this forever
		while (true)
		{
			// Read packet from socket
			connection.ReceivePacket((unsigned char *)packet, sizeof(packet));

			if (strncmp(packet + crcSize, "INITIATION_STRING", strlen("INITIATION_STRING")) == kSUCCESS)
			{
				FileProcessor::BeginTransfer(packet, &fileName, &fileSize);

				// Start the timer here
				start = clock();
				break;
			}
		}
		// Get the output stream to write to a file
		ofstream outputStream(fileName, ofstream::binary);

		// This is done until the quit message is recieved
		while (true)
		{
			// Reset the receiving packet
			memset(packet, kEMPTY, PacketSize + crcSize + kFIX_OFF_BY_ONE_ERROR);
			int bytes_read = connection.ReceivePacket((unsigned char *)packet, sizeof(packet));

			// Restart loop if empty read
			if (bytes_read == kEMPTY)
			{
				continue;
			}
			// Check if the message was a quit
			if (strcmp(packet + crcSize, "quit") == kSUCCESS)
			{
				break;
			}
			// Pull the crc from the client
			sscanf(packet, "%4d", &clientCRC);
			serverCRC = crc::crcSlow((unsigned char *)packet + crcSize, bytes_read - crcSize);

			// Write to the file
			outputStream.write(packet + crcSize, bytes_read - crcSize - kFIX_OFF_BY_ONE_ERROR);

			// Store the CRC values
			clientCrcs.push_back(clientCRC);
			serverCrcs.push_back(serverCRC);
			if (verbose)
			{
				printf("Comparing CRCs: Client: %04d, Server: %04d, Status: %s\n", clientCRC, serverCRC, clientCRC == serverCRC ? "Equal" : "Not Equal");
			}
		}

		// Grab the total elapsed time
		duration = (clock() - start) / (double)CLOCKS_PER_SEC;

		// Print out the time to the command window
		if (crc::checkCrcs(clientCrcs, serverCrcs))
		{
			printf("Name: %20s, Size: %10d, Time: %10.2f, Speed: %10.2f, Success: Succeeded\n", fileName, fileSize, duration, (float)fileSize / (float)duration);
		}
		// Same print, flag as failed
		else
		{
			printf("Name: %20s, Size: %10d, Time: %10.2f, Speed: %10.2f, Success: Failed\n", fileName, fileSize, duration, (float)fileSize / (float)duration);
		}

		// Stop and reset the connection
		connection.Stop();
		InitializeSockets();
		connection.Start(port);
		connection.Listen();
	}

	return kSUCCESS;
}
