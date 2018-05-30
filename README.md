# Reliable UDP Implementation

## Summary
We wanted to implement a file transfer program based off a reiable implementation of UDP. The goal was to orive that this version of UDP had a zero or minimal packet loss. The client accepts the connection information to connect to the server, as well as a path to a file, and then the file is transferred over UDP, while the server performs a CR validity check on the information. Just a note, there is no retransmission. The server just runs the check and flags the transfer if something went wrong.

## Usage
The single application can be run as either the server or the client.

### Server
To run it as a server, simply run the application like this:

        IAD_A2.exe PORT_NUMBER [-v] 
   
        PORT_NUMBER : Must be the same as client
        -v          : Verbose, display more information about transferring files
 
 ### Server
To run it as a server, simply run the application like this:


        IAD_A2.exe PORT_NUMBER SERVER_ADDRESS FILE_PATH 
   
        PORT_NUMBER     : Must be the same as server
        SERVER_ADDRESS  : Ip address of server
        FILE_PATH       : Path to file to be transferred
