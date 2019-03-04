#include "connection.h"
#include "net_manager.h"
#include <fmt/core.h>
#include <time.h>
#include "event_dispatcher.h"

namespace core {


	Connection::Connection(NetManager& netManager, uv_tcp_t* client,bool connect)
		:netManager_(netManager),
		client_(client)
	{
		client_->data = this;
		uniqueID_ = generateUniqueID(connect);

		memset(&writeReq_, 0, sizeof(writeReq_));
		writeReq_.req.data = this;

		uv_read_start((uv_stream_t*)client_, 
			[](uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) 
		{
			Connection* conn = static_cast<Connection*>(handle->data);
			conn->allocBuffer(suggested_size, buf);
		}, 
			[](uv_stream_t* stream,ssize_t nread,const uv_buf_t* buf)
		{
			Connection* conn = static_cast<Connection*>(stream->data);
			conn->onRead(nread);
		});
		
	}

	Connection::~Connection()
	{
		close();
	}

	void Connection::close()
	{
		if (client_)
		{
			if (::uv_is_active((uv_handle_t*)client_))
			{
				::uv_read_stop((uv_stream_t*)client_);
			}
			if (::uv_is_closing((uv_handle_t*)client_) == 0)
			{
				//libuv 在loop轮询中会检测关闭句柄，delete会导致程序异常退出。
				::uv_close((uv_handle_t*)client_, 
					[](uv_handle_t* handle)
				{
					delete handle;
					handle = nullptr;
				});				
			}			
		}	
	}

	void Connection::allocBuffer(size_t suggested_size, uv_buf_t* buf)
	{
		buf->base = tempBuffer_;
		buf->len = sizeof(tempBuffer_);
	}

	void Connection::onRead(ssize_t nread)
	{
		if (nread < 0) {
			if (nread != UV_EOF)
				fprintf(stderr, "Read error %s\n", uv_err_name(nread));
			netManager_.closeConn(uniqueID_);
		}
		else 
		{
			readBuffer_.append(tempBuffer_, nread);

			while (readBuffer_.size() > sizeof(int32_t))
			{
				int32_t packSize = *(reinterpret_cast<int32_t*>(&readBuffer_[0]));
				packSize = ntohl(packSize);

				if (packSize > MAX_PACK_SIZE)
				{
					netManager_.closeConn(uniqueID_);
					return;
				}

				if(readBuffer_.size() < (size_t)packSize) break;
				netManager_.netInterface_.onNetMessage(uniqueID_,std::string(&readBuffer_[sizeof(int32_t)], packSize - sizeof(int32_t)));

				readBuffer_.erase(0, packSize);
			}			
		}	
	}

	void Connection::send(const void* buf, size_t n)
	{	
		sendBuffer_.append((const char*)buf, n);
		send();
	}

	void Connection::send()
	{
		if (sendBuffer_.empty()) return;
		if (sendding_) return;

		sendding_ = true;
		writeReq_.buf = uv_buf_init(&sendBuffer_[0],sendBuffer_.length());

		uv_write((uv_write_t*)&writeReq_, (uv_stream_t*)client_, &writeReq_.buf, 1,
			[](uv_write_t* req, int status)
		{
			write_req_t* wr = (write_req_t*)req;
			Connection* conn = static_cast<Connection*>(req->data);
			conn->onSend(wr->buf.len,status);
		});
	}

	void Connection::onSend(size_t n,int status)
	{
		if (status) 
		{
			fprintf(stderr, "Write error %s\n", uv_strerror(status));
			netManager_.closeConn(uniqueID_);
		}
		else 
		{
			sendding_ = false;
			sendBuffer_.erase(0, n);
			send();
		}
	}

	std::string Connection::getUniqueID() const
	{
		return uniqueID_;
	}

	std::string Connection::generateUniqueID(bool connect)
	{
		sockaddr_in addr[2];
		memset(&addr, 0, sizeof(addr));
		int len = sizeof(addr);

		int err;
		if (!connect)
			err = uv_tcp_getpeername(client_, (struct sockaddr*)&addr, &len);
		else
			err = uv_tcp_getsockname(client_, (struct sockaddr*)&addr, &len);

		if (err != 0)
		{
			throw std::runtime_error(uv_strerror(err));
		}

		char buffer[INET_ADDRSTRLEN];
		err = uv_ip4_name(&addr[0], buffer, sizeof(buffer));

		if (err != 0)
		{
			throw std::runtime_error(uv_strerror(err));
		}

		
		// "127.0.0.1:8800:time"
		return fmt::format("{}:{}:{}", buffer, ntohs(addr[0].sin_port), uv_now(&netManager_.dispatcher_.getIOService()));
	}

}