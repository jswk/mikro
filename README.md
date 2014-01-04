Serwer HTTP wspierający IPv6
============================

1. Ogólny opis projektu
-----------------------
Tematem naszego projektu jest skonstruowanie prostego serwera http. Wszystkie
protokoły z kolejnych warstw modelu OSI/ISO będą przez nas implementowane.
Drugą częścią projektu będzie stworzenie programu klienckiego umożliwiającego
wysyłanie instrukcji sterujących do serwera, który w odpowiedzi wysyłał będzie
potwierdzenie do klienta. Strona ta będzie służyła jako środek do testowania
naszego serwera. Istotnym elementem naszego projektu będzie wsparcie dla
protokołu IPv6, zamiast mniej przyszłościowego IPv4.


2. Wykaz użytych sprzętów i podzespołów
------------------------------------
W projekcie skorzystamy z platformy Arduino Uno R3, zawierającej mikrokontroler
ATmega328P oraz modułu sieciowego ETHERNET ENC28J60, kompatybilnego z tymże
sprzętem lub będziemy używać komputera stacjonarnego w celu symulacji tejże płytki.


3. Opis realizowanych protokołów i ich zakres
---------------------------------------------
W celu realizacji podstawowych funkcjonalności serwera http (to jest metod GET i POST)
koniecznym jest zapewnienie wsparcia szeregu protokołów. Nie będziemy w stanie
zaimplementować wszystkich funkcjonalności tychże protokołów, jednak skupimy się
na tych ich elementach, które umożliwią pracę naszego serwera. Wspomniane protokoły to:
* NDP
* IPv6
* TCP
* HTTP

Protokół NDP posłuży nam do przekonwertowania adresu MAC do IP, natomiast TCP
zapewni możliwość przesyłania danych między klientem i serwerem. Dzięki
wykorzystaniu protokołu IP w wersji 6, a nie 4 zwiększymy możliwą przestrzeń
adresową, z adresów 32 bitowych do 128 bitowych. W protokole HTTP skupimy się
na metodach GET i POST.


4. Części systemu
-----------------

### 1. PHY
Warstwa fizyczna łącza zapewniana jest przez układ ENC28J60, do obsługi którego
wykorzystaliśmy bibliotekę będącą częścią m.in. projektu github.com/jcw/ethercard
i która stworzona została przez Guido Socher'a i udostępniona na licencji GPLv2.

### 1. Implementacja Ethernet
Klasa: Ethernet
Klasa ta zapewnia obsługę warstwy łącza danych. Zapisuje ona również informacje
o adresie MAC urządzenia, który może być dowolnie ustalany podczas inicjalizacji.
Jest ona zdefiniowana w pliku ethernet.h, zaś jej implementacja znajduje się w pliku 
ethernet.cpp. Za pomocą metod statycznych umożliwia ona obsługę pakietu ethernet
(obejmuje to zarówno przygotowanie pakietu do wysłania (ustawienie odpowiednich adresów
MAC, nadawcy jak i odbiorcy), wysłanie przygotowanego wcześniej pakietu jak i 
przetworzenie odebranego pakietu). Klasa ta udostępnia także szereg metod pomocniczych
umożliwiających na przykład pobranie adresu MAC nadawcy danej ramki czy skopiowanie
adresu MAC zlokalizowanego w pamięci za pomocą wskaźnika do innego miejsca w pamięci.
Co istotne, w ramach wykonania metod tej klasy, związanych z wysyłaniem i odbieraniem
ramki, obecne już jest generowanie sumy kontrlonej za pomocą algorytmu CRC (w przypadku 
przygotowanie ramki do wysłania) jak i weryfikacja tejże sumy kontrolenj w przypadku
odbierania ramki. 

	class Ethernet {
	public:
	static uint8_t* buffer;
	static uint8_t* MAC;
	static uint16_t packetPrepare(const uint8_t* dest_mac, uint16_t typelen);
	static void packetSend(uint16_t length);
	static void packetProcess(uint16_t length);

	static uint8_t* getSrcMAC();

	static uint16_t getTypeLen();

	static void cp_mac(uint8_t *to, const uint8_t *from);
	};


### 2. Implementacja NDP
Klasa: NDP
NDP jest to protokół związany z IPv6. Jest on poniekąd odpowiednikiem ARP w IPv4.
Operuje na warstwie łącza danych. Jego zdaniem jest między innymi: 
* autokonfiguracja węzłów w sieci
* odkrywanie innych elementów sieci
* ustalanie adresu warstwy łącza danych 
* znajdowanie routerów
* inne.

W przypdaku naszej implementacji skupiliśmy się na ustalaniu adresów warstwy łącza danych,
zarówno na potrzeby wysyłania przez nas ramek jak i w odpowiedzi na zapytania innych
węzłów sieci. Jest ona zdefiniowana w pliku ndp.h, zaś jej implementacja znajduje się w pliku 
ndp.cpp. Kluczowa z punktu widzenia implementowanej przez nas funkcjonalności protokołu NDP
struktura, zawierająca adres mac i odpowiadający mu adres IP w  wersji szóstej, została przez
nas zdefiniowana w następujący sposób:

	struct ndp_pair {
	uint8_t ip[16];
	uint8_t mac[6];
	uint32_t created;
	};

