#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include <netcode/netcode.h>

#ifndef PORT
#define PORT 4545
#endif

int main(int argc, char **argv)
{
	struct netcode_state netcode;
	switch (netcode_init(&netcode, PORT))
	{
	case EINVAL:
		printf("Received NULL Argument\n");
		return -1;
		break;
	case EAGAIN:
		printf("Insufficient System Resources\n");
		return -1;
		break;
	case EINVAL:
		printf("Invalid thread attributes\n");
		return -1;
		break;
	case EPERM:
		printf("Invalid Permissions\n");
		return -1;
		break;
	}

	int listening = 1;
	size_t buffer_size = sizeof(char) * 512;
	size_t touched = buffer_size;
	char *buffer = malloc(buffer_size);
	struct timespec time, elapsed;
	time.tv_sec = 2;

	while (listening) {
		if (touched) memset(buffer, '\0', touched);

		if (netcode_read(&netcode, buffer, buffer_size) == EQEMPTY) {
			touched = 0;
			nanosleep(&time, &elapsed);
			continue;
		}

		if (!strncmp(buffer, "!quit!", sizeof(char) * 6)) listening = 0;

		printf("data: %s\n", buffer);
		touched = strlen(buffer);
	}

	free(buffer);

	return netcode_fini(&netcode);
}
