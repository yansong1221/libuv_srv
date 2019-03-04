#include "event_dispatcher.h"

namespace core {

	EventDispatcher::EventDispatcher()
		:timerManager_(*this)
	{
		uv_loop_init(&io_);
	}

	EventDispatcher::~EventDispatcher()
	{

	}

	uv_loop_t& EventDispatcher::getIOService()
	{
		return io_;
	}

	core::TimerOperation EventDispatcher::addTimer(uint64_t milliseconds, TimerCallBack handler)
	{
		return timerManager_.add(milliseconds, handler);
	}

	int EventDispatcher::run()
	{
		return uv_run(&io_, uv_run_mode::UV_RUN_DEFAULT);
	}

	void EventDispatcher::stop()
	{
		uv_stop(&io_);
	}

}