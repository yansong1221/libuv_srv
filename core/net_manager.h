#ifndef _CORE_TCP_SERVER_H
#define _CORE_TCP_SERVER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>
#include <unordered_map>

#include "typedef.h"
#include "interface.h"
#include <functional>

namespace core {

	class EventDispatcher;
	class Connection;
	class NetInterface;

	class NetManager 
	{
	public:

		friend class Connection;

		typedef std::function<void(const std::string&, int)> ConnectCallBack;

		NetManager(EventDispatcher& dispatcher,NetInterface& netInterface);
		~NetManager();


		void listen(int backlog = 128);
		void bind(const std::string& ip, int port);

		void connect(const std::string& ip, int port, ConnectCallBack handler);

		uv_loop_t& getIOService();

		std::shared_ptr<Connection> findConn(const std::string& uniqueID);

		void closeConn(std::shared_ptr<Connection> conn);
		void closeConn(const std::string& uniqueID);
	private:

		void onAccept(uv_tcp_t *client);

	private:

		NetInterface& netInterface_;
		EventDispatcher& dispatcher_;
		uv_tcp_t server_;

		std::unordered_map<std::string, std::shared_ptr<Connection>> connections_;

	};
}

#endif
