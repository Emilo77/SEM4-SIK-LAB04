# Scenariusz 4 - TCP
1 Wprowadzenie

Podobnie jak w przypadku UDP, do komunikacji po TCP także używamy gniazd. Różnicą jest to, że serwer otwiera gniazdo w trybie nasłuchu. Serwer wybiera port (oraz opcjonalnie adres interfejsu sieciowego, jeżeli chce przyjmować tylko połączenia kierowane na adres tego interfejsu). Na każdym porcie nasłuchuje co najwyżej jeden proces serwera (lub jeden na interfejs, jeżeli podamy adresy interfejsów – system operacyjny na podstawie przychodzącego pakietu musi jednoznacznie określić, który program powinien go obsłużyć). Na przykład serwer HTTP zwykle używa portu 80 lub 8080, serwer telnet – 22, a serwer daytime – 13. Standardowe przypisanie portów do serwerów (well-known ports) znajdziesz w /etc/services.

Klient najpierw nawiązuje połączenie z wybranym serwerem (podając adres IP i numer portu), a następnie korzysta z gniazda podobnie jak z otwartego pliku.

Dokładniejszy opis sposobu stosowania gniazd można znaleźć w man 7 socket.
2 Serwer echo

Przykładowy serwer echo działający z wykorzystaniem protokołu TCP otrzymuje od klienta ciąg znaków, a następnie odsyła go z powrotem.
2.1 Przepływ sterowania - sekwencja wywołań funkcji API

socket -> bind -> listen -> accept -> (read/write) -> [ close ]

2.2 socket: stwórz gniazdo komunikacji sieciowej

int socket(int domain, int type, int protocol);

Analogicznie do UDP (domain to AF_INET, type to SOCK_STREAM).
2.3 bind: dowiąż gniazdo do lokalnego adresu

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

Analogicznie jak przy UDP.
2.4 listen: przełącz gniazdo w tryb nasłuchu

int listen(int sockfd, int backlog);

    sockfd: deskryptor gniazda
    backlog: maksymalna długość kolejki żądań połączeń

2.5 accept

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

Funkcja accept() pobiera pierwsze z oczekujących żądań połączenia z gniazda sockfd i tworzy dla niego nowe, połączone gniazdo. Identyfikator tego gniazda zwracany jest jako rezultat funkcji. Funkcja accept() blokuje się, jeśli nie ma żadnego żądania połączenia.

    addr: wskaźnik do struktury, w której zostanie umieszczony adres połączonego klienta
    addrlen: wskaźnik do zmiennej, która zawiera długość struktury opisującej adres połączonego klienta (uwaga: ten parametr musi być poprawnie zainicjowany przed wywołaniem funkcji)

2.6 Obsługa klienta: read / write

W przykładzie obsługa klienta odbywa się w głównym (jedynym) wątku programu. Używamy standardowych funkcji czytania z deskryptora i pisania do deskryptora:

int read(int fd, void* buf, size_t count);
int write(int fd, const void* buf, size_t count);

W przypadku komunikacji strumieniowej jaką zapewnia protokół TCP serwer nie kontroluje otrzywanych danych. Na przykład nie może założyć, że dane odebrane od klienta są zakończone zerem. Przeanalizuj, jak przykładowy serwer echo radzi sobie z tym problemem.

Funkcja read() nie musi zwrócić od razu całego komunikatu wysłanego przez klienta. Ponieważ TCP dzieli strumień przesyłanych bajtów na segmenty, które są buforowane, komunikaty klienta mogą być dowolnie dzielone i łączone podczas transmisji.
2.7 Ćwiczenia

    Uruchom serwer (./echo-server).

    Znajdź gniazdo serwera w wykazie otwartych gniazd (netstat -ltnp).

    Podłącz się do serwera programem telnet lub nc.

    Znajdź połączenie w wykazie istniejących połączeń (netstat -tnp).

    Podczas gdy jedna sesja jest otwarta, spróbuj na innej konsoli połączyć się z serwerem jeszcze raz.

3 Klient echo
3.1 Przepływ sterowania - sekwencja wywołań funkcji API

socket -> connect -> (read/write) -> [ close ]

3.2 connect: połączenie się ze zdalnym adresem

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

    sockfd: deskryptor gniazda
    addr: wskaźnik do struktury opisującej adres, z którym będziemy się łączyć (pamiętaj o sieciowej kolejności bajtów)
    addrlen: długość struktury opisującej adres

3.3 read/write: czytanie i pisanie

Zwróć uwagę, że klient nie wysyła zera, które zwykle kończy łańcuchy znaków w C. Klient nie może również zakładać, że dane od serwera są zakończone zerem. Przeanalizuj, jak klient radzi sobie z tym problemem.
3.4 close lub shutdown: zamknięcie połączenia

int close(int fd);

Połączenie może zostać zamknięte przez klienta lub przez serwer. Gdy jedna ze stron zamyka połączenie, read() wykonany przez drugą stronę zwraca 0, natomiast próba zapisu powoduje dostarczenie do procesu sygnału SIGPIPE i w efekcie zakończenie procesu z błędem EPIPE.

