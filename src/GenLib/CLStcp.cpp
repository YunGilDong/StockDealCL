//---------------------------------------------------------------------------

#pragma hdrstop

#include "CLStcp.h"
#include "CLSlog.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)


//CLSlog Log("THR_TEST1","D:\\work\\Builder\\Berlin\\01.Study PJT\\bin\\log");
extern CLSlog Log;
//------------------------------------------------------------------------------
// CLStcp
//------------------------------------------------------------------------------
__fastcall CLStcp::CLStcp()
{
	Connected = false;
}
//------------------------------------------------------------------------------
__fastcall CLStcp::CLStcp(const char *name, int port, SOCKET id, TCP_MODE mode)
{
	Connected = false;
	Init(port, id, NULL, mode);
}
//------------------------------------------------------------------------------
__fastcall CLStcp::CLStcp(const char *name, int port, const char *ipAddr, TCP_MODE mode)
{
    Connected = false;
	Init(port, 0, ipAddr, mode);
}
//------------------------------------------------------------------------------
// ~CLStcp
//------------------------------------------------------------------------------
__fastcall CLStcp::~CLStcp(void)
{
}
//------------------------------------------------------------------------------
// Accept
//------------------------------------------------------------------------------
bool __fastcall CLStcp::Accept(bool *newConnection, char *remoteIP)
{
	int	length;
	struct sockaddr_in	client;

	*newConnection = false;
	length = sizeof(struct sockaddr_in);
	if ((NewSocket = accept(ID, (struct sockaddr *)&client, &length)) < 0)
	{
		if (WSAGetLastError() == EAGAIN || WSAGetLastError() == WSAEWOULDBLOCK)
			return(true);

		return(false);
	}
	*newConnection = true;
	if (remoteIP != NULL)
	{
		getpeername(NewSocket, (struct sockaddr *)&client, &length);    //연결된 호스트정보를 알아온다.
		sprintf(remoteIP, "%s", inet_ntoa(client.sin_addr));
	}
	Connected = true;
	return(true);
}
//------------------------------------------------------------------------------
// Close
//------------------------------------------------------------------------------
void __fastcall CLStcp::Close(void)
{
	if (ID < 0)
	{
        Log.Write("Socket close No ID");
		return;
	}

	closesocket(ID);
	WSACleanup();
	ID = -1;
	Initialized = false;
	Connected = false;
	Log.Write("Socket close");
}
//------------------------------------------------------------------------------
// Connect
//------------------------------------------------------------------------------
CON_RESULT __fastcall CLStcp::Connect(void)
{
	int error, retVal;
	int length = sizeof(struct sockaddr_in);
	fd_set readFds, writeFds;
	int optLength;
	struct sockaddr_in server;
	struct timeval	timeout = {0, 50000};		// 50 msec (50000/1000000)

	memset(&server, 0, length);
	server.sin_family = AF_INET;
	server.sin_port=htons(m_port);
	server.sin_addr.s_addr = inet_addr(m_ipAddr);

	if(connect(ID, (struct sockaddr *)&server, length) >= 0)
	{
		Connected = true;
		return (CON_OK);
	}
	//접속 Error 확인
	if(WSAGetLastError() == WSAEWOULDBLOCK)
	{
		Sleep(100);
		//return(CON_ERROR);
		return(CON_INPROGRESS);
	}

	FD_ZERO(&readFds);
	FD_SET(ID, &readFds);
	FD_ZERO(&writeFds);
	FD_SET(ID, &writeFds);

    if ((retVal = select(ID+1, &readFds, &writeFds, NULL, &timeout)) < 0)
	{
		return(CON_ERROR);
	}
	if (FD_ISSET(ID, &readFds) || FD_ISSET(ID, &writeFds))
	{
		optLength = sizeof(int);
		if (getsockopt(ID, SOL_SOCKET, SO_ERROR, (char*)&error, &optLength) < 0)	//
		{
			return(CON_ERROR);
		}
		if (!error)
		{
			Connected = true;
			return(CON_OK);
		}
	}
	return(CON_INPROGRESS);
}
//------------------------------------------------------------------------------
// Init
//------------------------------------------------------------------------------
void __fastcall CLStcp::Init(int port, SOCKET id, const char *ipAddr, TCP_MODE mode)
{
	m_port = port;
	if (ipAddr != NULL)
		sprintf(m_ipAddr, "%s", ipAddr);
	else	memset(m_ipAddr, 0, IPADDR_LEN);
	m_mode = mode;
	if (m_mode != TCP_CHILD)
		return;

	ID = id;
	Initialized = true;
	Connected = true;
}
//------------------------------------------------------------------------------
// IsDataReady
//------------------------------------------------------------------------------
int __fastcall CLStcp::IsDataReady(void)
{
	int		retVal;
	fd_set	expFds, readFds;
	struct timeval	rxTimeout;

	// SOCKET ID 확인
	if (ID <= 0)
		return(-1);

	// 정보 수신 확인
	FD_ZERO(&readFds);
	FD_ZERO(&expFds);
	FD_SET(ID, &readFds);
	FD_SET(ID, &expFds);
	rxTimeout.tv_sec = 0;
	rxTimeout.tv_usec = 10000;
	if ((retVal = select(ID+1, &readFds, 0, &expFds, &rxTimeout)) <= 0)
		return(retVal);
	if (FD_ISSET(ID, &expFds))
	{
		return(-1);
	}
	if (FD_ISSET(ID, &readFds))
		return(1);

//	if (retVal == 0) {
//        return -1;
//	}

	return(0);
}
//------------------------------------------------------------------------------
// Open
//------------------------------------------------------------------------------
bool __fastcall CLStcp::Open(void)
{
	int length = sizeof(struct sockaddr_in);
	WSADATA wsaData;
	struct sockaddr_in server;

	if( WSAStartup(MAKEWORD(2,2), &wsaData)!=0 )
	{
		return false;
	}
	// Open socket
	if((ID = socket(AF_INET,SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
//		Debug("Socket open fail");
		return(false);
	}

	SetOption();				// Set socket option

	if (m_mode != TCP_SERVER)
	{
		Initialized = true;
		return(true);
	}

	// Bind socket
	memset(&server, 0, length);
	server.sin_family = AF_INET;
	server.sin_port = htons(m_port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind(ID, (const struct sockaddr *)&server, sizeof(server));
	/*
	if ((ret = bind(ID, (const struct sockaddr *)&server, sizeof(server))) != 0)
	{
		return (false);
	}
    */

	if( bind (ID, (const struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		return (false);
	}

	// Listen socket
	if (listen(ID, 10) == SOCKET_ERROR)
	{
		return(false);
	}

	Initialized = true;
	return(true);
}
//------------------------------------------------------------------------------
// Read
//------------------------------------------------------------------------------
int __fastcall CLStcp::Recv(char *ptr, int length)
{
	int		count, status;

	// 수신할 정보가 있는지 확인
	if ((status = IsDataReady()) <= 0)
		return(status);

	if ((count = recv(ID, ptr, length, 0)) < 0)
	{
		return(-1);
	}
	// recv 오류체크 (temp)
	// IsDataReady상태인데 count가 0 일경우
	if (count == 0) {
        return (-1);
	}

	return(count);
}
//------------------------------------------------------------------------------
// SetOption
//------------------------------------------------------------------------------
void __fastcall CLStcp::SetOption(void)
{
	int		flag = 1;
	u_long iMode = 1;

	setsockopt(ID, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(int));
	setsockopt(ID, SOL_SOCKET, SO_KEEPALIVE, (char *)&flag, sizeof(int));
	setsockopt(ID, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));   //remove nagle algorithm

    //-------------------------
	// Set the socket I/O mode: In this case FIONBIO
	// enables or disables the blocking mode for the
	// socket based on the numerical value of iMode.
	// If iMode = 0, blocking is enabled;
	// If iMode != 0, non-blocking mode is enabled.
	ioctlsocket(ID, FIONBIO, &iMode);
//	ioctl(ID, FIONBIO, &flag);     //linux
}
//------------------------------------------------------------------------------
// Write
//------------------------------------------------------------------------------
bool __fastcall CLStcp::Send(char *buffer, int length)
{
	int		count;

	if ((count = send(ID, buffer, length, 0)) < 0)
		return(false);

	return((count == length) ? true : false);
}
//------------------------------------------------------------------------------
// GetNumber
//------------------------------------------------------------------------------
UINT CLStcp::GetNumber(char *ptr, int length, COM_ENDIAN endian)
{
	UINT	value = 0;

	switch (endian)
	{
	case ENDIAN_BIG:
		for (int idx = 0; idx < length; idx++)
			value = (value * 256) + (BYTE)*(ptr+idx);
		break;
	default:
		for (int idx = length-1; idx >= 0; idx--)
			value = (value * 256) + (BYTE)*(ptr+idx);
		break;
	}
	return(value);
}
//------------------------------------------------------------------------------
// SetNumber
//------------------------------------------------------------------------------
void CLStcp::SetNumber(char *ptr, UINT value, int length, COM_ENDIAN endian)
{
	switch (endian)
	{
	case ENDIAN_BIG:
		for (int idx = length-1; idx >= 0; idx--, value /= 256)
			*(ptr+idx) = value % 256;
		break;
	default:
		for (int idx = 0; idx < length; idx++, value /= 256)
			*(ptr+idx) = value % 256;
		break;
	}
}
//------------------------------------------------------------------------------
// String2Int
//------------------------------------------------------------------------------
int CLStcp::String2Int(char *pData, int length)
{
	char	buffer[SHORTBUF_LEN];

	if (length >= SHORTBUF_LEN - 1)
		return(-1);

	memcpy(buffer, pData, length);
	buffer[length] = 0;
	return(atoi(buffer));
}
//------------------------------------------------------------------------------
// End of CLScomm.cpp
//------------------------------------------------------------------------------
