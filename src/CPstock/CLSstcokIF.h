//---------------------------------------------------------------------------

#ifndef CLSstcokIFH
#define CLSstcokIFH
//---------------------------------------------------------------------------
// Include
//---------------------------------------------------------------------------
#include "CLStcp.h"
#include "CLSlog.h"
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
// Constant
//---------------------------------------------------------------------------
#define TCPBUF_LEN  1024
#define HEARD_LEN   7
#define HEART_INTERVAL  5000	// 5 second
//---------------------------------------------------------------------------
// Protocol
//---------------------------------------------------------------------------
#define STX1_CHAR 0x7E
#define STX2_CHAR 0x7E
//---------------------------------------------------------------------------
#define STX1    0
#define STX2    STX1+1
#define SIZE1   STX2+1
#define SIZE2   SIZE1+1
#define STATUS  SIZE2+1
#define OPCODE  STATUS+1
#define SEQ     OPCODE+1
#define DATA    SEQ+1
//---------------------------------------------------------------------------
#define HEARTBEAT   0x01
#define BUY_SIG     0x20
#define SELL_SIG    0x21
#define ACK         0x80
#define NACK        0x81
//---------------------------------------------------------------------------
// Type Definition
//---------------------------------------------------------------------------
typedef struct
{
	int heartTime;					// 전송 주기 시간(second)
	struct timeval heartSendTime;   // Heart 전송 시각
	char message[TCPBUF_LEN];
	int seq;
	bool readyHeart;    			// Heartbeat를 수신했을때
} MSG_CNTL;
//---------------------------------------------------------------------------
// Class
//---------------------------------------------------------------------------
class CLSstockIF : public CLStcp
{
private:
	MSG_CNTL m_msgCnt;
	time_t m_curClock;
	int m_txSeq;
	int m_rxSeq;
	struct timeval	m_checkTimer;

	CON_RESULT	__fastcall ManageConnection(void);

public:
	__fastcall CLSstockIF(void);
	__fastcall CLSstockIF(const char *name, int port, const char *ipAddr, TCP_MODE mode = TCP_SERVER);
	__fastcall CLSstockIF(const char *name, int port, SOCKET id, TCP_MODE mode = TCP_SERVER);
	__fastcall ~CLSstockIF(void);

	void __fastcall InitComState(bool connected = false);
	void __fastcall CloseNetwork(char *message);
	bool __fastcall Manage(void);

	bool __fastcall ManageRX(void);
	bool __fastcall ManageTX(void);
    bool __fastcall SendMessage(BYTE code, int length, char *info);

	bool __fastcall SendEcho(char *str);
	bool __fastcall SendHeartBeat(void);
};
#endif
