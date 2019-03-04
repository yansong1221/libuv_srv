#include "app.h"

App::App()
	:netManager_(dispacher_,*this)
{

}

App::~App()
{

}

void App::onNewConnection(const std::string& uniqueID)
{

}

void App::onCloseConnection(const std::string& uniqueID)
{

}

void App::onNetMessage(const std::string& uniqueID, const std::string& data)
{

}

bool App::startUp()
{
	//≥ı ºªØ–≈∫≈
	uv_signal_init(&dispacher_.getIOService(), &closeSig_);
	closeSig_.data = this;

	uv_signal_start(&closeSig_, 
		[](uv_signal_t* handle, int signum)
	{
		App* app = static_cast<App*>(handle->data);
		app->onShutdown();

	}, SIGINT);

	netManager_.bind("127.0.0.1", 8800);
	netManager_.listen();

	dispacher_.addTimer(1000, std::bind(&App::timerTest, this, std::placeholders::_1));

	netManager_.connect("127.0.0.1", 8800, [](const std::string& uniqueID, int status) 
	{
		
	});

	netManager_.connect("127.0.0.1", 8800, [](const std::string& uniqueID, int status)
	{

	});

	return true;
}

void App::onShutdown()
{
	dispacher_.stop();
}

int App::run()
{
	return dispacher_.run();
}
