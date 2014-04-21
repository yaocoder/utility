int connect_nonb( int sockfd, struct sockaddr* saptr, socklen_t salen, int nsec )
{
	int ret, n, error = 0;
	socklen_t len;
	FD_SET rset, wset;
	struct timeval tval;
	unsigned long ul=1;  
	unsigned long ul1=0;

	ret = ioctlsocket(sockfd, FIONBIO, (unsigned long *)&ul);	//设置成非阻塞模式
	if (SOCKET_ERROR == ret)
	{
		LOG4CXX_ERROR(g_logger, "connect_nonb:ioctlsocket failed. errorcode = " << WSAGetLastError());
		return -1;
	}

	if ((n = ::connect(sockfd, saptr, salen) < 0))
	{
		error = WSAGetLastError();
		if (WSAEWOULDBLOCK != error)
		{
			LOG4CXX_ERROR(g_logger, "connect_nonb:connect failed. errorcode = " << error);
			return -1;
		}
	}

	if(0 == n)
		goto done;

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	wset = rset;
	tval.tv_sec = nsec;
	tval.tv_usec = 0;

	if ((n = select(sockfd+1, &rset, &wset, NULL, nsec?&tval:NULL)) == 0)
	{
		::closesocket(sockfd);
		error = WSAETIMEDOUT;
		return -2;
	}

	if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset))
	{
		len = sizeof(error);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)&error, &len) < 0)
		{
			LOG4CXX_ERROR(g_logger, "connect_nonb:getsockopt failed. errorcode = " << WSAGetLastError());
			return -1;
		}
	}
	
done:
	ret = ioctlsocket(sockfd, FIONBIO, (unsigned long*)&ul1);
	if (SOCKET_ERROR == ret)
	{
		LOG4CXX_ERROR(g_logger, "connect_nonb:ioctlsocket1 failed. errorcode = " << WSAGetLastError());
		return -1;
	}

	return 0;
}