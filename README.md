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
* ARP
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
Klasa ta zapewnia obsługę warstwy łącza danych. Zapisuje ona również informacje
o adresie MAC urządzenia, który może być dowolnie ustalany podczas inicjalizacji.

	class Ethernet {
	public:
		static uint8_t* buffer;
		static uint8_t* MAC;
		static uint16_t packetPrepare(const uint8_t* dest_mac, uint16_t typelen);
		static void packetSend(uint16_t length);
		static uint16_t packetReceive();
	
		static void getSrcMAC(uint8_t* mac, const uint8_t* buffer);
	
		static uint16_t getTypeLen(const uint8_t* buffer);
	};


### 2. Implementacja NDP
### 3. Implementacja IPv6
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