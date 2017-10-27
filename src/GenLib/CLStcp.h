//---------------------------------------------------------------------------
#ifndef CLStcpH
#define CLStcpH
//---------------------------------------------------------------------------
//Include
//------------------------------------------------------------------------------
#include "General.h"
#include "CLStbObject.h"
#include <winsock2.h>
//------------------------------------------------------------------------------
// Type definition
//------------------------------------------------------------------------------
// COM_STATE
//------------------------------------------------------------------------------
typedef enum {ENDIAN_BIG, ENDIAN_LITTLE}	COM_ENDIAN;
typedef enum {COM_START, COM_STOP}	COM_STATE;
// CON_RESULT
//------------------------------------------------------------------------------
typedef enum {CON_ERROR, CON_INPROGRESS, CON_OK}	CON_RESULT;
//------------------------------------------------------------------------------
// TCP_MODE
//------------------------------------------------------------------------------
typedef enum {TCP_SERVER, TCP_CLIENT, TCP_CHILD}	TCP_MODE;
//------------------------------------------------------------------------------

class CLStcp : public CLStbObject
{
private:
	int m_port;
	char m_ipAddr[IPADDR_LEN];
	TCP_MODE m_mode;

	void __fastcall	Init(int port, SOCKET id, const char *ipAddr, TCP_MODE mode);
	int  __fastcall IsDataReady(void);
	void __fastcall	SetOption(void);

public:
	SOCKET ID;
	int NewSocket;
	bool Connected;
	bool Initialized;

	__fastcall CLStcp(void);
	__fastcall CLStcp(const char *name, int port, SOCKET id, TCP_MODE mode = TCP_CHILD);
	__fastcall CLStcp(const char *name, int port, const char *ipAddr, TCP_MODE mode = TCP_SERVER);
	__fastcall ~CLStcp(void);

	bool __fastcall	Accept(bool *newConnection, char *remoteIP = NULL);
	void __fastcall Close(void);
	CON_RESULT __fastcall Connect(void);
	bool __fastcall	Open(void);
	int	 __fastcall Recv(char *ptr, int length);
	bool __fastcall	Send(char *ptr, int length);

	void __fastcall SetComState(COM_STATE state);

	UINT 	GetNumber(char *ptr, int length = 2, COM_ENDIAN endian = ENDIAN_BIG);
	void	SetNumber(char *ptr, UINT value, int length = 2, COM_ENDIAN endian = ENDIAN_BIG);
	int		String2Int(char *pData, int length = 2);
};
#endif
//------------------------------------------------------------------------------
// End of CLStcp.h
//------------------------------------------------------------------------------
