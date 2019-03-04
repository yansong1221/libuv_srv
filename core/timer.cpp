
#include "timer.h"
#include "event_dispatcher.h"
#include <assert.h>

namespace core {

	TimerManager::TimerManager(EventDispatcher& dispatcher)
		:dispatcher_(dispatcher)
	{

	}

	TimerManager::~TimerManager()
	{

	}

	void TimerManager::remove(TimerItem* timerItem)
	{
		auto it = timers_.find(timerItem);
		if (it == timers_.end())
		{
			assert(false);
			return;
		}

		delete *it;
		timers_.erase(it);
	}

	core::TimerOperation TimerManager::add(uint64_t milliseconds, TimerCallBack handler)
	{
		auto timerItem = new TimerItem(*this, milliseconds, handler);
		timers_.insert(timerItem);

		return TimerOperation(timerItem);
	}

	void TimerManager::destroy()
	{
		for (auto item : timers_)
		{
			delete item;
		}
		timers_.clear();
	}

	TimerItem::TimerItem(TimerManager& timerManager, uint64_t milliseconds, TimerCallBack handler)
		:timerManager_(timerManager),
		handler_(handler)
	{
		uv_timer_init(&timerManager.dispatcher_.getIOService(), &time_);
		time_.data = this;

		uv_timer_start(&time_,
			[](uv_timer_t* handle)
		{
			uv_timer_again(handle);
			TimerItem* item = static_cast<TimerItem*>(handle->data);
			item->handler_(TimerOperation(item));

		}, milliseconds, 1);
	}

	TimerItem::~TimerItem()
	{
		uv_timer_stop(&time_);
	}

	void TimerItem::cancel()
	{
		timerManager_.remove(this);
	}

	TimerOperation::TimerOperation(TimerItem* timerItem)
		:timerItem_(timerItem)
	{

	}

	TimerOperation::~TimerOperation()
	{

	}

	void TimerOperation::cancel()
	{
		timerItem_->cancel();
	}

}
