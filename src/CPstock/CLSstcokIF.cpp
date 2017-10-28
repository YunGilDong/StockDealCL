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
	InitComState();
}
//---------------------------------------------------------------------------
__fastcall CLSstockIF::CLSstockIF(const char *name, int port, const char *ipAddr, TCP_MODE mode)
	:CLStcp(name, port, ipAddr, mode)
{
	InitComState();
}
//---------------------------------------------------------------------------
__fastcall CLSstockIF::CLSstockIF(const char *name, int port, SOCKET id, TCP_MODE mode)
	:CLStcp(name, port, id, mode)
{
	InitComState();
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
		//Log.Write("Conn OK");
		return (CON_OK);
	}

	Log.Write("Connection...");
	if (!Initialized && !Open())
		return (CON_ERROR);

	//연결요구
	if((status = Connect()) == CON_OK)
	{
		Log.Write("Connected");
		InitComState(true);
		Log.Write("InitComState OK");
	}
	return (status);
}
//---------------------------------------------------------------------------
// GenLRC
//---------------------------------------------------------------------------
BYTE __fastcall CLSstockIF::GenLRC(char *message, int length)
{
	BYTE lrc = 0;
	for(int idx = 0; idx < length; idx++)
		lrc ^= *(message + idx);
	return (lrc);
}
//---------------------------------------------------------------------------
// SetRxState
//---------------------------------------------------------------------------
void __fastcall CLSstockIF::SetRxState(RX_STATE state, int delta)
{
	m_state = state;
	switch(state)
	{
	case COMST_STX1:
		m_tally = 0;
		if (delta)
			m_index += delta;
		break;
	default:
		m_tally += delta;
		m_index += delta;
		break;
    }
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall CLSstockIF::InitComState(bool connected)
{
	m_txSeq = 0;
	m_rxSeq = 0;
	SetRxState(COMST_STX1);
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
	RxHandler(buffer, count);
	//Log.Write("RECV : %s [%d]", buffer, count);
	//Log.FLdump("RX DATA", buffer, count, count);
	Message.str.printf("RECV:[%s]", buffer);
	PostMessage(StockMainF->Handle, WM_MSGLOG, (WPARAM)0, (LPARAM)0);
	return (true);
}
//---------------------------------------------------------------------------
// ManageTX
//---------------------------------------------------------------------------
bool __fastcall CLSstockIF::ManageTX(void)
{
	// Line check (Heart beat check)
	if (CheckElapsedTime(&m_checkTimer, HEART_INTERVAL))
		return(SendHeartBeat());

	return (true);
}
//---------------------------------------------------------------------------
// RxHandler
//---------------------------------------------------------------------------
void __fastcall CLSstockIF::RxHandler(char *buffer, int length)
{
	int remain, request;
	char *dPtr, *sPtr;

	Log.FLdump("RAW", buffer, length, length);
	for(m_index = 0; m_index < length;)
	{
		dPtr = &m_message[m_tally];
		sPtr = buffer+m_index;
		//sPtr = &buffer[m_index];
		switch(m_state)
		{
		case COMST_STX1:
			if(*sPtr == STX1_CHAR)
			{
				*dPtr = *sPtr;
				SetRxState(COMST_STX2, 1);
			}
			else    SetRxState(COMST_STX1, 1);
			break;
		case COMST_STX2:
			if(*sPtr == STX2_CHAR)
			{
				*dPtr = *sPtr;
				SetRxState(COMST_SIZE1, 1);
			}
			else    SetRxState(COMST_STX1, 1);
			break;
		case COMST_SIZE1:
			*dPtr = *sPtr;
			SetRxState(COMST_SIZE2, 1);
			break;
		case COMST_SIZE2:
			*dPtr = *sPtr;
			m_length = GetNumber(dPtr-1, 2);
			//m_length = GetNumber(&m_message[SIZE1],2);
			m_length += HEARD_LEN;
			SetRxState(COMST_DATA, 1);
			break;
		case COMST_DATA:
			remain = length - m_index;
			request = m_length - m_tally;
			if(remain > 0 && request > 0)
			{
				request = min(remain, request);
				memcpy(dPtr, sPtr, request);
				SetRxState(COMST_DATA, request);
			}
			if(m_tally < m_length)
			 	break;

			MsgHandler();
		default:    SetRxState(COMST_STX1, 1); break;
		}
    }
}
//---------------------------------------------------------------------------
// MsgHandler
//---------------------------------------------------------------------------
void __fastcall CLSstockIF::MsgHandler(void)
{
	BYTE code = m_message[OPCODE];

	// 수신 메시지 확인
	m_rxSeq = m_message[SEQ];
	Log.FLdump("RX", m_message, m_tally, m_tally);

	// 수신 메시지 처리
	switch(code)
	{
	case BUY_SIG:
	case SELL_SIG:
		PrcTradeSignal();
		break;
    }
}
//---------------------------------------------------------------------------
// SendMessage
//---------------------------------------------------------------------------
bool __fastcall CLSstockIF::SendMessage(BYTE code, int length, char *info)
{
	int lrcLen = 1;
	int txLength = length + HEARD_LEN + lrcLen;
	int dataLen = length + lrcLen;
	int lrcIdx = length + HEARD_LEN;
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
	case ACK:
	case NACK:
		message[SEQ] = m_rxSeq++;
		break;
	default :
		Log.Write("Undifined TX code [%02X]", code);
		return (true);
	}

	SetNumber(&message[SIZE1], dataLen, 2);
	message[lrcIdx] = GenLRC(message, lrcIdx);	// LRC;

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
// PrcTradeSignal
//---------------------------------------------------------------------------
void __fastcall CLSstockIF::PrcTradeSignal(void)
{
	int idx = 0;
	int sigTime;
	char buffer[TCPBUF_LEN];
	memcpy(buffer, &m_message[DATA], m_length-HEARD_LEN);
	TDINFO.type = buffer[idx];      idx += 1;

	sigTime = GetNumber(&buffer[idx], 4);
	TDINFO.minute = sigTime % 100;    sigTime /= 100;
	TDINFO.hour = sigTime % 100;      sigTime /= 100;
	TDINFO.day = sigTime % 100;   	  sigTime /= 100;
	TDINFO.mon = sigTime % 100;		  idx += 4;
	memcpy(TDINFO.stockCode, &buffer[idx], 7);      idx += 7;
	memcpy(TDINFO.stockNm, &buffer[idx], 32);    	idx += 32;
	TDINFO.price = GetNumber(&buffer[idx], 4);      idx += 4;

	Log.Write("[%c]\t[%d]:[%d]:[%d]:[%d]\t[%s][%s] : [%d]"
		, TDINFO.type, TDINFO.mon, TDINFO.day, TDINFO.hour, TDINFO.minute, TDINFO.stockCode,TDINFO.stockNm, TDINFO.price);

	Message.str.printf("[TradeSignal] [%c]\t[%d]:[%d]:[%d]:[%d]\t[%s][%s] : [%d]"
		, TDINFO.type, TDINFO.mon, TDINFO.day, TDINFO.hour, TDINFO.minute, TDINFO.stockCode,TDINFO.stockNm, TDINFO.price);
	PostMessage(StockMainF->Handle, WM_MSGLOG, (WPARAM)0, (LPARAM)0);

	SendACK(m_message[OPCODE]);
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
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
bool __fastcall CLSstockIF::SendHeartBeat(void)
{
	char info;
	Log.Write("HEAR BEAT [%02X]", HEARTBEAT);
	return (SendMessage(HEARTBEAT, 0, &info));

}
//---------------------------------------------------------------------------
// SendACK
//---------------------------------------------------------------------------
bool __fastcall CLSstockIF::SendACK(BYTE code)
{
	char info;
	Log.Write("ACK [%02X]", ACK);
	info = code;
	return (SendMessage(ACK, 1, &info));
}
//---------------------------------------------------------------------------
// SendNACK
//---------------------------------------------------------------------------
bool __fastcall CLSstockIF::SendNACK(BYTE code)
{
	char info;
	Log.Write("NACK [%02X]", NACK);
	info = code;
	return (SendMessage(NACK, 1, &info));
}
