#ifndef netcode_h_
#define netcode_h_

#include <pthread.h>
#include <stddef.h>
#include <datstruc/datstruc.h>

#define EQEMPTY -1

struct netcode_state {
	pthread_t listener;
	pthread_mutex_t mutex;
	struct queue_t cmd_objects;
	int port;
};

int netcode_init(struct netcode_state *state, int port);
int netcode_read(struct netcode_state *state, char *buf, size_t len);
int netcode_fini(struct netcode_state *state);

#endif
