#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <netcode/netcode.h>
#include <netcode/udp.h>
#include <datstruc/datstruc.h>

#define PACKET_SIZE 512
#define ACK "ACK"

struct handler_args {
	int sockfd;
	int bytes;
	char *buffer;
	struct netcode_state *state;
	struct sockaddr_storage addr;
};

void *handle_packet(void *data)
{
	struct handler_args *args = (struct handler_args *)data;

	pthread_mutex_lock(&args->state->mutex);

	queue_push(&args->state->cmd_objects, args->buffer);

	pthread_mutex_unlock(&args->state->mutex);

	sendto(args->sockfd, ACK, sizeof(ACK), 0, 
		(struct sockaddr *)&args->addr, sizeof(struct sockaddr_storage));

	free(args);
	pthread_exit(NULL);
}

void *listener_main(void *data)
{
	int sockfd;
	switch (sockfd = udp_listen_socket(((struct netcode_state *)data)->port)) {
	case -1: // bad socket
		pthread_exit((void *) -1);
		break;
	case -2: // bind() failed
		pthread_exit((void *) -2);
		break;
	}

	int err;
	socklen_t bytes;
	socklen_t addr_size;
	pthread_t handler;
	struct handler_args *args;

	for(;;) {
		size_t args_length = sizeof(struct handler_args);
		args = malloc(args_length);
		memset(args, '\0', args_length);

		size_t buffer_length = sizeof(sizeof(char) * PACKET_SIZE);
		args->buffer = malloc(buffer_length);
		memset(args->buffer, '\0', buffer_length);

		addr_size = sizeof(args->addr);
		bytes = recvfrom(sockfd, args->buffer, PACKET_SIZE - 1, 0,
					(struct sockaddr*)&args->addr, &addr_size);
		if (bytes <= 0) {
			free(args->buffer);
			free(args);
			continue;
		}

		args->sockfd = sockfd;
		args->bytes = bytes;
		args->buffer[bytes] = '\0';
		args->state = (struct netcode_state *)data;

		if ((err = pthread_create(&handler, NULL, handle_packet, (void *) args))) {
			free(args->buffer);
			free(args);
			continue;
		}
	}
	// dunno how we got here
	pthread_exit(NULL);
}

// ==================
// exported functions
// ==================

int netcode_init(struct netcode_state *state, int port)
{
	if (!state) { return EINVAL; }
	memset(state, '\0', sizeof(struct netcode_state));

	state->port = port;
	pthread_mutex_init(&state->mutex, NULL);

	return pthread_create(&state->listener, NULL, listener_main, state);
}

int netcode_read(struct netcode_state *state, char *buf, size_t len)
{
	char *msg;
	int msg_len;

	pthread_mutex_lock(&state->mutex);
	msg = (char *)queue_rpop(&state->cmd_objects);
	pthread_mutex_unlock(&state->mutex);
	
	if (!msg) return EQEMPTY;

	msg_len = strlen(msg) + 1;
	msg_len = msg_len > len ? len : msg_len;
	memcpy(buf, msg, msg_len);
	free(msg);

	return 0;
}

int netcode_fini(struct netcode_state *state)
{
	pthread_cancel(state->listener);

	pthread_mutex_lock(&state->mutex);
	queue_delete(&state->cmd_objects);

	pthread_mutex_destroy(&state->mutex);
	pthread_exit(NULL);
	return 0;
}
