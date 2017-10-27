//---------------------------------------------------------------------------

#pragma hdrstop

#include "THRclient.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
// External
//---------------------------------------------------------------------------
extern int global;
extern CLSlog Log;
//---------------------------------------------------------------------------
// THRclient
//---------------------------------------------------------------------------
__fastcall THRclient::THRclient(void) : TThread(true)
{
	ostringstream oss;
	global = 5;
	oss<<global;
	string s=oss.str();

	Log.Write(s.c_str());
}
//---------------------------------------------------------------------------
__fastcall THRclient::THRclient(CLSstockIF *tcpClient) : TThread(true)
{
	m_tcpClinet = tcpClient;
}
//---------------------------------------------------------------------------
// test
//---------------------------------------------------------------------------
void __fastcall THRclient::test()
{
	ostringstream oss;
	oss<<global;
	string s=oss.str();

	Log.Write(s.c_str());
}
//---------------------------------------------------------------------------
// start
//---------------------------------------------------------------------------
void __fastcall THRclient::start()
{
	Start();
}
//---------------------------------------------------------------------------
// stop
//---------------------------------------------------------------------------
void __fastcall THRclient::stop()
{
	Terminate();
}
//---------------------------------------------------------------------------
// TCLmanage
//---------------------------------------------------------------------------
bool __fastcall THRclient::TCLmanage(void)
{
	return (m_tcpClinet->Manage());
	//return (true);
}
void __fastcall THRclient::TCLclearEnv(void)
{
    m_tcpClinet->CloseNetwork("Socket Close");
}
//---------------------------------------------------------------------------
// Execute
//---------------------------------------------------------------------------
void __fastcall THRclient::Execute()
{
	int cycle = 0;
	while( !Terminated )
	{
		if (cycle % 10 == 0) {
			cycle++;
			Log.Write("thread run[%d]", cycle);
			if (cycle == 100000000) cycle = 0;
		}

		// 立加 包府
		if (!TCLmanage())
		{
			Log.Write("TCP Mng false");
			TCLclearEnv();
//			break;
		}

		Sleep(1000);
	}
	Log.Write("Thread exit");
	TCLclearEnv();
}
