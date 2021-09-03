#include <cstdio>
#include <cstring>
#include <cstdarg>

#include <Stratega/Representation/BuildContext.h>

namespace SGA
{

#if defined(WIN32)

	// Win32
#include <windows.h>

	TimeVal getPerfTime()
	{
		__int64 count;
		QueryPerformanceCounter((LARGE_INTEGER*)&count);
		return count;
	}

	int getPerfTimeUsec(const TimeVal duration)
	{
		static __int64 freq = 0;
		if (freq == 0)
			QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		return (int)(duration * 1000000 / freq);
	}

#else

	// Linux, BSD, OSX

#include <sys/time.h>

	TimeVal getPerfTime()
	{
		timeval now;
		gettimeofday(&now, 0);
		return (TimeVal)now.tv_sec * 1000000L + (TimeVal)now.tv_usec;
	}

	int getPerfTimeUsec(const TimeVal duration)
	{
		return (int)duration;
	}

#endif

	BuildContext::BuildContext() :
		m_messageCount(0),
		m_textPoolSize(0)
	{
		memset(m_messages, 0, sizeof(char*) * MAX_MESSAGES);

		resetTimers();
	}

	// Virtual functions for custom implementations.
	void BuildContext::doResetLog()
	{
		m_messageCount = 0;
		m_textPoolSize = 0;
	}

	void BuildContext::doLog(const rcLogCategory category, const char* msg, const int len)
	{
		if (!len) return;
		if (m_messageCount >= MAX_MESSAGES)
			return;
		char* dst = &m_textPool[m_textPoolSize];
		int n = TEXT_POOL_SIZE - m_textPoolSize;
		if (n < 2)
			return;
		char* cat = dst;
		char* text = dst + 1;
		const int maxtext = n - 1;
		// Store category
		*cat = (char)category;
		// Store message
		const int count = rcMin(len + 1, maxtext);
		memcpy(text, msg, count);
		text[count - 1] = '\0';
		m_textPoolSize += 1 + count;
		m_messages[m_messageCount++] = dst;
	}

	void BuildContext::doResetTimers()
	{
		for (int i = 0; i < RC_MAX_TIMERS; ++i)
			m_accTime[i] = -1;
	}

	void BuildContext::doStartTimer(const rcTimerLabel label)
	{
		m_startTime[label] = getPerfTime();
	}

	void BuildContext::doStopTimer(const rcTimerLabel label)
	{
		const TimeVal endTime = getPerfTime();
		const TimeVal deltaTime = endTime - m_startTime[label];
		if (m_accTime[label] == -1)
			m_accTime[label] = deltaTime;
		else
			m_accTime[label] += deltaTime;
	}

	int BuildContext::doGetAccumulatedTime(const rcTimerLabel label) const
	{
		return getPerfTimeUsec(m_accTime[label]);
	}

	void BuildContext::dumpLog(const char* format, ...)
	{
		// Print header.
		va_list ap;
		va_start(ap, format);
		vprintf(format, ap);
		va_end(ap);
		printf("\n");

		// Print messages
		const int TAB_STOPS[4] = { 28, 36, 44, 52 };
		for (int i = 0; i < m_messageCount; ++i)
		{
			const char* msg = m_messages[i] + 1;
			int n = 0;
			while (*msg)
			{
				if (*msg == '\t')
				{
					int count = 1;
					for (int j = 0; j < 4; ++j)
					{
						if (n < TAB_STOPS[j])
						{
							count = TAB_STOPS[j] - n;
							break;
						}
					}
					while (--count)
					{
						putchar(' ');
						n++;
					}
				}
				else
				{
					putchar(*msg);
					n++;
				}
				msg++;
			}
			putchar('\n');
		}
	}

	int BuildContext::getLogCount() const
	{
		return m_messageCount;
	}

	const char* BuildContext::getLogText(const int i) const
	{
		return m_messages[i] + 1;
	}
}
