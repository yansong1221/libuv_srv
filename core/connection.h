#ifndef _CORE_CONNECTION_H
#define _CORE_CONNECTION_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "typedef.h"

namespace core {

	class NetManager;

	class Connection : public std::enable_shared_from_this<Connection>
	{
	public:
		Connection(NetManager& mgr, uv_tcp_t* client);
		~Connection();

		void close();

		void send(const void* buf, size_t n);

		std::string getUniqueID() const;
	private:
		void allocBuffer(size_t suggested_size, uv_buf_t* buf);
		void onRead(ssize_t nread);
		void onSend(size_t n, int status);
		void send();

		std::string generateUniqueID();
	private:
		NetManager& mgr_;
		uv_tcp_t* client_;
		char readBuffer_[65535];

		std::string sendBuffer_;

		bool sendding_ = false;
		std::string uniqueID_;
	};
}


#endif
