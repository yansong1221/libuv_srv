
#ifndef _CORE_INTERFACE_H
#define _CORE_INTERFACE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <string>

namespace core {
	class NetInterface
	{
	public:
		virtual void onNewConnection(const std::string& uniqueID) = 0;
		virtual void onCloseConnection(const std::string& uniqueID) = 0;
		virtual void onNetMessage(const std::string& uniqueID, const std::string& data) = 0;
	};
}

#endif
