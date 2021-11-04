#pragma once
#include "Recast.h"
#include <cstdint>

#include<Stratega/Utils/warnings.h>

DISABLE_WARNING_PUSH
#if defined(_MSC_VER)
	DISABLE_WARNING_UNSAFE_CONVERSION
#elif defined(__clang__)    
	DISABLE_WARNING_OLD_CAST
	DISABLE_WARNING_SIGN_CONVERSION
	//DISABLE_WARNING_USELESS_CAST
	DISABLE_WARNING_FORMAT_NONLITERAL
#elif defined(__GNUC__)
	DISABLE_WARNING_OLD_CAST
	DISABLE_WARNING_SIGN_CONVERSION
	DISABLE_WARNING_USELESS_CAST
	DISABLE_WARNING_FORMAT_NONLITERAL
#endif

namespace SGA
{
	typedef int64_t TimeVal;

	/// Recast build context.
	class BuildContext : public rcContext
	{
		TimeVal m_startTime[RC_MAX_TIMERS];
		TimeVal m_accTime[RC_MAX_TIMERS];

		static const int MAX_MESSAGES = 1000;
		const char* m_messages[MAX_MESSAGES];
		int m_messageCount;
		static const int TEXT_POOL_SIZE = 8000;
		char m_textPool[TEXT_POOL_SIZE];
		int m_textPoolSize;

	public:
		BuildContext();

		/// Dumps the log to stdout.
		void dumpLog(const char* format, ...);
		/// Returns number of log messages.
		int getLogCount() const;
		/// Returns log message text.
		const char* getLogText(const int i) const;

	protected:
		/// Virtual functions for custom implementations.
		///@{
		virtual void doResetLog();
		virtual void doLog(const rcLogCategory category, const char* msg, const int len);
		virtual void doResetTimers();
		virtual void doStartTimer(const rcTimerLabel label);
		virtual void doStopTimer(const rcTimerLabel label);
		virtual int doGetAccumulatedTime(const rcTimerLabel label) const;
		///@}
	};
}
