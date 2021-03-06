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
adresu MAC zlokalizowanego w pamięci za pomocą wskaźnika, do innego miejsca w pamięci.
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

Przykład skutków działania tejże klasy został umieszczony na rysunku ethernet.png.
Są tam pokazane przykładowe pola ramki ethernetowej wysłanej przez nasz serwer zarejsetrowane
przy pomocy programu Wireshark.
![Rys.](../master/blob/ethernet.png)

### 2. Implementacja NDP
Klasa: NDP
NDP jest to protokół związany z IPv6. Jest on poniekąd odpowiednikiem ARP w IPv4.
Operuje na warstwie łącza danych. Jego zdaniem jest między innymi: 
* autokonfiguracja węzłów w sieci
* odkrywanie innych urządzeń w sieci
* ustalanie adresu warstwy łącza danych 
* znajdowanie routerów
* i inne.

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

Przykład skutków działania tegoż protokołu zostały umieszczone na rysunku solic.png.
Jest na nim widoczna wymiana pakietów związanych z protokołem NDp, to jest najpierw zapytanie
typu Neighbour Solicitation od urządzania zewnętrznego do serwera i potem odpowiedź na to 
żądanie czyli pakiet Neighbour Advertisment wysłany przez serwer do urządzenia zewnętrznego.
![Rys.](../master/blob/solic.png)

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
* wygenerowanie checksumy dla ramki IPv6 (metoda generateChecksum)
* sprawdzenie czy należy przetwarzać ramkę (metoda filter)
* metoda umożliwiająca kopiowanie adresu ip między miejscami w pamięci (metoda cp_ip)

W ramach przetwarzania ramki, jeśli pole next_header przyjmie wartość stałej ICMPv6_NEXT_HEADER,
wykorzystana zostaje klasa ICMPv6 w celu przetworznia takiego nagłówka.

Protokół ICMPv6 (Internet Control Message Protocol) ma szereg zastosowań i rozszerzeń, my skupiliśmy
się na trzech kluczowych, to jest:
* obsługę Neighbour Advertisment
* obsługę Neighbour Solicitation
* obsługę Ping Request

Pierwsze dwa obsługiwana są przez omówioną wcześniej klasę NDP, przez wykorzystanie metod:
handleSolicitation i handleAdvertisment. Trzeci obsługiwanay jest z wykorzystaniem metody 
handlePingRequest tej samej klasy, to jesy ICMPv6. Wykorzystuje ona metody klasy IPv6 w celu odesłania 
odpowiedzi na zapytanie typu ping request (ping reply).


### 4. Implementacja TCP
Klasa TCP
TCP to strumieniowy protokół połączeniowy. Jest on niezawodny (sprawdza czy pakiety nie są tracone).
Wykorzystuje on przedstawiony wcześniej protokół IPv6 do odbierania i wysyłania danych.
TCP jest częścią transportowej warstwy modelu OSI.
Klasa TCP jest zdefiniowana w pliku tcp.h i zaimplementowana w pliku tcp.cpp. 
Kluczowa struktura modelująca nagłówek protokołu TCP jest zdefiniowana następująco:
	struct TCP_header {
	uint16_t src_port;
	uint16_t dst_port;
	uint32_t seq_num;
	uint32_t ack_num;
	uint8_t data_offset; 
	uint8_t control_bits; 
	uint16_t window;
	uint16_t checksum;
	uint16_t urgent_pointer;
	uint8_t options[];
	};

Pola src_port i dst_port zawierają porty odpowiednio nadawcy i odbiorcy. Pole seq_num pomaga podczas procesu 
fragmentacji, ack_num czyli numer potwierdzenia pozwala na sprawdzanie czy pakiety zostały zgubione.
Ostatnim istotnym dla nas polem jest checksum, zawierający sumę kontrolną, pozwalający sprawdzić poprawność
wymienionych danych.

Do obsługi pakietów wykorzystywany jest handler. Zdefiniowany jest on za pomocą stuktury:
	struct TCP_handler {
	uint16_t port;
	void (*handler)(struct TCP_handler_args*);
	struct TCP_handler* next;
	};
Funkcja obsługi przyjmuję jako argument strukturę:
	struct TCP_handler_args {
	struct TCP_status* status;
	uint8_t* data;
	uint16_t length;
	};
