#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "err.h"
#include "common.h"
#include "time.h"

#define BUFFER_SIZE 1000000

char shared_buffer[BUFFER_SIZE];

static char *rand_string(size_t size) {
    char* str = malloc(sizeof(char)* size);
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK...";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
	    fatal("Usage: %s <host> <port> <packages number> <data size> ...\n", argv[0]);
    }

    char *host = argv[1];
    uint16_t port = read_port(argv[2]);
    struct sockaddr_in server_address = get_address(host, port);

    int socket_fd = open_socket();

    connect_socket(socket_fd, &server_address);

    char *server_ip = get_ip(&server_address);
    uint16_t server_port = get_port(&server_address);

    unsigned long packages_number = strtoul(argv[3], NULL, 10);
    PRINT_ERRNO();
    unsigned long data_size = strtoul(argv[4], NULL, 10);
    PRINT_ERRNO();

    for (int i = 0; i < (int) packages_number; i++) {
		char *random_str = rand_string(data_size + 1);
        size_t message_length = strnlen(random_str, BUFFER_SIZE);
        int flags = 0;
        send_message(socket_fd, random_str, message_length, flags);
		free(random_str);
    }
	printf("sent to %s:%u: %lu random_massages with %lu bytes\n", server_ip, server_port, packages_number, data_size);

    // Notify server that we're done sending messages
    CHECK_ERRNO(shutdown(socket_fd, SHUT_WR));
    CHECK_ERRNO(close(socket_fd));

    return 0;
}
