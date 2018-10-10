#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <netcode/udp.h>

int udp_listen_socket(int port)
{
	int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		return -1;
	}
	struct sockaddr_in addr;
	memset(&addr, '\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr*) &addr, sizeof(addr))) {
		return -2;
	}

	return sockfd;
}
