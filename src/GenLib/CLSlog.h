//---------------------------------------------------------------------------
#ifndef CLSlogH
#define CLSlogH


// epoch time으로 변환할 상수
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

//---------------------------------------------------------------------------
// Constant
//---------------------------------------------------------------------------
#define  BUFFER_LEN   1024
#define  FILENAME_LEN 256
#define  OBJ_NAME_LEN 64

//---------------------------------------------------------------------------
// Include
//---------------------------------------------------------------------------
#include "General.h"
#include <time.h>
#include <Winsock2.h>	//m sec까지 구하기 위함
#include <stdarg.h>

//---------------------------------------------------------------------------
// Constant
//---------------------------------------------------------------------------
#define	DEFAULT_DUMP_LEN	20
//---------------------------------------------------------------------------
// Struct
//---------------------------------------------------------------------------
// for timezone
//---------------------------------------------------------------------------
struct timezone2
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};

//---------------------------------------------------------------------------
// Class
//---------------------------------------------------------------------------
class CLSlog
{
private:
	char m_buffer[BUFFER_LEN];
	char m_dir[FILENAME_LEN];
	char m_name[OBJ_NAME_LEN];
	char m_timeStamp[BUFFER_LEN];
	struct tm m_tod;

	void __fastcall Logging(void);
	char * __fastcall GetTimeString(void);
	char * __fastcall GetTimeString(struct tm *pTod);   //Tod is today
	int __fastcall gettimeofday(struct timeval *tv, struct timezone2 *tz);
public:
	__fastcall CLSlog(void);
	__fastcall CLSlog(const char *name, const char *pDir = NULL);
	__fastcall ~CLSlog(void);
	void __fastcall SetLogdir(const char *pDir);

	void FLdump(const char *stamp, char *data, int length, int maxLength);
	void Write(const char *format, ...);
};
#endif