,gdzie TCP_status zdefiniowany jest następująco:
struct TCP_status {
	int state;
	uint8_t ip[16];
	uint16_t port;
	uint16_t local_port;
	uint32_t local_num;
	uint32_t remote_num;
	void (*handler)(struct TCP_handler_args*);
};

Struktury te (czyli TCP_status i TCP_handler_args) reprezentują dane zawarte w ramce TCP i warstwach niższych 
w przyjazny dla warstw wyższych sposób. 
W TCP_handler_args.length mamy długość danych natomiast TCP_handler_args.data to wskaźnik do danych.
TCP_status zawiera natomiast informacje takie jak numer portu czy adres ip urządzenia z którym nawiązana jest 
komunikacja.

Dodatkowo zdefiniowana jest struktura TCP_status_node, dzięki ktorej możliwe jest przechowywanie obiektów TCP_status
w liście jednokierunkowej. Do operacji na niej zdefiniowane zostały trzy funkcje:
- struct TCP_status* getStatus(uint8_t* ip, uint16_t port) , umożliwiające uzyskanie struktury TCP_status
przechowywanej w liście na bazie pary: adres ip - numer portu, podanej jako argument.
- TCP_status* addStatus(uint8_t* ip, uint16_t port, uint16_t local_port), pozwalającej stworzyć i dodać strukturę
TCP_status do listy, na podstawie podanych argumentów (kolejno: adres ip urządzenia, jego port i port lokalny Arduino)
- void removeStatus(uint8_t* ip, uint16_t port) , analogiczne do getStatus, tylko, że zamiast zwracać, to usuwa wpis
w liście na bazie adresu ip i numeru portu.

Sama klasa realizująca zadania protokołu TCP zdefiniowana jest w następujący sposób:
	class TCP {
	public:

	static uint8_t* buffer;
	static struct TCP_header* header;
	static void initialize(uint8_t* buffer);

	static void packetProcess(uint8_t* src_ip, uint16_t offset, uint16_t length);

	static void send(TCP_status* status, uint8_t* data, uint16_t length);

	static void registerHandler(uint16_t port, void (*handler)(struct TCP_handler_args* args));
	};
	
Tak jak we wcześniejszych klasach, TCP udostępnia szereg metod statycznych umożliwiających szereg działań 
związanych z funkcjonowaniem protokołu TCP. W także statycznych polach buffer i header przechowywane są
kolejno: początek dostępnego miejsca w pamięci oraz początek nagłówka TCP ramki przechowanej w pamięci.

Metoda initialize jest konieczna do działania pozostałych metod, ponieważ ustawia pole buffer, niezbędne 
do jakichkolwiek operacji na pamięci.

Metoda packetProcess jest analogiczna do innych metod o tej nazwie wśród wcześniej wspomnianych klas, służy
więc do przetwarzania pakietu. Posługuje się ona wspominianymi wcześniej funkcjami zarządzającymi TCP_status.
W zależności od rodzaju otrzymanej wiadomości konstruję inny nagłówek TCP_header i go wysyła za pomocą metody
IPv6::packetSend. 
Metoda registerHandler pozwala zarejestrować opisane wcześniej handlery, które w razie otrzymania pakietu
z zgadzającego się z danymi w interesującej go strukturze TCP_status, zostają wywołane.


### 5. Implementacja HTTP
Klasa HTTP.
Hypertext Transfer Protocol (HTTP), czyli  protokół przesyłania dokumentów hipertekstowych, jest elementem 
warstwy aplikacji modelu OSI. W tym właśnie protokole przesyłane są żądania dotyczące dokumentów związanych
z siecią internetową. Normalizuje on formę zarówno żądań klienta, jak i odpowiedzi serwera. Zawiera szereg metod,
w nasze immplementacji zawarliśmy jednak tylko wybrane, konieczne do działania naszego serwera. W przeciwieństwie
do TCP jest on bezstanowy.

Klasa HTTP jest zdefiniowana w pliku kttp.h i zaimplementowana w pliku http.cpp. 
Klasa realizująca zadania protokołu http jest zdefiniowana w następujący sposób:

	class HTTP {
	public:
	static char buffer[];
	static void initialize();
	static void handler(TCP_handler_args* args);
	};
	
