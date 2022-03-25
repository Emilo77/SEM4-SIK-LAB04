#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>

struct packet {
    uint8_t type;
    uint32_t param;
};

int main() {
    struct packet pkt = {
            .type = 42,
            .param = htonl(256),
    };

    size_t size = sizeof(pkt);

    //
    // Jak myślisz, ile bajtów zajmuje w pamięci zmienna pkt? Jakie to bajty?
    //
    // Uruchom program i sprawdź.
    //
    // Dopisz w deklaracji struct packet klauzulę
    //   __attribute__((__packed__))
    // (pomiędzy 'struct' a 'packet') i sprawdź, co teraz.
    //

    uint8_t *pchar = (uint8_t *) &pkt;

    printf("size=%zu\n", size);
    for (int i = 0; i < size; i++) {
        printf("%02X ", pchar[i]);
    }
    printf("\n");

    return 0;
}
