#include "RCSocketCore.hpp"



WSADATA RCSocketCore::m_wsaData = {};
bool RCSocketCore::m_wsaIsInitialized = false;


RCSocketCore::RCSocketCore():
	m_socket(INVALID_SOCKET)
{
	if(!m_wsaIsInitialized)
	{
		int iResult = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
		if(iResult)
		{
			assert(iResult == SOCKET_ERROR);
			printf("WSAStartup failed with error: %d\n", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		m_wsaIsInitialized = true;

		atexit(wsaDestr);
	}
}


void RCSocketCore::wsaDestr() noexcept
{
	if(m_wsaIsInitialized)
	{
		int iResult = WSACleanup();
		if(iResult)
		{
			assert(iResult == SOCKET_ERROR);
			printf("WSACleanup failed with error: %d\n", WSAGetLastError());
		}

		m_wsaIsInitialized = false;
	}
}


RCSocketCore::~RCSocketCore()
{
	if(m_socket != INVALID_SOCKET)
		closeSocket();
}


RCRet RCSocketCore::listen(const UINT16 port)
{
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	char port_str[MAX_UINT16_LEN + 1] = {};

	sprintf_s(port_str, "%hu", port);

	int iResult = getaddrinfo(NULL, port_str, &hints, &result);
	if(iResult)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		return RCRet::FAILURE;
	}

	m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if(m_socket == INVALID_SOCKET)
	{
		printf("socket failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		return RCRet::FAILURE;
	}

	iResult = bind(m_socket, result->ai_addr, (int)result->ai_addrlen);
	if(iResult)
	{
		assert(iResult == SOCKET_ERROR);

		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closeSocket();
		return RCRet::FAILURE;
	}

	freeaddrinfo(result);

	iResult = ::listen(m_socket, SOMAXCONN);
	if(iResult)
	{
		assert(iResult == SOCKET_ERROR);

		printf("listen failed with error: %d\n", WSAGetLastError());
		closeSocket();
		return RCRet::FAILURE;
	}

	return RCRet::SUCCESS;
}


RCRet RCSocketCore::accept(RCSocketCore *socket)
{
	assert(m_socket != INVALID_SOCKET);
	assert(socket->m_socket == INVALID_SOCKET);

	//printf("waiting for client...\n");
	socket->m_socket = ::accept(m_socket, NULL, NULL);
	if(socket->m_socket == INVALID_SOCKET)
	{
		int error = WSAGetLastError();
		if(error != WSAEINTR)
			printf("accept failed with error: %d\n", error);
		return RCRet::FAILURE;
	}
	printf("client connected\n");

	return RCRet::SUCCESS;
}


RCRet RCSocketCore::connect(const char addr_str[], const UINT16 port)
{
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	char port_str[MAX_UINT16_LEN + 1] = {};

	sprintf_s(port_str, "%hu", port);

	int iResult = getaddrinfo(addr_str, port_str, &hints, &result);
	if(iResult)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		return RCRet::FAILURE;
	}

	assert(m_socket == INVALID_SOCKET);

	// Attempt to connect to an address until one succeeds
	for(ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		m_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if(m_socket == INVALID_SOCKET)
		{
			printf("socket failed with error: %d\n", WSAGetLastError());
			return RCRet::FAILURE;
		}

		// Connect to server.
		iResult = ::connect(m_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if(iResult)
		{
			assert(iResult == SOCKET_ERROR);
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if(m_socket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		return RCRet::FAILURE;
	}

	return RCRet::SUCCESS;
}


RCRet RCSocketCore::send(const char buf[], int length) const
{
	int iResult = ::send(m_socket, buf, length, 0);
	if(iResult == SOCKET_ERROR)
	{
		int error = WSAGetLastError();

		if(error == WSAECONNRESET)
		{
			printf("Connection closed\n");
			return RCRet::CONNECTION_CLOSED;
		}

		DBG_PRINTF("send failed with error: %d\n", error);

		return RCRet::FAILURE;
	}

	return RCRet::SUCCESS;
}


RCRet RCSocketCore::recv(char buf[], int length, int *bytesReceived) const
{
	int iResult = ::recv(m_socket, buf, length, 0);
	if(iResult <= 0)
	{
		int error = WSAGetLastError();

		if(iResult == 0 || error == WSAECONNRESET)
		{
			printf("Connection closed\n");
			return RCRet::CONNECTION_CLOSED;
		}

		DBG_PRINTF("recv failed with error: %d\n", error);

		return RCRet::FAILURE;
	}

	if(bytesReceived)
		*bytesReceived = iResult;

	return RCRet::SUCCESS;
}


void RCSocketCore::shutdown()
{
	DBG_PUTS("RCSocket::shutdown()");
	int iResult = ::shutdown(m_socket, SD_BOTH);
	if(iResult)
	{
		assert(iResult == SOCKET_ERROR);
		int error = WSAGetLastError();
		printf("shutdown failed with error: %d\n", error);
	}
}


void RCSocketCore::shutdownSafe()
{
	if(m_socket != INVALID_SOCKET)
		shutdown();
}


void RCSocketCore::closeSocket()
{
	DBG_PUTS("RCSocket::closeSocket()");
	int iResult = closesocket(m_socket);
	if(iResult)
	{
		assert(iResult == SOCKET_ERROR);
		printf("closesocket failed with error: %d\n", WSAGetLastError());
	}

	m_socket = INVALID_SOCKET;
}


void RCSocketCore::closeSocketSafe()
{
	if(m_socket != INVALID_SOCKET)
		closeSocket();
}