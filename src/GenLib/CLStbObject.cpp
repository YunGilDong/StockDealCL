//---------------------------------------------------------------------------

#pragma hdrstop

//------------------------------------------------------------------------------
// Include
//------------------------------------------------------------------------------
#include "CLStbObject.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

//------------------------------------------------------------------------------
// CLStbObject
//------------------------------------------------------------------------------
CLStbObject::CLStbObject(void)
{
}
//------------------------------------------------------------------------------
CLStbObject::CLStbObject(const char *name)
{
}
//------------------------------------------------------------------------------
// ~CLStbObject
//------------------------------------------------------------------------------
CLStbObject::~CLStbObject(void)
{
}
//------------------------------------------------------------------------------
// CheckElapsedTime
//------------------------------------------------------------------------------
bool CLStbObject::CheckElapsedTime(struct timeval *timer, int duration, int offset)
{
	struct timeval current, elapsed;
	gettimeofday(&current, NULL);
	if(!timerisset(timer))
	{
		memcpy(timer, &current, sizeof(struct timeval));
		return (false);
	}
	timersub(&current, timer, &elapsed);
	if(TimerToMsec(&elapsed) < duration)
		return (false);
	memcpy(timer, &current, sizeof(struct timeval));
    return (true);
}
//------------------------------------------------------------------------------
// TimerToMsec
//------------------------------------------------------------------------------
int  CLStbObject::TimerToMsec(struct timeval *timer)
{
    return (timer->tv_sec * 1000 + timer->tv_usec / 1000);
}
//------------------------------------------------------------------------------
// gettimeofday
//------------------------------------------------------------------------------
int CLStbObject::gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (NULL != tv)
	{

		// system time�� ���ϱ�
		GetSystemTimeAsFileTime(&ft);

		// unsigned 64 bit�� �����
		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		// 100nano�� 1micro�� ��ȯ�ϱ�
		tmpres /= 10;

		// epoch time���� ��ȯ�ϱ�
		tmpres -= DELTA_EPOCH_IN_MICROSECS;

		// sec�� micorsec���� ���߱�
		tv->tv_sec = (tmpres / 1000000UL);
		tv->tv_usec = (tmpres % 1000000UL);
	}

	// timezone ó��
	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}
