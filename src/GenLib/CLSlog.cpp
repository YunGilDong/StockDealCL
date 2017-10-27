//---------------------------------------------------------------------------

#pragma hdrstop

#include "CLSlog.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
// Constructor
///---------------------------------------------------------------------------
__fastcall CLSlog::CLSlog(void)
{
}
//---------------------------------------------------------------------------
__fastcall CLSlog::CLSlog(const char *name, const char *pDir)
{
	if(pDir != NULL)
		sprintf(m_dir, "%s", pDir);

	sprintf(m_name, "%s", name);
}
//---------------------------------------------------------------------------
// Destroy
//---------------------------------------------------------------------------
__fastcall CLSlog::~CLSlog(void)
{

}
//---------------------------------------------------------------------------
// SetLogdir
//---------------------------------------------------------------------------
void __fastcall CLSlog::SetLogdir(const char *pDir)
{
	sprintf(m_dir, "%s", pDir);
}
//------------------------------------------------------------------------------
// FLdump
//------------------------------------------------------------------------------
void CLSlog::FLdump(const char *stamp, char *data, int length, int maxLength)
{
	int		maxCount, pos;

	// 메시지 표시
	sprintf(m_buffer, "%s  %s [%d]\n", GetTimeString(&m_tod), stamp, length);
	Logging();
	maxCount = min(length, maxLength);
	pos = 0;
	for (int idx = 0; idx < maxCount; idx++, data++)
	{
		if ((idx % DEFAULT_DUMP_LEN) == 0)
		{
			if (idx != 0)
			{
				sprintf(&m_buffer[pos], "\n");
				Logging();
			}
			pos = sprintf(m_buffer, "    ");
		}
		pos += sprintf(&m_buffer[pos], " %02X", (BYTE)*data);
	}
	if (pos)
	{
		sprintf(&m_buffer[pos], "\n");
		Logging();
	}
}
//---------------------------------------------------------------------------
// Logging
//---------------------------------------------------------------------------
void __fastcall CLSlog::Logging(void)
{
	int length;
	char filename[FILENAME_LEN];
	FILE *fp;

	//메시지 확인
	if((length = strlen(m_buffer))<=0)
		return ;
	sprintf(filename, "%s/%s_%04d%02d%02d.log", m_dir, m_name, m_tod.tm_year+1900, m_tod.tm_mon+1, m_tod.tm_mday);
	if((fp = fopen(filename, "a+")) == NULL)
	{
		return ;
	}
	fwrite(m_buffer, 1, length, fp);
    fclose(fp);
}
//---------------------------------------------------------------------------
// Write
//---------------------------------------------------------------------------
void CLSlog::Write(const char *format, ...)
{
	char buffer[BUFFER_LEN];
	va_list argList;

	va_start(argList, format);
	vsprintf(buffer, format, argList);
    sprintf(m_buffer, "%s  %s\n", GetTimeString(&m_tod), buffer);
	va_end(argList);
    Logging();
}
//---------------------------------------------------------------------------
// GetTimeString
//---------------------------------------------------------------------------
char * __fastcall CLSlog::GetTimeString(void)
{
	struct tm	*tod;
	struct timeval	timeVal;

	gettimeofday(&timeVal, NULL);
	tod = localtime(&timeVal.tv_sec);
	sprintf(m_timeStamp, "%04d/%02d/%02d  %02d:%02d:%02d.%03d [%s]",
		tod->tm_year+1900, tod->tm_mon+1, tod->tm_mday, tod->tm_hour,
		tod->tm_min, tod->tm_sec, timeVal.tv_usec/1000, m_name);
	return(m_timeStamp);
}
//---------------------------------------------------------------------------
char * __fastcall CLSlog::GetTimeString(struct tm *pTod)
{
	struct timeval	timeVal;

	gettimeofday(&timeVal, NULL);
	//pTod=localtime(&timeVal.tv_sec);
	localtime_s(&timeVal.tv_sec, pTod);     //pTod에 시 분 초 로
	sprintf(m_timeStamp, "%04d/%02d/%02d  %02d:%02d:%02d.%03d [%s]",
		pTod->tm_year+1900, pTod->tm_mon+1, pTod->tm_mday, pTod->tm_hour,
		pTod->tm_min, pTod->tm_sec, timeVal.tv_usec/1000, m_name);
	return(m_timeStamp);
}
//---------------------------------------------------------------------------
// gettimeofday
//---------------------------------------------------------------------------
int __fastcall CLSlog::gettimeofday(struct timeval *tv, struct timezone2 *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (NULL != tv)
	{

		// system time을 구하기
		GetSystemTimeAsFileTime(&ft);

		// unsigned 64 bit로 만들기
		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		// 100nano를 1micro로 변환하기
		tmpres /= 10;

		// epoch time으로 변환하기
		tmpres -= DELTA_EPOCH_IN_MICROSECS;

		// sec와 micorsec으로 맞추기
		tv->tv_sec = (tmpres / 1000000UL);
		tv->tv_usec = (tmpres % 1000000UL);
	}

	// timezone 처리
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
