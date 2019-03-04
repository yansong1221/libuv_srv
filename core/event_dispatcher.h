#ifndef _CORE_EVENT_DISPATCHER_H
#define _CORE_EVENT_DISPATCHER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "typedef.h"

namespace core {

	class EventDispatcher 
	{
	public:

		EventDispatcher();
		~EventDispatcher();

	public:
		uv_loop_t& getIOService();

		int run();
	private:
		uv_loop_t io_;
	};
}

#endif