int shutdown(int fd, int how);

Możliwe jest zamknięcie tylko jednej strony połączenia. Jeśli how = SHUT_RD uniemożliwiamy odczyt, a jeśli how = SHUT_WR - zapis do gniazda. Natomiast how = SHUT_RDWR zamyka obydwa kierunki. W tym przypadku różnica między funkcją close() a shutdown() polega na tym, że pierwsza operacja powoduje wysłanie segmentu RST a druga FIN.
3.5 getaddrinfo: tłumaczenie adresów na struct sockaddr

int getaddrinfo(const char *node, const char *service,
    const struct addrinfo *hints,
    struct addrinfo **res);

Funkcja jest opisana w scenariuszu o UDP.
3.6 Ćwiczenia

    Uruchom klienta i połącz się ze swoim lokalnym serwerem.

    Połącz się z serwerem sąsiada.

    Uruchom wireshark. Następnie uruchom klienta. Zaobserwuj pakiety wysyłane przez klienta i odsyłane przez serwer.

    Po wywołaniu connect() zawieś wykonanie klienta (np. przez sleep(10)). Zaobserwuj, czy connect() powoduje wysłanie jakiś pakietów, czy też wysyłane one są dopiero po pierwszym write().

    W podobny sposób opóźnij wykonanie close(). Zidentyfikuj pakiety wysyłane przez klienta i przez serwer w rezultacie wykonania close().

    Zmodyfikuj serwer tak, by każdy ciąg znaków odsyłany do klienta poprzedzał twoimi inicjałami.

4 Klient i serwer nums

W plikach nums-server.c i nums-client.c znajduje się inna para serwer-klient wykorzystująca do komunikacji protokół TCP. Klient dla każdej wczytanej z wejścia liczby wysyła do serwera parę: numer kolejny (liczba 16-bitowa) oraz wczytaną liczbę (32-bitową). Serwer wypisuje otrzymane dane.
4.1 Ciekawe elementy

Programy nums-*.c korzystają z tych samych funkcji systemowych co programy echo-*.c. Warto jednak zwrócić uwagę na następujące elementy:

    Powinniśmy precyzyjnie podawać ilubajtowe liczby wysyłamy i odbieramy. Możemy to zrobić korzystając z typów takich jak uint32_t. Długość liczby typu int nie jest ustalona i może zależeć od systemu.

    Jeśli chcemy wysyłać lub odbierać całą strukturę (a nie pojedyncze dane typów prostych), powinniśmy ją zadeklarować z atrybutem __packed__. Bez tego kompilator pomiędzy polami struktury może wstawić dodatkowe nieużywane bajty (aby przyspieszyć działanie programu). Zobacz przykładowy program struct-in-memory.c.

    Przed wysłaniem zamieniamy kolejność bajtów w liczbie na sieciową (funkcje htons(), htonl()).

    Możliwe jest, że wywołanie read nie zwróci całej wysłanej struktury, a jedynie jej część. Konieczne jest w takim przypadku zastosowanie funkcji recv z odpowiednią flagą lub innego sposobu obsługi zapewniającego odebranie całej struktury.

4.2 Ćwiczenia

    Uruchom serwer i połącz się klientem ze swoim lokalnym serwerem.

    Sprawdź, jaka jest długość liczby int na twoim komputerze i na students.

    Sprawdź, jaka będzie długość struktury DataStructure, jeśli zadeklarujemy ją bez atrybutu __packed__.

    Sprawdź, że (nawet w przypadku wysyłania pojedynczych sześciobajtowych struktur, jak w przykładowym kliencie) read() rzeczywiście odczytuje czasem mniej bajtów niż długość struktury (spróbuj podać do klienta dużo danych na raz, pracując przez sieć).

Dla chętnych

Obejrzyj 10-minutowy film podsumowujący różnice między protokołami TCP a UDP.

https://youtu.be/Vdc8TCESIg8

Ćwiczenie punktowane (1 pkt)

Zmodyfikuj klienta w ten sposób, aby przyjmował dwa dodatkowe parametry: liczbę pakietów do wysłania (n) i rozmiar porcji danych (k). Klient n razy przesyła k dowolnych bajtów do serwera. Do skonstruowania takiego komunikatu możesz użyć funkcji memset().

Zmodyfikuj serwer w ten sposób, aby czytał w pętli komunikaty od klienta. Po otrzymaniu danych serwer dopisuje je do pliku o ustalonej nazwie, a na standardowe wyjście podaje jedynie liczbę otrzymanych bajtów.

Uruchom klienta i serwera na dwóch różnych maszynach. Przetestuj różne wartości k (np. 10, 100, 1000, 5000, 70000). Zaobserwuj, czy wartości wypisywane przez serwer zależą od parametru klienta.

Rozwiązania można prezentować w trakcie zajęć nr 4 lub 5.
