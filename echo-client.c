#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "err.h"
#include "common.h"

#define BUFFER_SIZE 20

char shared_buffer[BUFFER_SIZE];

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fatal("Usage: %s host port message ...\n", argv[0]);
    }

    char *host = argv[1];
    uint16_t port = read_port(argv[2]);
    struct sockaddr_in server_address = get_address(host, port);

    int socket_fd = open_socket();

    connect_socket(socket_fd, &server_address);

    char *server_ip = get_ip(&server_address);
    uint16_t server_port = get_port(&server_address);

    for (int i = 3; i < argc; i++) {
        size_t message_length = strnlen(argv[i], BUFFER_SIZE);
        int flags = 0;
        send_message(socket_fd, argv[i], message_length, flags);
        printf("Sent %zd bytes to %s:%u: '%.*s'\n", message_length, server_ip, server_port, (int) message_length,
               argv[i]);
    }

    // Notify server that we're done sending messages
    CHECK_ERRNO(shutdown(socket_fd, SHUT_WR));

    size_t received_length;
    do {
        memset(shared_buffer, 0, BUFFER_SIZE); // clean the buffer
        size_t max_length = BUFFER_SIZE - 1; // leave space for the null terminator
        int flags = 0;
        received_length = receive_message(socket_fd, shared_buffer, max_length, flags);
        printf("Received %zd bytes from %s:%u: '%s'\n", received_length, server_ip, server_port, shared_buffer);
    } while (received_length > 0);

    CHECK_ERRNO(close(socket_fd));

    return 0;
}
