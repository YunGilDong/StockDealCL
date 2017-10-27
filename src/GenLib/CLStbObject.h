//---------------------------------------------------------------------------

#ifndef CLStbObjectH
#define CLStbObjectH
//---------------------------------------------------------------------------
// epoch time으로 변환할 상수
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

//------------------------------------------------------------------------------
// Include
//------------------------------------------------------------------------------
#include "General.h"
#include <time.h>
#include <Winsock2.h>	//CheckElapsedTime 구하기 위함

//------------------------------------------------------------------------------
// Struct
//------------------------------------------------------------------------------
// for timezone
//---------------------------------------------------------------------------
struct timezone
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};

//------------------------------------------------------------------------------
// macro
//------------------------------------------------------------------------------
#ifdef _WIN32
inline void timersub(const timeval* tvp, const timeval* uvp, timeval* vvp)
{
  vvp->tv_sec = tvp->tv_sec - uvp->tv_sec;
  vvp->tv_usec = tvp->tv_usec - uvp->tv_usec;
  if (vvp->tv_usec < 0)
  {
	 --vvp->tv_sec;
	 vvp->tv_usec += 1000000;
  }
}
#endif

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class CLStbObject
{
private:
public:
	CLStbObject(void);
	CLStbObject(const char *name);
	~CLStbObject(void);

	bool CheckElapsedTime(struct timeval *timer, int duration, int offset = 0);
	int  gettimeofday(struct timeval *tv, struct timezone *tz);
	int  TimerToMsec(struct timeval *timer);
};
//------------------------------------------------------------------------------
#endif // CLStbObjectH
//------------------------------------------------------------------------------
// End of CLStbObject.h
//------------------------------------------------------------------------------
