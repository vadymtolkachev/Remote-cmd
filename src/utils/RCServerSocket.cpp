#include "RCServerSocket.hpp"


RCServerSocket::~RCServerSocket()
{
	m_socketCore.shutdownSafe();
}


RCRet RCServerSocket::send(const char buf[], int length) const
{
	return m_socketCore.send(buf, length);
}


RCRet RCServerSocket::recv(char buf[], int length, int *bytesReceived) const
{
	return m_socketCore.recv(buf, length, bytesReceived);
}


void RCServerSocket::close()
{
	m_socketCore.shutdown();
	m_socketCore.closeSocket();
}