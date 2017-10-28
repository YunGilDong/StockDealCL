//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "StockMain.h"
#include "StockDB.h"
#include "CPstock.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
// Global
//---------------------------------------------------------------------------
TStockMainF *StockMainF;
CLSlog Log;
//---------------------------------------------------------------------------
// Local
//---------------------------------------------------------------------------
AnsiString LogDir;
//---------------------------------------------------------------------------
__fastcall TStockMainF::TStockMainF(TComponent* Owner)
	: TForm(Owner)
{
	// Set Log directory
	LogDir = ExtractFilePath(Application->ExeName);
	LogDir.printf("%s\\log", LogDir.c_str());
//	SetCurrentDirectory(LogDir.c_str());

	//Log = CLSlog("STOCKCL", LogDir.c_str());
	Log = CLSlog("STOCKCL", "D:\\work\\Builder\\Berlin\\01.Study PJT_\\bin\\log");
	Log.Write("Process start");

	mTcpSt = false;
	//Pstock = new CPstock(commEnv.ip, commEnv.port);
	Pstock = new CPstock("210.220.167.67", 12000);
	tmTcpSt->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TStockMainF::tmTcpStTimer(TObject *Sender)
{
	if(mTcpSt != Pstock->GetTcpStatus())
	{
		mTcpSt = Pstock->GetTcpStatus();
		lbCommSt->Caption = (mTcpSt)?"ON":"OFF";
	}
}
//---------------------------------------------------------------------------
void __fastcall TStockMainF::btnEchoClick(TObject *Sender)
{
//
	if(Pstock->SendEcho("Hello world"))
	{
		ShowMessage("success");
	}
	else
	{
        ShowMessage("fail");
    }

}
//---------------------------------------------------------------------------
void __fastcall TStockMainF::fnMessageLog(TMessage Msg)
{
	lbLog->Items->Add(Message.str);
}
void __fastcall TStockMainF::fnMessageSendLog(TMessage Msg)
{
    lbSendLog->Items->Add(SendMsgLog.str);
}
