#ifndef _CORE_CONNECTION_H
#define _CORE_CONNECTION_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "typedef.h"

namespace core {

	class NetManager;

	typedef struct 
	{
		uv_write_t req;
		uv_buf_t buf;
	} write_req_t;

	class Connection
	{
	public:
		Connection(NetManager& netManager, uv_tcp_t* client,bool connect = false);
		~Connection();

		void close();
		void send(const void* buf, size_t n);

		std::string getUniqueID() const;

	private:
		void allocBuffer(size_t suggested_size, uv_buf_t* buf);
		void onRead(ssize_t nread);
		void onSend(size_t n, int status);
		void send();

		std::string generateUniqueID(bool connect);
	private:
		NetManager& netManager_;
		uv_tcp_t* client_;

		static const int MAX_PACK_SIZE = 65535;
		char tempBuffer_[MAX_PACK_SIZE];

		std::string sendBuffer_,readBuffer_;

		write_req_t writeReq_;

		bool sendding_ = false;
		std::string uniqueID_;

		
	};
}


#endif
