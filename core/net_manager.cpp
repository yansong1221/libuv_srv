#include "net_manager.h"

#include <uv.h>
#include "event_dispatcher.h"
#include <functional>
#include "connection.h"
#include <memory>

namespace core {

	NetManager::NetManager(EventDispatcher& dispatcher)
		:dispatcher_(dispatcher)
	{
		int err = uv_tcp_init(&dispatcher_.getIOService(), &server_);

		if (err != 0)
		{
			throw std::runtime_error(uv_strerror(err));
		}
		server_.data = this;
	}

	NetManager::~NetManager()
	{

	}

	void NetManager::listen(int backlog)
	{

		int err = uv_listen((uv_stream_t*)&server_, backlog, 
			[](uv_stream_t* server, int status)
		{
			if (status < 0) {
				fprintf(stderr, "New connection error %s\n", uv_strerror(status));
				// error!
				return;
			}

			NetManager* manager = static_cast<NetManager*>(server->data);


			uv_tcp_t *client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
			uv_tcp_init(&manager->getIOService(), client);

			if (uv_accept(server, (uv_stream_t*)client) == 0) 
			{
				manager->onAccept(client);
			}
			else 
			{
				uv_close((uv_handle_t*)client, 
					[](uv_handle_t* handle) 
				{
					delete handle;
				});
			}

		});
	}

	void NetManager::bind(const std::string& ip, int port)
	{
		struct sockaddr_in addr;
		int err = uv_ip4_addr(ip.c_str(), port, &addr);

		if (err != 0)
		{
			throw std::runtime_error(uv_strerror(err));
		}

		err = uv_tcp_bind(&server_, (const struct sockaddr*)&addr, 0);

		if (err != 0)
		{
			throw std::runtime_error(uv_strerror(err));
		}
	}


	uv_loop_t& NetManager::getIOService()
	{
		return dispatcher_.getIOService();
	}

	void NetManager::close(std::shared_ptr<Connection> conn)
	{

	}

	void NetManager::onAccept(uv_tcp_t* client)
	{
		auto conn = std::make_shared<Connection>(*this, client);
		connections_["122"] = conn;
	}

}