Tak jak w przypadku pozostałych klas wszystkie metody i pola są statyczne.
Kluczowym i jedynym dostępnym zewnętrznie do użytku elementem klasy jest metoda handler. Przyjmuje ona argument
typu TCP_handler_args*. Jest ona wykorzystywana jako wcześniej opisany handler w klasie TCP. Jest ona rejestrowana
przy pomocy TCP::registerHandler, i w naszym przypadku nasłuchuje na porcie 80 (standardowy port), można to jednak 
zmienić, podając inny parametr w metodzie TCP::registerHandler. Metoda ta sprawdza rodzaj zapytania i wywołuje
jedną z metod pomocniczych by go obsłużyć. Wszystkie te metody konstruują odpowiedź i następnie ją odsyłają.
Wszystkie odpowiedzi zawierają kod odpowiedzi (OK lub ERROR), typ zawartości (content_type) oraz jej długość.
I tak metoda serveIndex skutkuje odesłaniem pakietu zawierającego potwierdzenie poprawności (HTTP/1.1 200 OK) i 
ciało strony głównej na sztywno wpisane w kodzie:
```html
	<html>
			<head>
				<title>Arduino Board</title>
				<script type=\"text/javascript\" src=\"//local.cdn.arduino/boot.js\"></script>
			</head>
			<body></body>
	</html>
```
Z powodu małej objętość dostępnej na naszej platformie pamięci koniecznym było odesłanie jedynie takiego 
prostego dokumentu html, natomiast bardziej zaawansowane funkcjonalności strony zawarte są pliku javascriptowym
boot.js. Oprócz tego dostępny jest zestaw funkcji pozwalających przetwarzać żądania pozwalające manipulowaniem
urządzeniem peryferyjnym (w naszym przypadku diodom) na serwerze jak i otrzymanie w wiadomości zwrotnej
stanu tegoż urządzenia. W wypadku nie rozpoznania żądania jako poprawnego zostaje użyta funkcja serve404. 
Zwraca on kod odpowiedzi równy HTTP/1.1 404 Not Found jak i dokument html:
```html
	<html>
		<head>
			<title>Arduino Board</title>
		</head>
		<body><h1>404</h1></body>
	</html>
```
Przykładowa wymiana ramek między serwerem i urządzeniem zewnętrznym została zaprezentowana na rysunku http.png.
Pokazuje on wysłanie trzech kolejnych żądań (jest to jedno typu GET i dwa typu POST) pozwalających zmienić
stan urządzenia peryferyjnego podłączonego do serwera i odpowiedzi na nie wysłane z serwera do urządzenia 
będącego nadawcą wyżej wspomnianych żądań. 
![Rys.](../master/blob/http.png)

5. Postęp prac
--------------

### 12.11.2013
Zakup Arduino Uno i modułu Ethernet ENC28J60. Wstępne przetestowanie
sprzętu i konfiguracja środowiska programistycznego do obsługi mikrokontrolera.

### 26.11.2013
Połączenia Arduino z modułem Ethernet. Implementacja warstwy Ethernet wraz
z walikdcją poprawności przesyłanych danych poprzez CRC. Rejestracja komunikacji
poprzez Wireshark.

### 12.2013 spotkanie I
Implementacja protokołu NDP oraz części protokołu IPv6.

### 12.2013 spotkanie II
Implementacja reszty protokołu IPv6 wraz z całością ICMPv6 w tym obsługą pinga jak i zarządzaniem czynności
z protokołu NDP. Rejestracja komunikacji przez Wiresharka.

### 01.2013 spotkanie I
Implementacja protokołów TCP i HTTP. Rejestracja komunikacji przez Wiresharka.

### 01.2013 spotkanie II
Uzupełnienie dokumentacji projektu oraz utworzenie kodu w javascripcie umożliwiającego zaprezentowanie
funkcjonalności udostęnianych przez serwer.

6. Przydatne komendy
--------------------
Po sklonowaniu projektu wywołaj `npm install` w katalogu głównym.

Aby zbudować dokumentację należy wywołać `node markdown.js`. Plik wynikowy zapisywany jest jako `README.pdf`.