//---------------------------------------------------------------------------

#ifndef StockMainH
#define StockMainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
// User Message
//---------------------------------------------------------------------------
#define WM_MSGLOG              (WM_USER + 0x01)
#define WM_SENDLOG              (WM_USER + 0x02)
//---------------------------------------------------------------------------
// Class
//---------------------------------------------------------------------------
class TStockMainF : public TForm
{
__published:	// IDE-managed Components
	TButton *btnEcho;
	TButton *Button2;
	TButton *Button3;
	TTimer *tmTcpSt;
	TLabel *lbCommSt;
	TListBox *lbLog;
	TListBox *lbSendLog;
	TLabel *Label1;
	TLabel *Label2;
	void __fastcall tmTcpStTimer(TObject *Sender);
	void __fastcall btnEchoClick(TObject *Sender);
private:	// User declarations
	bool mTcpSt;
public:		// User declarations

	__fastcall TStockMainF(TComponent* Owner);

	void __fastcall fnMessageLog(TMessage Msg);
	void __fastcall fnMessageSendLog(TMessage Msg);

	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_MSGLOG,     TMessage, fnMessageLog);
		VCL_MESSAGE_HANDLER(WM_SENDLOG,     TMessage, fnMessageSendLog);
	END_MESSAGE_MAP(TForm);
};
//---------------------------------------------------------------------------
extern PACKAGE TStockMainF *StockMainF;
//---------------------------------------------------------------------------
#endif
