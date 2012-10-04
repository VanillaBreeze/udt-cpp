#include "epoll.h"

#ifndef dlog
    #define dlog(...)
#endif

#define DEBUG_EPOLL 0

UDT::Epoll::Epoll(const int events) {
	dlog(1, "        create epoll with events %x", events);
	this->m_epoll = UDT::epoll_create();
	this->m_default_events = events;
}

UDT::Epoll::~Epoll() {
	dlog(DEBUG_EPOLL, "        release epoll");
	UDT::epoll_release(this->m_epoll);
}

int UDT::Epoll::add(const UDTSOCKET usock, const int *events) {
	const int *the_events = events ? events : &this->m_default_events;
    dlog(DEBUG_EPOLL, "        trying to add epoll sock %x with events %x", usock, *the_events);
    int res = UDT::epoll_add_usock(this->m_epoll, usock, the_events);
    return res;
}

int UDT::Epoll::remove(const UDTSOCKET usock, const int *events) {
	#if DEBUG
	const int *the_events = events ? events : &this->m_default_events;
	dlog(DEBUG_EPOLL, "        trying to remove epoll sock %x with events %x", usock, *the_events);
	#endif
	return UDT::epoll_remove_usock(this->m_epoll, usock);
}

int UDT::Epoll::wait(int64_t ms_timeout) {
	dlog(DEBUG_EPOLL, "        trying to wait with timeout %ldms", ms_timeout);
	return UDT::epoll_wait(this->m_epoll, &this->m_readrds, &this->m_writerds, ms_timeout);
}

std::set<UDTSOCKET> &UDT::Epoll::get_readrds() {
	return this->m_readrds;
}

std::set<UDTSOCKET>& UDT::Epoll::get_writerds() {
	return this->m_writerds;
}
