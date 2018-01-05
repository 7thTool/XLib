#ifndef _H_TIME_H_
#define _H_TIME_H_

#include "XLib.h"
#ifdef _AFX
typedef CTime Time;
typedef CTimeSpan TimeSpan;
#else
#include <time.h>

#ifndef __oledb_h__
struct tagDBTIMESTAMP;
typedef tagDBTIMESTAMP DBTIMESTAMP;
#endif

#if _MSC_VER<=1200

/////////////////////////////////////////////////////////////////////////////
// TimeSpan and Time

class XLIB_API TimeSpan
{
public:

// Constructors
	TimeSpan();
	TimeSpan(time_t time);
	TimeSpan(LONG lDays, int nHours, int nMins, int nSecs);

	TimeSpan(const TimeSpan& timeSpanSrc);
	const TimeSpan& operator=(const TimeSpan& timeSpanSrc);

// Attributes
	// extract parts
	LONG GetDays() const;   // total # of days
	LONG GetTotalHours() const;
	int GetHours() const;
	LONG GetTotalMinutes() const;
	int GetMinutes() const;
	LONG GetTotalSeconds() const;
	int GetSeconds() const;

// Operations
	// time math
	TimeSpan operator-(TimeSpan timeSpan) const;
	TimeSpan operator+(TimeSpan timeSpan) const;
	const TimeSpan& operator+=(TimeSpan timeSpan);
	const TimeSpan& operator-=(TimeSpan timeSpan);
	BOOL operator==(TimeSpan timeSpan) const;
	BOOL operator!=(TimeSpan timeSpan) const;
	BOOL operator<(TimeSpan timeSpan) const;
	BOOL operator>(TimeSpan timeSpan) const;
	BOOL operator<=(TimeSpan timeSpan) const;
	BOOL operator>=(TimeSpan timeSpan) const;

private:
	time_t m_timeSpan;
	friend class Time;
};

class XLIB_API Time
{
public:

// Constructors
	static time_t PASCAL GetCurrentTime();

	Time();
	Time(time_t time);
	Time(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,
		int nDST = -1);
	Time(WORD wDosDate, WORD wDosTime, int nDST = -1);
	Time(const Time& timeSrc);

	Time(const SYSTEMTIME& sysTime, int nDST = -1);
	Time(const FILETIME& fileTime, int nDST = -1);
	const Time& operator=(const Time& timeSrc);
	const Time& operator=(time_t t);

// Attributes
	struct tm* GetGmtTm(struct tm* ptm = NULL) const;
	struct tm* GetLocalTm(struct tm* ptm = NULL) const;
	BOOL GetAsSystemTime(SYSTEMTIME& timeDest) const;

	time_t GetTime() const;
	int GetYear() const;
	int GetMonth() const;       // month of year (1 = Jan)
	int GetDay() const;         // day of month
	int GetHour() const;
	int GetMinute() const;
	int GetSecond() const;
	int GetDayOfWeek() const;   // 1=Sun, 2=Mon, ..., 7=Sat

// Operations
	// time math
	TimeSpan operator-(Time time) const;
	Time operator-(TimeSpan timeSpan) const;
	Time operator+(TimeSpan timeSpan) const;
	const Time& operator+=(TimeSpan timeSpan);
	const Time& operator-=(TimeSpan timeSpan);
	BOOL operator==(Time time) const;
	BOOL operator!=(Time time) const;
	BOOL operator<(Time time) const;
	BOOL operator>(Time time) const;
	BOOL operator<=(Time time) const;
	BOOL operator>=(Time time) const;

private:
	time_t m_time;
};

#else

/////////////////////////////////////////////////////////////////////////////
// TimeSpan and Time

class XLIB_API TimeSpan
{
public:
	TimeSpan();
	TimeSpan( __time64_t time );
	TimeSpan( LONG lDays, int nHours, int nMins, int nSecs );

	LONGLONG GetDays() const;
	LONGLONG GetTotalHours() const;
	LONG GetHours() const;
	LONGLONG GetTotalMinutes() const;
	LONG GetMinutes() const;
	LONGLONG GetTotalSeconds() const;
	LONG GetSeconds() const;

	__time64_t GetTimeSpan() const;

	TimeSpan operator+( TimeSpan span ) const;
	TimeSpan operator-( TimeSpan span ) const;
	TimeSpan& operator+=( TimeSpan span );
	TimeSpan& operator-=( TimeSpan span );
	bool operator==( TimeSpan span ) const;
	bool operator!=( TimeSpan span ) const;
	bool operator<( TimeSpan span ) const;
	bool operator>( TimeSpan span ) const;
	bool operator<=( TimeSpan span ) const;
	bool operator>=( TimeSpan span ) const;

protected:
	__time64_t m_timeSpan;
};

/////////////////////////////////////////////////////////////////////////////////////

class XLIB_API Time
{
public:
	static __time64_t GetCurrentTime();
	static BOOL IsValidFILETIME(const FILETIME& ft);

public:
	Time();
	Time( __time64_t time );
	Time( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,
		int nDST = -1 );
	Time( WORD wDosDate, WORD wDosTime, int nDST = -1 );
	Time( const SYSTEMTIME& st, int nDST = -1 );
	Time( const FILETIME& ft, int nDST = -1 );
#ifdef __oledb_h__
	Time( const DBTIMESTAMP& dbts, int nDST = -1 );
#endif

	Time& operator=( __time64_t time );

	Time& operator+=( TimeSpan span );
	Time& operator-=( TimeSpan span );

	TimeSpan operator-( Time time ) const;
	Time operator-( TimeSpan span ) const;
	Time operator+( TimeSpan span ) const;

	bool operator==( Time time ) const;
	bool operator!=( Time time ) const;
	bool operator<( Time time ) const;
	bool operator>( Time time ) const;
	bool operator<=( Time time ) const;
	bool operator>=( Time time ) const;

	struct tm* GetGmtTm( struct tm* ptm ) const;
	struct tm* GetLocalTm( struct tm* ptm ) const;

	bool GetAsSystemTime( SYSTEMTIME& st ) const;
	bool GetAsDBTIMESTAMP( DBTIMESTAMP& dbts ) const;

	__time64_t GetTime() const;

	int GetYear() const;
	int GetMonth() const;
	int GetDay() const;
	int GetHour() const;
	int GetMinute() const;
	int GetSecond() const;
	int GetDayOfWeek() const;

protected:
	__time64_t m_time;
};

#endif//
#endif//

class XLIB_API TickSpan
{
public:
	static ULONG GetTickCount();
protected:
	ULONG m_TickCount;
public:
	TickSpan(void);
	virtual ~TickSpan();
public:
	void Reset();
	ULONG GetTickSpan(void);
};
#if WINVER >= 0X0600
class XLIB_API TickSpan64
{
public:
	static unsigned __int64 GetTickCount();
protected:
	unsigned __int64 m_TickCount;
public:
	TickSpan64(void);
	virtual ~TickSpan64();
public:
	void Reset(void);
	unsigned __int64 GetTickSpan(void);
};
#endif//

#endif//_H_TIME_H_