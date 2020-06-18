#include "RCClientSocket.hpp"


RCClientSocket::~RCClientSocket()
{
	m_socketCore.shutdownSafe();
}


RCRet RCClientSocket::send(const char buf[], int length) const
{
	return m_socketCore.send(buf, length);
}


RCRet RCClientSocket::recv(char buf[], int length, int *bytesReceived) const
{
	return m_socketCore.recv(buf, length, bytesReceived);
}


RCRet RCClientSocket::connect(const char addr_str[], const UINT16 port)
{
	return m_socketCore.connect(addr_str, port);
}
