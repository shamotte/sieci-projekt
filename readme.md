# Yapu Yapu - aplikacja do czatowania

Yapu Yapu to prosta aplikacja czatu oparta na połączeniach TCP z SSL/TLS - z klientem na Windows (Python) oraz serwerem w C++.
Aplikacja pozwala użytkownikom wysyłać i odbierać wiadomości od wielu osób, oferując funkcje takie jak logowanie, zarządzanie kontaktami oraz historia wiadomości.



## 🔧 Technologie

- **C++** – główny język programowania dla serwera,
- **SQLite** – baza danych do przechowywania wiadomości i użytkowników,
- **Python** – w nim napisany jest klient komunikatora.

## 📁 Struktura projektu

```bash
sieci-projekt
├── Client
│   ├── gui.py
│   ├── main.py
│   ├── edit.png
│   └── includes.hpp
│ 
├── siecioszki_serwerek
│   ├── external
│   ├── src
│   │   ├── client.cpp
│   │   ├── client.hpp
│   │   ├── includes.hpp
│   │   └── serwerek.cpp
│   │ 
│   ├── build.sh
│   ├── server.crt
│   └── server.key
```
## Wymagania 
### Wymagane biblioteki do klienta
Aplikacja wymaga następujących bibliotek:
- `tkinter` 
- `threading`
- `json`
- `time`
- `datetime`

### Wymagane biblioteki do serwera
- `gcc` (```sudo zypper install gcc-c++```)
- `sqlite3` (```sudo zypper install sqlite3-devel```)
  
## 🔨 Kompilacja

- **Skrypt do kompilacji serwera** - znajduje się w pliku ```build.sh```

- **Uruchomienie serwera:**  ```./serwer```

- **Klient:** ```python3 /Client/gui.py``` lub plik wykonywalny `gui.exe`


## Serwer
Serwer obsługuje połączenia TCP zabezpieczone przez SSL/TLS.
Każdy klient łączy się dwoma gniazdami – jedno do wysyłania, drugie do odbierania wiadomości. 
### Funkcje serwera

- Obsługa wielu klientów jednocześnie,
- Przekazywanie wiadomości, nawet gdy użytkownik był offline,

 - Przechowywanie wiadomości w bazie SQLite
 ...



## Klient

### Funkcje
- **Logowanie**: Logowanie za pomocą PIDu użytkownika wraz z podaniem adresu serwera.
- **Kontakty**: Możliwość dodawania kontaktów wraz z możliwością ustawienia personalizowanych nicków z możliwością późniejszej zmiany.
- **Wiadomości**: Obsługa wiadomości - odbieranie oraz wysyłanie w wielu czatach jednocześnie.
- **Powiadomienia**: Notyfikowanie użytkownika o nieprzeczytanych wiadomościach w określonych czatach.
- **Historia czatu**: Wyświetlanie czatu z poprzednich sesji wraz z powiadomieniami o wiadomościach przychodzących, gdy użytkownik był offline.


