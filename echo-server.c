#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "err.h"

#define BUFFER_SIZE 1000000
#define QUEUE_LENGTH 5

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fatal("Usage: %s <port>", argv[0]);
	}

	uint16_t port = read_port(argv[1]);
	printf("Listening on port %u\n", port);

	int socket_fd = open_socket();
	bind_socket(socket_fd, port);

	// switch to listening (passive open)
	start_listening(socket_fd, QUEUE_LENGTH);


	char buffer[BUFFER_SIZE];
	for (;;) {
		memset(buffer, 0, BUFFER_SIZE);
		struct sockaddr_in client_address;
		int client_fd = accept_connection(socket_fd, &client_address);
		char *client_ip = inet_ntoa(client_address.sin_addr);
		// We don't need to free this,
		// it is a pointer to a static buffer.

		uint16_t client_port = ntohs(client_address.sin_port);
		printf("Accepted connection from %s:%d\n", client_ip, client_port);

		// Reading needs to be done in a loop, because:
		// 1. the client may send a message that is larger than the buffer
		// 2. a single read() call may not read the entire message, even if it fits in the buffer
		// 3. in general, there is no rule that for each client's write(), there will be a corresponding read()
		size_t read_length;
		size_t counter = 0;
		do {
			int flags = 0;
			read_length = receive_message(client_fd, buffer, BUFFER_SIZE,
			                              flags);
			if (read_length > 0) {

				FILE *fp = fopen("result.txt", "ab+");
				if (fp == NULL) {
					fatal("Could not open file");
				}

				fputs(buffer, fp);
				fputs("\n\n", fp);
				printf("Received %zd bytes\n", read_length);
				counter += read_length;
				// note: we specify the length of the printed string
				fclose(fp);
			}
		} while (read_length > 0);
		printf("Received %zd bytes in total\n", counter);
		printf("Closing connection\n");
		CHECK(close(client_fd));

		if (strncmp(buffer, "exit", 4) == 0) {
			break;
		}
	}

	CHECK(close(socket_fd));

	return 0;
}
