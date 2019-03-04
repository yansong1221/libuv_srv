#include "connection.h"
#include "net_manager.h"

#include <sha.h>
#include <hex.h>

namespace core {

	typedef struct {
		uv_write_t req;
		uv_buf_t buf;
	} write_req_t;

	Connection::Connection(NetManager& mgr, uv_tcp_t* client)
		:mgr_(mgr),
		client_(client)
	{
		client_->data = this;

		uniqueID_ = generateUniqueID();

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
		buf->base = readBuffer_;
		buf->len = sizeof(readBuffer_);
	}

	void Connection::onRead(ssize_t nread)
	{
		if (nread < 0) {
			if (nread != UV_EOF)
				fprintf(stderr, "Read error %s\n", uv_err_name(nread));

		}
		else 
		{
			send(readBuffer_, nread);
		}	
	}

	void Connection::send(const void* buf, size_t n)
	{	
		sendBuffer_.append((const char*)buf, n);
		send();
		close();
	}

	void Connection::send()
	{
		if (sendBuffer_.empty()) return;
		if (sendding_) return;

		sendding_ = true;

		write_req_t* req = (write_req_t*)malloc(sizeof(write_req_t));
		req->req.data = this;

		req->buf = uv_buf_init(&sendBuffer_[0],sendBuffer_.length());

		uv_write((uv_write_t*)req, (uv_stream_t*)client_, &req->buf, 1,
			[](uv_write_t* req, int status)
		{
			write_req_t* wr = (write_req_t*)req;
			Connection* conn = static_cast<Connection*>(req->data);
			conn->onSend(wr->buf.len,status);

			free(wr);
		});
	}

	void Connection::onSend(size_t n,int status)
	{
		if (status) 
		{
			fprintf(stderr, "Write error %s\n", uv_strerror(status));
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

	std::string Connection::generateUniqueID()
	{
		std::string hashText;

		sockaddr_in addr;
		int len = sizeof(addr);

		int err = uv_tcp_getpeername(client_, (struct sockaddr*)&addr, &len);

		if (err != 0)
		{
			throw std::runtime_error(uv_strerror(err));
		}

		char buffer[INET_ADDRSTRLEN];
		err = uv_ip4_name(&addr, buffer, sizeof(buffer));

		if (err != 0)
		{
			throw std::runtime_error(uv_strerror(err));
		}

		// "127.0.0.1:8800,time"

		hashText.append(buffer);
		hashText.append(":");
		hashText.append(std::to_string(ntohs(addr.sin_port)));
		hashText.append(":");
		hashText.append(std::to_string(time(nullptr)));

		

		CryptoPP::SHA1 sha1;
		CryptoPP::HashFilter hashFilter(sha1);

		std::string hash;
		hashFilter.Attach(new CryptoPP::HexEncoder(new CryptoPP::StringSink(hash), false));
		hashFilter.Put(reinterpret_cast<const unsigned char*>(hashText.data()), hashText.size());

		hashFilter.MessageEnd();

		return hash;
	}

}