Z powodu ograniczenia pamięci w wykorzystywanej przez nas platformie, zmuszeni byliśmy do 
ustalenia liczby przechowywanych w ten sposób par do 3. Pole created zawiera czas odświeżenia
wpisu w tejże tablicy i umożliwia ocenę poprawności przechowywanych danych.
Sama klasa wypełniająca zadania protokołu NDP jest zdefiniowana następująco:

	class NDP {
	public:
	static uint8_t IP[];
	static struct ndp_pair pairs[];
	static uint8_t* getMAC(uint8_t *ip);
	static void savePairing(uint8_t *ip, uint8_t *mac);

	static void handleAdvertisment(struct ICMPv6_header *header);
	static void handleSolicitation(struct ICMPv6_header *header);
	static void sendAdvertisment(uint8_t *dst_ip, bool solicited, bool override);

	};

Tak jak w wypadku klasy Ethernet, zawiera ona szereg statycznych metod. Dwie z nich służa do 
bezpośredniego operowania na wspomnianej wcześniej tablicy struktur ndp_pair. Są to getMAC,
umożliwiająca uzyskanie adresu MAC, na podstawie adresu IP przekazanego jako parametr oraz
savePairing, umożiwiająca dokonanie zapisu pary adres MAC - adres IP, podanej jako argument
do wspomnianej tablicy, lub w wypadku istnienia już takiego wpisu, odświeżenie pola
created.
Metoda handleAdvertisment umożliwia obsługę ramek typu Neighbor Advertisement, będących 
odpowiedziami na żądania uzyskania adresu MAC : Neighbor Solicitation. Uaktualnia ona
tablicę z parami MAC - IP nowo nabytą informacją. Metoda pozwalająca na odpowiedź na 
Neighbor Solicitation, to handleSolicitation, która za pomocą metody sendAdvertisment
wysyła ramkę typu Neighbor Advertisement.


### 3. Implementacja IPv6
Klasy: ICMPv6 i IPv6
IPv6 to protokół komunikacyjny, następca IPv4. Zapewnia on większą przestrzeń adresowania
w porównaniu do jego poprzednika (z 32 do 128 bitów) oraz szereg innych usprawnień, na 
przykład większą elastyczność dzięki wprowadzeniu rozszerzeń.
Klasa IPv6 jest zdefiniowana w pliku IPv6.h i zaimplementowana w pliku IPv6.cpp. 
Struktura przechowywująca informacje o zawarte w nagłówku IPv6 jest zdefiniowana następująco:

	struct IPv6_header {
	uint32_t first4octets;
	uint16_t payload_length;
	uint8_t next_header;
	uint8_t hop_limit;
	uint8_t src_ip[16];
	uint8_t dst_ip[16];
	};
	
Najbardziej potrzebne nam informacje w niej zawarte to adresy docelowe i źródłowe oraz pole 
next_header.

Sama klasa wypełniająca zadania protokołu IPv6 jest zdefiniowana następująco:

	class IPv6 {
	public:

	static uint8_t* buffer;
	static uint16_t* address;
	static struct IPv6_header* header;

	static void packetProcess(uint16_t offset, uint16_t length);
	static uint16_t generateChecksum(uint16_t correction);
	static uint16_t packetPrepare(uint8_t *dst_ip, uint8_t next_header, uint16_t length);
	static void packetSend(uint16_t length);

	static bool filter(uint8_t *ip);
	static void cp_ip(uint8_t *to, const uint8_t *from);
	};
	
Podobnie jak w przypadku klasy Ethernet umożliwia ona przetworzenie pakietu przychodzącego
oraz przygotowanie i wysłanie pakietu do wybranego adresu docelowego. Udostępniony jest też
szerego metod pomocniczych umożliwiających:
* wygenerowanie checksumy dla ramki IPv6
* sprawdzenie czy należy przetwarzać ramkę (metoda filter)
* metoda umożliwiająca kopiowanie adresu ip między miejscami w pamięci

W ramach przetwarzania ramki, jeśli pole next_header przyjmie wartość stałej ICMPv6_NEXT_HEADER,
wykorzystana zostaje klasa ICMPv6 w celu przetworznia takiego nagłówka.

Protokół ICMPv6 (Internet Control Message Protocol) ma szereg zastosowań i rozszerzeń, my skupiliśmy
się na trzech kluczowych, to jest:
* obsługę Neighbour Advertisment
* obsługę Neighbour Solicitation
* obsługę Ping Request

Pierwsze dwa obsługiwana są przez omówioną wcześniej klasę NDP, przez wykorzystanie metod:
handleSolicitation i handleAdvertisment. Trzeci obsługiwanay jest z wykorzystaniem metody 
handlePingRequest samej klasy ICMPv6. Wykorzystuje ona metody klasy IPv6 w celu odesłania 
odpowiedzi na zapytanie typu ping request (ping reply).


### 4. Implementacja TCP
### 5. Implementacja HTTP
### 6. Obsługa zapytań POST po stronie serwera


5. Postęp prac
--------------

### 12.11.2013
Zakup Arduino Uno i modułu Ethernet ENC28J60. Wstępne przetestowanie
sprzętu i konfiguracja środowiska programistycznego do obsługi mikrokontrolera.

### 26.11.2013
Połączenia Arduino z modułem Ethernet. Implementacja warstwy Ethernet wraz
z walikacją poprawności przesyłanych danych poprzez CRC. Rejestracja komunikacji
poprzez Wireshark.

6. Przydatne komendy
--------------------
Po sklonowaniu projektu wywołaj `npm install` w katalogu głównym.

Aby zbudować dokumentację należy wywołać `node markdown.js`. Plik wynikowy zapisywany jest jako `README.pdf`.