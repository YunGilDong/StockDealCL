//---------------------------------------------------------------------------

#ifndef CPstockH
#define CPstockH
//---------------------------------------------------------------------------
// Include
//---------------------------------------------------------------------------
#include "CLSstcokIF.h"
#include "CLSlog.h"
#include "THRclient.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Class
//---------------------------------------------------------------------------
class CPstock
{
private:
	CLSstockIF *m_tcpClinet;
	char *m_tcpClientIP;
	int port;
	CLSlog	m_log;
	THRclient *m_thr;
    BYTE m_seq;

	bool __fastcall InitNetwork();

	int __fastcall MainLoop();
public:
	CPstock(void);
	CPstock(const char *ipAddr, int port);
	~CPstock(void);

	bool __fastcall InitEnv();
	void __fastcall ClearEnv();
	void __fastcall ManageThread();

	bool __fastcall GetTcpStatus();
	bool __fastcall GetCommStatus();

	bool __fastcall SendEcho(char *str);
};
//---------------------------------------------------------------------------
// External
//---------------------------------------------------------------------------
extern CPstock *Pstock;

#endif
