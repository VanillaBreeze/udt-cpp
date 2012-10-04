#include <set>
#include <udt/udt.h>

namespace UDT {
	class Epoll {
	protected:
		int m_epoll;
		int m_default_events;
		std::set<UDTSOCKET> m_readrds;
		std::set<UDTSOCKET> m_writerds;
	public:
		Epoll(const int default_events = NULL);
		~Epoll();
		int add(const UDTSOCKET usock, const int *events = nullptr);
		int remove(const UDTSOCKET usock, const int *events = nullptr);
		int wait(int64_t msTimeOut = -1);
		std::set<UDTSOCKET>& get_readrds();
		std::set<UDTSOCKET>& get_writerds();
	};
}
