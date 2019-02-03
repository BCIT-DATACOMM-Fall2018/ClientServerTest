/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: socket.c - A socket function library for using TCP and UDP sockets.
--
--
-- PROGRAM: libsocket
--
-- UDP and TCP FUNCTIONS:
-- struct socketStruct * createSocket()
-- int bindPort(struct socketStruct* socketPointer, uint16_t port)
-- int closeSocket(struct socketStruct * socket)
-- void freeSocket(struct socketStruct * socket)
--
-- UDP FUNCTIONS:
-- int initSocket(struct socketStruct* socketPointer)
-- int sendData(struct socketStruct* socket, struct destination * dest, const char* data, size_t dataLength)
-- int recvData(struct socketStruct* socket, struct destination * dest, char * dataBuffer, size_t dataBufferLength)
--
-- TCP FUNCTIONS:
-- int initSocketTCP(struct socketStruct* socketPointer)
-- int connectPort(struct socketStruct* socketPointer, struct destination* dest)
-- struct socketStruct * acceptClient(struct socketStruct* socketPointer)
-- int sendDataTCP(struct socketStruct* socketPointer, const char* data, size_t dataLength)
-- int recvDataTCP(struct socketStruct* socketPointer, char* dataBuffer, int32_t packetSize)
--
-- OTHER FUNCTIONS 
-- int getSocketError(struct socketStruct* socketPointer)
--
-- DATE: January 23rd, 2018
--
-- REVISIONS: 
--
-- DESIGNER: Cameron Roberts, Simon Wu
--
-- PROGRAMMER: Cameron Roberts, Simon Wu
--
-- NOTES:
-- The functions in this file can by either a client or server to create a TCP or UDP
-- socket as well as send and recieve data.
----------------------------------------------------------------------------------------------------------------------*/


