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

### 1. Implementacja NDP
### 2. Implementacja IPv6
### 3. Implementacja TCP
### 4. Implementacja HTTP
### 5. Obsługa zapytań POST po stronie serwera


5. Postęp prac
--------------

### 12.11.2013
Zakup Arduino Uno i modułu Ethernet ENC28J60. Wstępne przetestowanie sprzętu i konfiguracja
środowiska programistycznego do obsługi mikrokontrolera.

6. Przydatne komendy
--------------------
Po sklonowaniu projektu wywołaj `npm install` w katalogu głównym.

Aby zbudować dokumentację należy wywołać `node markdown.js`. Plik wynikowy zapisywany jest jako `README.pdf`.