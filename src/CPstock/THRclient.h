//---------------------------------------------------------------------------

#ifndef THRclientH
#define THRclientH
//---------------------------------------------------------------------------
// Include
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "General.h"
#include "CLSlog.h"
#include "CLSstcokIF.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Constant
//---------------------------------------------------------------------------
// Forward Declaration
//---------------------------------------------------------------------------
//class CLSBsHlIF;
//---------------------------------------------------------------------------
// Class
//---------------------------------------------------------------------------
class THRclient : public TThread
{
private:
	CLSstockIF *m_tcpClinet;

	bool __fastcall TCLmanage(void);
	void __fastcall TCLclearEnv(void);
protected:
	virtual void __fastcall Execute(void);

public:
	__fastcall THRclient(void);
	__fastcall THRclient(CLSstockIF *tcpClient);

	void __fastcall start();
	void __fastcall stop();

	void __fastcall test();
};
#endif
