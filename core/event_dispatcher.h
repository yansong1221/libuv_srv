#ifndef _CORE_EVENT_DISPATCHER_H
#define _CORE_EVENT_DISPATCHER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "typedef.h"
#include "timer.h"

namespace core {

	class EventDispatcher 
	{
	public:

		EventDispatcher();
		~EventDispatcher();

	public:
		uv_loop_t& getIOService();

		TimerOperation addTimer(uint64_t milliseconds, TimerCallBack handler);

		int run();
		void stop();
	private:
		uv_loop_t io_;
		TimerManager timerManager_;
	};
}

#endif
