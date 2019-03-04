#include "event_dispatcher.h"

namespace core {

	EventDispatcher::EventDispatcher()
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

	int EventDispatcher::run()
	{
		return uv_run(&io_, uv_run_mode::UV_RUN_DEFAULT);
	}

}