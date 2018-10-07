#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <netcode.h>

#ifndef PORT
#define PORT 4545
#endif

int main(int argc, char **argv)
{
	struct netcode_state netcode;
	netcode_init(&netcode, PORT);

	struct timespec time, elapsed;
	time.tv_sec = 2;
	size_t buffer_size = sizeof(char) * 512;
	size_t touched = buffer_size;
	char *buffer = malloc(buffer_size);

	while (1) {
		if (touched) memset(buffer, '\0', touched);
		int res = netcode_read(&netcode, buffer, buffer_size);
		if (res == EQEMPTY) {
			touched = 0;
			res = nanosleep(&time, &elapsed);
			continue;
		}
		printf("data: %s\n", buffer);
		touched = strlen(buffer);
	}

	free(buffer);

	return netcode_fini(&netcode);
}
