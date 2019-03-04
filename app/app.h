#pragma once

#include "net_manager.h"
#include "event_dispatcher.h"

class App : public core::NetInterface
{
public:
	App();
	~App();
public:
	virtual void onNewConnection(const std::string& uniqueID);
	virtual void onCloseConnection(const std::string& uniqueID);
	virtual void onNetMessage(const std::string& uniqueID, const std::string& data);

	virtual bool startUp();

	virtual void onShutdown();

	void timerTest(core::TimerOperation op)
	{
		op.cancel();
	}
	int run();
private:
	core::EventDispatcher dispacher_;
	core::NetManager netManager_;
	uv_signal_t closeSig_;
};