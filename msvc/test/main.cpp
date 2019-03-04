#include "net_manager.h"
#include "event_dispatcher.h"
int main()
{
	core::EventDispatcher dispatcher;
	core::NetManager manager(dispatcher);

	manager.bind("127.0.0.1", 8800);
	manager.listen();

	dispatcher.run();
	return 0;
}