#include "include/socket.h"


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: createSocket
--
-- DATE: January 23rd, 2019
--
-- REVISIONS: 
--
-- DESIGNER: Cameron Roberts
--
-- PROGRAMMER: Cameron Roberts
--
-- INTERFACE: struct socketStruct * createSocket()
--
-- RETURNS: On success, a pointer to a socketStruct is returned. On error, a null pointer is returned.
--
-- NOTES:
-- This function is used to allocate memory for a socketStruct which can then be passed
-- to initSocket or initSocketTCP to initialize the socket.
----------------------------------------------------------------------------------------------------------------------*/
struct socketStruct * createSocket(){
    return malloc(sizeof(struct socketStruct));
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: initSocketTCP
--
-- DATE: January 23rd, 2019
--
-- REVISIONS: 
--
-- DESIGNER: Simon Wu
--
-- PROGRAMMER: Simon Wu, Cameron Roberts
--
-- INTERFACE: int initSocketTCP(struct socketStruct* socketPointer)
--                struct socketStrict * socketPointer: A pointer to the socketStruct whose 
--                                                     socket is to be initialized.
--
-- RETURNS: On success 1 is returned. On error 0 is returned and lastError of the socket struct is set appropriately.
--    
-- NOTES:
-- This function is used to initialize the socket contained within a socketStruct as a
-- TCP socket. After the socket is initialized it should be bound to a port by calling bindPort.
----------------------------------------------------------------------------------------------------------------------*/
int initSocketTCP(struct socketStruct* socketPointer) {
  if ((socketPointer->socketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Can't create a socket for TCP");
    return 0;
  }

  return 1;
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: initSocket
--
-- DATE: January 23rd, 2019
--
-- REVISIONS: 
--
-- DESIGNER: Cameron Roberts
--
-- PROGRAMMER: Cameron Roberts
--
-- INTERFACE: int initSocket(struct socketStruct* socketPointer)
--                struct socketStrict * socketPointer: A pointer to the socketStruct whose 
--                                                     socket is to be initialized.
--
-- RETURNS: On success 1 is returned. On error 0 is returned and lastError of the socket struct is set appropriately.
--
-- NOTES:
-- This function is used to initialize the socket contained within a socketStruct as a
-- UDP socket. After the socket is initialized it should be bound to a port by calling bindPort.
----------------------------------------------------------------------------------------------------------------------*/
int32_t initSocket(struct socketStruct* socketPointer){
  if ((socketPointer->socketDescriptor = socket (AF_INET, SOCK_DGRAM, 0)) == -1)
  {
    perror ("Can't create a socket");
    socketPointer->lastError=errno;
    return 0;
  }

  return 1;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: bindPort
--
-- DATE: January 23rd, 2019
--
-- REVISIONS: 
--
-- DESIGNER: Simon Wu, Cameron Roberts
--
-- PROGRAMMER: Simon Wu, Cameron Roberts
--
-- INTERFACE: int bindPort(struct socketStruct* socketPointer, uint16_t port)
--                struct socketStrict * socketPointer: A pointer to the socketStruct whose
--                                                     socket should be bound
--                uint16_t: The port for the socket to be bound to. A port of 0 specifies 
--                          an ephemeral port
--
-- RETURNS: On success 1 is returned. On error 0 is returned and lastError of the socket struct is set appropriately.
--
-- NOTES:
-- This function is used to initialize the socket contained within a socketStruct as a
-- UDP socket. After the socket is initialized it should be bound to a port by calling
-- bindPort. Use a port of 0 to specify an ephemeral port.
----------------------------------------------------------------------------------------------------------------------*/
int32_t bindPort(struct socketStruct* socketPointer, uint16_t port) {
  struct sockaddr_in socketAddress;
  memset((char *)&socketAddress, 0, sizeof(socketAddress));
  socketAddress.sin_family = AF_INET;
  socketAddress.sin_port = port;
  socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);


  if (bind(socketPointer->socketDescriptor, (struct sockaddr*) &(socketAddress), sizeof(socketAddress)) == -1) {
    perror ("Can't bind name to socket");
    socketPointer->lastError=errno;
    return 0;
  }
  return 1;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: connectPort
--
-- DATE: January 23rd, 2019
--
-- REVISIONS: 
--
-- DESIGNER: Simon Wu
--
-- PROGRAMMER: Simon Wu, Cameron Roberts
--
-- INTERFACE: int connectPort(struct socketStruct* socketPointer, struct destination* dest)
--                struct socketStrict * socketPointer: A pointer to the socketStruct whose
--                                                     socket should be used to connect
--                struct destination* dest: A pointer to a destination constructor containing
--                                          the address and port to connect to.
--
-- RETURNS: On success 1 is returned. On error 0 is returned and lastError of the socket struct is set appropriately.
--    
-- NOTES:
-- This function is used to connect an initialized TCP socket to a destination.
----------------------------------------------------------------------------------------------------------------------*/
int connectPort(struct socketStruct* socketPointer, struct destination* dest) {
  struct sockaddr_in destSockAddr;
  memset((char *)&destSockAddr, 0, sizeof(destSockAddr));
  destSockAddr.sin_family = AF_INET;
  destSockAddr.sin_port = dest->port;
  destSockAddr.sin_addr.s_addr = dest->address;

  if (connect(socketPointer->socketDescriptor, (struct sockaddr *) &(destSockAddr), sizeof(destSockAddr)) == -1) {
    perror("Can't connect to server");
    socketPointer->lastError=errno;
    return 0;
  }
  return 1;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: acceptClient
--
-- DATE: January 23rd, 2019
--
-- REVISIONS: January 31st 2019
--              -Altered function to return a socket descriptor rather than a socketStruct
--
-- DESIGNER: Simon Wu
--
-- PROGRAMMER: Simon Wu, Cameron Roberts
--
-- INTERFACE: uint64_t acceptClient(struct socketStruct* socketPointer)
--                struct socketStrict * socketPointer: A pointer to the socketStruct whose
--                                                     socket should be to accept an incoming
--                                                     connection
--
-- RETURNS: On sucess a new socket descriptor is returned. On error NULL is
--          returned and lastError of the socket struct is set appropriately.
--
-- NOTES:
-- This function is used to accept a incoming TCP connection.
----------------------------------------------------------------------------------------------------------------------*/
int32_t acceptClient(struct socketStruct* socketPointer) {
  struct sockaddr_in clientAddr;
  memset((char *)&clientAddr, 0, sizeof(clientAddr));
  socklen_t clientAddressLength = sizeof(clientAddr);
  int32_t socketDescriptor;
  if ((socketDescriptor = accept(socketPointer->socketDescriptor, (struct sockaddr *) &clientAddr, &clientAddressLength)) == -1) {
    perror("Unable to connect to client");
    socketPointer->lastError=errno;
    return 0;
  }
  return socketDescriptor;
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: sendDataTCP
--
-- DATE: January 23rd, 2019
--
-- REVISIONS: 
--
-- DESIGNER: Simon Wu
--
-- PROGRAMMER: Simon Wu, Cameron Roberts
--
-- INTERFACE: int sendDataTCP(struct socketStruct* socketPointer, const char* data, size_t dataLength)
--                struct socketStrict * socketPointer: A pointer to the socketStruct whose
--                                                     socket should be to send the data
--                const char * data: A char array containing the data to be sent
--                size_t dataLength: The length of the data in the char array
--
-- RETURNS: On success 1 is returned. On error 0 is returned and lastError of the socket struct is set appropriately.
--
-- NOTES:
-- This function is used to send data on a connected TCP socket.
----------------------------------------------------------------------------------------------------------------------*/
int32_t sendDataTCP(struct socketStruct* socketPointer, const char* data, size_t dataLength) {
  	if (send(socketPointer->socketDescriptor, data, dataLength, 0) < 0) {
      perror("send error");
      socketPointer->lastError=errno;
      return 0;
    }
    return 1;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: sendData
--
-- DATE: January 23rd, 2019
--
-- REVISIONS: 
--
-- DESIGNER: Cameron Roberts
--
-- PROGRAMMER: Cameron Roberts
--
-- INTERFACE: int sendData(struct socketStruct* socketPointer, struct destination * dest, const char* data, size_t dataLength)
--                struct socketStrict * socketPointer: A pointer to the socketStruct whose
--                                                     socket should be to send the data
--                struct destination * dest: A destination struct containing and IP address and port
--                const char * data: A char array containing the data to be sent
--                size_t dataLength: The length of the data in the char array
--
-- RETURNS: On success 1 is returned. On error 0 is returned and lastError of the socket struct is set appropriately.
--
-- NOTES:
-- This function is used to send data on a bound UDP port. The data will be sent to the IP address and port
-- specified in the destination struct.
----------------------------------------------------------------------------------------------------------------------*/
int32_t sendData(struct socketStruct* socketPointer, struct destination * dest, const char* data, size_t dataLength){
    struct sockaddr_in destSockAddr;
    memset((char *)&destSockAddr, 0, sizeof(destSockAddr));
    destSockAddr.sin_family = AF_INET;
    destSockAddr.sin_port = dest->port;
	  destSockAddr.sin_addr.s_addr = dest->address;
    if (sendto (socketPointer->socketDescriptor, data, dataLength, 0,(struct sockaddr *)&destSockAddr, sizeof(destSockAddr)) < 0)
		{
			perror ("sendto error");
      socketPointer->lastError=errno;
			return 0;
		}
    return 1;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: recvDataTCP
--
-- DATE: January 23rd, 2019
--
-- REVISIONS: 
--
-- DESIGNER: Simon Wu
--
-- PROGRAMMER: Simon Wu, Cameron Roberts
--
-- INTERFACE: int recvDataTCP(struct socketStruct* socketPointer, char* dataBuffer, int32_t packetSize)
--                struct socketStrict * socketPointer: A pointer to the socketStruct whose
--                                                     socket should be read from
--                const char * data: A char array containing the data to be sent
--                int32_t packetSize: The number of characters to read
--
-- RETURNS: The number of characters read into dataBuffer.
--
-- NOTES:
-- This function is used to recieve data from a connected TCP socket. The function will continue
-- until packetSize characters have been read or an error occurs. If an error occurs errno will
-- be set accordingly.
----------------------------------------------------------------------------------------------------------------------*/
int32_t recvDataTCP(struct socketStruct* socketPointer, char* dataBuffer, int32_t packetSize) {
  int readCount;
  int32_t length = packetSize;
  while ((readCount = recv((socketPointer->socketDescriptor), dataBuffer, length, 0)) < length) {
    if (readCount == 0){
      // Other side disconnected
      return readCount;
    }
    if (readCount == -1) {
      perror("recv error");
      socketPointer->lastError=errno;
      return readCount;
    }
    dataBuffer += readCount;
    length -= readCount;
  }
  return packetSize-length;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: sendData
--
-- DATE: January 23rd, 2019
--
-- REVISIONS: 
--
-- DESIGNER: Cameron Roberts
--
-- PROGRAMMER: Cameron Roberts
--
-- INTERFACE: int recvData(struct socketStruct* socketPointer, char * dataBuffer, size_t dataBufferLength)
--                struct socketStrict * socketPointer: A pointer to the socketStruct whose
--                                                     socket should be read from
--                struct destination dest: A destination struct to fill with the address and port data was
--                                         recieved from
--                const char * dataBuffer: An array for received data to be placed into
--                size_t dataBufferSize: The size of dataBuffer
--
-- RETURNS: On success the number of bytes read into dataBuffer is returned. 
--          On error 0 is returned and lastError of the socket struct is set appropriately.
--
-- NOTES:
-- This function is used to receive data from a bound UDP port.
----------------------------------------------------------------------------------------------------------------------*/
int32_t recvData(struct socketStruct* socketPointer, struct destination * dest,  char * dataBuffer, size_t dataBufferSize){
    struct sockaddr_in destSockAddr;
    socklen_t destSockAddrSize = sizeof(destSockAddr);
    int bytesReceived;

    if ((bytesReceived = recvfrom (socketPointer->socketDescriptor, dataBuffer, dataBufferSize, 0, (struct sockaddr *)&destSockAddr, &destSockAddrSize)) < 0)
    {
      perror ("recvfrom error");
      socketPointer->lastError=errno;
      return -1;
    }
    dest->address = destSockAddr.sin_addr.s_addr;
    dest->port = destSockAddr.sin_port;

    return bytesReceived;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: closeSocket
--
-- DATE: January 23rd, 2019
--
-- REVISIONS: 
--
-- DESIGNER: Cameron Roberts
--
-- PROGRAMMER: Cameron Roberts
--
-- INTERFACE: int closeSocket(struct socketStruct * socketPointer)
--                struct socketStrict * socketPointer: A pointer to the socketStruct whose
--                                                     socket should be closed
--
-- RETURNS: On success 1 is returned. 
--          On error 0 is returned and lastError of the socket struct is set appropriately.
--
-- NOTES:
-- This function is used to close a socket contained within a socketStruct.
----------------------------------------------------------------------------------------------------------------------*/
int32_t closeSocket(struct socketStruct * socketPointer){
  if(close(socketPointer->socketDescriptor) == -1){
    socketPointer->lastError = errno;
    return 0;
  }
  return 1;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: freeSocket
--
-- DATE: January 23rd, 2019
--
-- REVISIONS: 
--
-- DESIGNER: Cameron Roberts
--
-- PROGRAMMER: Cameron Roberts
--
-- INTERFACE: void freeSocket(struct socketStruct * socketPointer)
--                struct socketStrict * socketPointer: A pointer to the socketStruct whose
--                                                     memory should be freed
--
-- RETURNS: void.
--
-- NOTES:
-- This function is used to free the memory allocated to a socketStruct.
----------------------------------------------------------------------------------------------------------------------*/
void freeSocket(struct socketStruct * socketPointer){
    free(socketPointer);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: getSocketError
--
-- DATE: January 23rd, 2019
--
-- REVISIONS: 
--
-- DESIGNER: Cameron Roberts
--
-- PROGRAMMER: Cameron Roberts
-- 
-- INTERFACE: int getSocketError()
--
-- RETURNS: The lastError value of the socketPointer
--
-- NOTES:
-- This function is used to retrieve errno
----------------------------------------------------------------------------------------------------------------------*/
int32_t getSocketError(struct socketStruct* socketPointer){
  return socketPointer->lastError;
}
