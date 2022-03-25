#include <netdb.h>
#include <stdio.h>
#include <unistd.h>

#include "err.h"
#include "common.h"

// 1) We send uint16_t, uint32_t etc., not int (the length of int is
//   platform-dependent).
// 2) If we want to send a structure, we have to declare it
//   with __attribute__((__packed__)). Otherwise the compiler
//   may add a padding bewteen fields (and then e.g.
//   sizeof(struct DataStucture) is 8, not 6).
struct __attribute__((__packed__)) DataStructure {
    uint16_t seq_no;
    uint32_t number;
};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fatal("Usage: %s host port\n", argv[0]);
    }

    char *host = argv[1];
    uint16_t port = read_port(argv[2]);
    struct sockaddr_in server_address = get_address(host, port);

    int socket_fd = open_socket();

    connect_socket(socket_fd, &server_address);

    struct DataStructure data_to_send;
    uint16_t seq_no = 0;
    uint32_t number = 0;

    // read all numbers from the standard input
    while (scanf("%u", &number) == 1) {

        printf("Sending number %d\n", number);

        // we send numbers in network byte order
        data_to_send.seq_no = htons(seq_no);
        data_to_send.number = htonl(number);
        seq_no++;

        int flags = 0;
        send_message(socket_fd, &data_to_send, sizeof(data_to_send), flags);
    }

    CHECK_ERRNO(close(socket_fd));

    return 0;
}
