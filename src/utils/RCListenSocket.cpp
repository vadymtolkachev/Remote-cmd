#include "RCListenSocket.hpp"


RCRet RCListenSocket::listen(const UINT16 port)
{
	return m_socketCore.listen(port);
}


RCRet RCListenSocket::accept(RCServerSocket *serverSocket)
{
	return m_socketCore.accept(&serverSocket->m_socketCore);
}


void RCListenSocket::close()
{
	m_socketCore.closeSocketSafe();
}