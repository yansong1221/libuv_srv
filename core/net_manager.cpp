#include "net_manager.h"

#include <uv.h>
#include "event_dispatcher.h"
#include <functional>
#include "connection.h"
#include <memory>
#include <assert.h>
#include "interface.h"

namespace core {

	struct ConnectInfo
	{
		uv_connect_t req;
		NetManager::ConnectCallBack handler_;
	};
	NetManager::NetManager(EventDispatcher& dispatcher, NetInterface& netInterface)
		:dispatcher_(dispatcher),
		netInterface_(netInterface)
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


	void NetManager::connect(const std::string& ip, int port, ConnectCallBack handler)
	{
		ConnectInfo* connInfo = new ConnectInfo;
		connInfo->req.data = this;
		connInfo->handler_ = handler;

		uv_tcp_t* conn = new uv_tcp_t;
		connInfo->req.handle = (uv_stream_t*)conn;

		uv_tcp_init(&dispatcher_.getIOService(), conn);

		sockaddr_in addr;
		uv_ip4_addr(ip.c_str(), port, &addr);

		uv_tcp_connect((uv_connect_t*)connInfo, conn, (const sockaddr*)&addr,
			[](uv_connect_t* req, int status)
		{
			ConnectInfo* connInfo = (ConnectInfo*)req;
			std::string uniqueID;

			if (status == 0)
			{
				NetManager* mgr = (NetManager*)connInfo->req.data;
				uv_tcp_t* client = (uv_tcp_t*)connInfo->req.handle;

				auto conn = std::make_shared<Connection>(*mgr, client,true);

				if (mgr->findConn(conn->getUniqueID()) != nullptr)
				{
					assert(false);
				}
				else
				{
					uniqueID = conn->getUniqueID();
					mgr->connections_[uniqueID] = conn;
				}
				
			}
			else
			{	
				delete (uv_tcp_t*)connInfo->req.handle;		
			}
			connInfo->handler_(uniqueID, status);
			delete connInfo;
		});

	}

	uv_loop_t& NetManager::getIOService()
	{
		return dispatcher_.getIOService();
	}


	std::shared_ptr<core::Connection> NetManager::findConn(const std::string& uniqueID)
	{
		auto it = connections_.find(uniqueID);

		if (it == connections_.end()) return nullptr;

		return it->second;
	}

	void NetManager::closeConn(std::shared_ptr<Connection> conn)
	{
		return closeConn(conn->getUniqueID());
	}

	void NetManager::closeConn(const std::string& uniqueID)
	{
		auto it = connections_.find(uniqueID);
		if (it == connections_.end()) return;

		netInterface_.onCloseConnection(uniqueID);
		connections_.erase(it);	
	}

	void NetManager::onAccept(uv_tcp_t* client)
	{
		auto conn = std::make_shared<Connection>(*this, client);

		auto it = connections_.find(conn->getUniqueID());

		if (it != connections_.end())
		{
			assert(false);
			return;
		}
		connections_[conn->getUniqueID()] = conn;

		netInterface_.onNewConnection(conn->getUniqueID());
	}

}