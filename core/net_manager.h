#ifndef _CORE_TCP_SERVER_H
#define _CORE_TCP_SERVER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include <unordered_map>

#include "typedef.h"


namespace core {

	class EventDispatcher;
	class Connection;

	class NetManager 
	{
	public:


		NetManager(EventDispatcher& dispatcher);
		~NetManager();


		void listen(int backlog = 128);
		void bind(const std::string& ip, int port);

		uv_loop_t& getIOService();

		void close(std::shared_ptr<Connection> conn);
	private:

		void onAccept(uv_tcp_t *client);

	private:

		EventDispatcher& dispatcher_;
		uv_tcp_t server_;

		std::unordered_map<std::string, std::shared_ptr<Connection>> connections_;
	};
}

#endif
