#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include "common.h"
#include "err.h"

#define QUEUE_LENGTH     5

struct __attribute__((__packed__)) DataStructure {
    uint16_t seq_no;
    uint32_t number;
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fatal("Usage: %s <port>", argv[0]);
    }

    uint16_t port = read_port(argv[1]);

    int socket_fd = open_socket();
    bind_socket(socket_fd, port);

    start_listening(socket_fd, QUEUE_LENGTH);

    printf("Listening on port %u\n", port);

    struct DataStructure data;
    for (;;) {
        struct sockaddr_in client_address;
        int client_fd = accept_connection(socket_fd, &client_address);
        char *client_ip = inet_ntoa(client_address.sin_addr);
        // We don't need to free this,
        // it is a pointer to a static buffer.

        uint16_t client_port = ntohs(client_address.sin_port);
        printf("Accepted connection from %s:%d\n", client_ip, client_port);

        size_t received_length;
        do {
            // Serwer waits for the whole structure
            int flags = MSG_WAITALL;
            received_length = receive_message(client_fd, &data, sizeof(data), flags);
            if (received_length > 0) {
                printf("Received %zd bytes from %s:%u\n", received_length, client_ip, client_port);
                printf("Received packet no %" PRIu16 ": %" PRIu32 "\n", ntohs(data.seq_no), ntohl(data.number));
            }
        } while (received_length > 0);

        CHECK_ERRNO(close(client_fd));
    }

    return 0;
}
