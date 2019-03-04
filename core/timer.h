
#ifndef _CORE_TIMER_H
#define _CORE_TIMER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <functional>
#include <uv.h>
#include <set>

namespace core {

	class EventDispatcher;
	class TimerManager;
	class TimerItem;

	class TimerOperation
	{
	public:
		TimerOperation(TimerItem* timerItem);
		~TimerOperation();

		void cancel();
	private:
		TimerItem* timerItem_;
	};
	typedef std::function<void(TimerOperation)> TimerCallBack;

	class TimerItem
	{
	public:
		TimerItem(TimerManager& timerManager, uint64_t milliseconds, TimerCallBack handler);
		~TimerItem();

		void cancel();
	private:
		uv_timer_t time_;
		TimerManager& timerManager_;
		TimerCallBack handler_;

	};

	class TimerManager 
	{
	public:

		friend class TimerItem;

		TimerManager(EventDispatcher& dispatcher);
		~TimerManager();

		void remove(TimerItem* timerItem);

		TimerOperation add(uint64_t milliseconds, TimerCallBack handler);

		void destroy();
	private:
		EventDispatcher& dispatcher_;
		std::set<TimerItem*> timers_;
	};
}

#endif
