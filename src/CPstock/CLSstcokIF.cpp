//---------------------------------------------------------------------------

#pragma hdrstop

#include "CLSstcokIF.h"
#include "StockMain.h"
#include "StockDB.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
// Global Variable
//---------------------------------------------------------------------------
int global;
//CLSlog Log("THR_TEST1","D:\\work\\Builder\\Berlin\\01.Study PJT\\bin\\log");
extern CLSlog Log;
//CPBsHl *PBsHl;

//---------------------------------------------------------------------------
// CLSstockIF
//---------------------------------------------------------------------------
__fastcall CLSstockIF::CLSstockIF(void)
{
	//Log = CLSlog("STOCKCL", )
}
//---------------------------------------------------------------------------
__fastcall CLSstockIF::CLSstockIF(const char *name, int port, const char *ipAddr, TCP_MODE mode)
	:CLStcp(name, port, ipAddr, mode)
{

}
//---------------------------------------------------------------------------
__fastcall CLSstockIF::CLSstockIF(const char *name, int port, SOCKET id, TCP_MODE mode)
	:CLStcp(name, port, id, mode)
{

}
//---------------------------------------------------------------------------
// ~CLSstockIF
//---------------------------------------------------------------------------
__fastcall CLSstockIF::~CLSstockIF(void)
{

}
//---------------------------------------------------------------------------
// ManageConnection
//---------------------------------------------------------------------------
CON_RESULT	__fastcall  CLSstockIF::ManageConnection(void)
{
    CON_RESULT	status;
	if(Connected)
	{
		Log.Write("Conn OK");
		return (CON_OK);
	}

	Log.Write("Connection...");
	if (!Initialized && !Open())
		return (CON_ERROR);

	//연결요구
	if((status = Connect()) == CON_OK)
	{
		Log.Write("Connected");
		//InitComState(true);
		Log.Write("InitComState OK");
	}
	return (status);
}
void __fastcall CLSstockIF::InitComState(bool connected)
{
	m_msgCnt.heartTime = 5;	// 5초
	m_msgCnt.readyHeart = false;
	m_txSeq = 0;
    m_rxSeq = 0;
}
//---------------------------------------------------------------------------
// CloseNetwork
//---------------------------------------------------------------------------
void __fastcall CLSstockIF::CloseNetwork(char *message)
{
	//Log.Write(message);
	Close();
}
//---------------------------------------------------------------------------
// Manage
//---------------------------------------------------------------------------
bool __fastcall  CLSstockIF::Manage(void)
{
    //Manage connection

	time(&m_curClock);		// 현재 시각 갱신

	switch (ManageConnection())
	{
	case CON_ERROR: CloseNetwork("Network Abnormal"); return(true);
	case CON_INPROGRESS:
	{
        Log.Write("INPROGRESS");
		return(true);
	}
	default: break;
	}

	if(!ManageRX())
	{
		Log.Write("RX fail");
		Close();
		return (false);
    }

	if (!ManageTX())
	{
        Log.Write("TX fail");
		Close();
		return (false);
	}
    return (true);
}
//---------------------------------------------------------------------------
// ManageRX
//---------------------------------------------------------------------------
bool __fastcall CLSstockIF::ManageRX(void)
{
	AnsiString s = "";
	int count;
	char buffer[TCPBUF_LEN];

	//Read
	if((count = Recv(buffer, TCPBUF_LEN)) <=0)
	{
		if(count < 0)
		{
			AnsiString s = "";
			s.printf("Abnomal read [%d][%d]", count, errno);
			Log.Write(s.c_str());
		}

		return ((count < 0 )?false : true);
	}

	Log.Write("RECV : %s [%d]", buffer, count);
	Log.FLdump("RX DATA", buffer, count, count);
	Message.str.printf("RECV:[%s]", buffer);
	PostMessage(StockMainF->Handle, WM_MSGLOG, (WPARAM)0, (LPARAM)0);
	return (true);
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
bool __fastcall CLSstockIF::ManageTX(void)
{
	// Line check (Heart beat check)
	if (CheckElapsedTime(&m_checkTimer, HEART_INTERVAL))
		return(SendHeartBeat());

	return (true);
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
bool __fastcall CLSstockIF::SendMessage(BYTE code, int length, char *info)
{
	int lrcLen = 1;
	int txLength = length + HEARD_LEN + lrcLen;
	char message[TCPBUF_LEN];
	memset(message, 0, TCPBUF_LEN);
	message[STX1] = STX1_CHAR;
	message[STX2] = STX2_CHAR;
	message[STATUS] = 0;
	message[OPCODE] = code;

	switch(code)
	{
	case HEARTBEAT:
		message[SEQ] = m_txSeq++;
        break;
	default :
		Log.Write("Undifined TX code [%02X]", code);
		return (true);
	}

	if( code == HEARTBEAT)
		message[DATA] = 0;	// LRC;
		SetNumber(&message[SIZE1], 1, 2);

	if (!Send(message, txLength)) {
		Log.FLdump("TX DATA FAIL", message, txLength, txLength);
		return (false);
	}
	Log.FLdump("TX DATA", message, txLength, txLength);
	SendMsgLog.str.printf("TX DATA [%02x] [%02x]", message[OPCODE], message[SEQ]);
	PostMessage(StockMainF->Handle, WM_SENDLOG, (WPARAM)0, (LPARAM)0);
	if(m_txSeq == 255)  m_txSeq = 0;

    return (true);
}
//---------------------------------------------------------------------------
// SendEcho
//---------------------------------------------------------------------------
bool __fastcall CLSstockIF::SendEcho(char *str)
{
	char buffer[256];
	int length=0;
	sprintf(buffer, "Hello World");
	length = sizeof(buffer);

	if(Send(buffer, sizeof(buffer)))
	{
		return (true);
	}
	else
	{
        return (false);
	}

	//return (SendMessage(0, length, buffer));
}
bool __fastcall CLSstockIF::SendHeartBeat(void)
{
	char info;
	Log.Write("HEAR BEAT [%02X]", HEARTBEAT);
	return (SendMessage(HEARTBEAT, 0, &info));

}
