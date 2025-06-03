# AutoMagik - natywna aplikacja komputerowa dla warsztatu samochodowego

## AutoMagik to w pełni funkcjonalny projekt przykładowy napisany w C++ z użyciem Qt, zaprojektowany, aby wspierać zarządzanie lokalnym warsztatem napraw samochodowych. Udostępnia interfejs klienta desktopowego dla pracowników warsztatu (menedżerów i pracowników) do obsługi zleceń serwisowych, komunikacji wewnętrznej oraz zbierania danych pojazdów. Backend jest oparty na Firebase, zapewniając synchronizację w czasie rzeczywistym i przechowywanie w chmurze.

## Spis treści

- [Przegląd](#overview)  
- [Kluczowe Funkcje](#key-features)  
  - [Tryby Użytkownika](#user-modes)  
  - [Funkcje Menedżera](#manager-functions)  
  - [Funkcje Pracownika](#worker-functions)  
- [Przegląd Interfejsu Użytkownika](#user-interface-walkthrough)  
  1. [Ekran Wyboru Użytkownika](#1-user-selection-screen)  
  2. [Okno Logowania](#2-sign-in-window)  
  3. [Menedżer: Panel Samochodów / Klientów](#3-manager-cars--clients-panel)  
  4. [Menedżer: Panel Zadań / Pracowników](#4-manager-tasks--workers-panel)  
  5. [Pracownik: Panel Zadań](#5-worker-tasks-panel)  
- [Instalacja](#installation)  
  - [Wymagania Wstępne](#prerequisites)  
  - [Klonowanie i Konfiguracja](#clone-and-configure)  
  - [Kompilacja na Windows/macOS/Linux](#building-on-windowsmacoslinux)  
  - [Uruchamianie Aplikacji](#running-the-application)  
- [Słowa Kluczowe (do wyszukiwania na GitHub)](#keywords-for-github-search)  

## Przegląd

Celem AutoMagik jest przedstawienie założeń i specyfikacji wersji 1.0 aplikacji desktopowej wspierającej zarządzanie warsztatem samochodowym. AutoMagik umożliwia małym i średnim warsztatom:

- Tworzenie, przydzielanie oraz śledzenie zleceń naprawczych (zadań)  
- Zarządzanie kontami pracowników oraz rolami użytkowników  
- Zbieranie i przechowywanie danych pojazdów  
- Komunikację wewnętrzną za pomocą komentarzy przypisanych do zadań  
- Synchronizację wszystkich informacji w czasie rzeczywistym poprzez backend Firebase  

Dostęp do systemu mają wyłącznie autoryzowani użytkownicy (pracownicy warsztatu z aktywnymi kontami). Aplikacja wymaga aktywnego połączenia z internetem, aby synchronizować się z Firebase.

---

## Technologie użyte

**Stos technologiczny:**  
- C++ 17  
- Qt 6 (Qt Widgets)  
- Firebase Realtime Database (klient SDK)  

## Kluczowe Funkcje

### Tryby Użytkownika

Po uruchomieniu AutoMagik użytkownicy wybierają jeden z dwóch typów kont:

- **Menedżer**  
- **Pracownik**

Każdy tryb odblokowuje inny zestaw funkcji, opisany poniżej.

### Funkcje Menedżera

1. **Zarządzanie Zadaniami**  
   - Tworzenie, edycja i usuwanie zadań serwisowych (zleceń naprawczych)  
   - Przydzielanie zadań poszczególnym pracownikom lub grupom  
   - Śledzenie statusu zadania (np. Oczekujące → W toku → Zakończone)  
   - Przegląd wszystkich zadań w warsztacie  

2. **Zarządzanie Użytkownikami i Kontami**  
   - Tworzenie, edycja, dezaktywacja lub usuwanie kont pracowników (pracownik/menedżer)  
   - Resetowanie haseł lub zmiana adresów e-mail dla dowolnego użytkownika  
   - Nadawanie/odbieranie uprawnień administracyjnych  
   - Monitorowanie aktywności logowania i statusu kont  

3. **Zarządzanie Pojazdami i Klientami**  
   - Dodawanie, edycja lub usuwanie rekordów pojazdów (każdy pojazd reprezentuje jednego klienta)  
   - Przegląd historii klienta i powiązanych zadań  
   - Załączanie szczegółów pojazdu (marka, model, numer rejestracyjny, VIN itp.)   

4. **Administracja Systemem**  
   - Nadzór nad integralnością i spójnością danych  
   - Zarządzanie konfiguracją aplikacji (np. punkty końcowe Firebase)  
   - Dostęp do wszystkich komentarzy i wpisów w dzienniku systemowym  

### Funkcje Pracownika

1. **Status Dostępności**  
   - Przełączanie się między **Dostępny (Zalogowany)** a **Niedostępny (Wylogowany)**  

2. **Uwierzytelnianie i Profil**  
   - Logowanie się za pomocą firmowego adresu e-mail i hasła  
   - Zmiana hasła podczas lub po zalogowaniu  
   - Przegląd i edycja danych profilowych (imię, dane kontaktowe)  

3. **Przepływ Pracy z Zadaniami**  
   - Przegląd spersonalizowanej listy przydzielonych zadań  
   - Dostęp do podstawowych informacji o pojeździe dla każdego zadania  
   - Aktualizacja statusu zadania (np. W toku, Wstrzymane, Zakończone)  
   - Dodawanie komentarzy lub aktualizacji postępu do przypisanych zadań  
   - Oznaczanie zadań jako zakończone po wykonaniu pracy  

4. **Komunikacja**  
   - Publikowanie i przeglądanie komentarzy w ramach zadań w celu komunikacji z menedżerem lub innymi pracownikami  

---

## Przegląd Interfejsu Użytkownika

Poniżej znajduje się krótki opis każdego z głównych ekranów interfejsu:

### 1. Ekran Wyboru Użytkownika

- Po uruchomieniu użytkownicy wybierają między trybem **Menedżer** a **Pracownik**.  
- Wybór determinuje, jakie funkcje będą widoczne po zalogowaniu.  

![Ekran wyboru użytkownika](/screenshots/automagik-user.jpg)

### 2. Okno Logowania

- Zarówno Menedżer, jak i Pracownik logują się przy użyciu **E-maila** i **Hasła**.  
- Przy pierwszym logowaniu dla Menedżerów używa się wstępnie ustalonych danych (dostarczonych przez właściciela warsztatu).  
- Użytkownik może tworzyć nowe konta Menedżerów.  
- Po uwierzytelnieniu Menedżera:  
  - Menedżer może tworzyć nowe konta Pracowników.  
  - Każdy nowy użytkownik otrzymuje automatycznie wygenerowany e-mail aktywacyjny (lub tymczasowe hasło).  
- Użytkownicy mogą również zmienić swój e-mail lub hasło poprzez dedykowane okna „Zmień e-mail” i „Zmień hasło”.

![Okno logowania pracownika](/screenshots/worker-sing-in.jpg)

![Okno logowania menedżera](/screenshots/manager-sing-in.jpg)

![Okno tworzenia menedżera](/screenshots/manger-create.jpg)

### 3. Menedżer: Panel Samochodów / Klientów

- **Cel:** Zarządzanie listą pojazdów (klientów).  
- Każdy rekord pojazdu zawiera:  
  - Imię i nazwisko właściciela  
  - Marka i model pojazdu  
  - Numer rejestracyjny / VIN  
  - Dane kontaktowe   
- Dostępne akcje:  
  - **Dodaj Pojazd (Nowy Klient)**  
  - **Edytuj Dane Pojazdu**  
  - **Usuń Pojazd**  
  - **Wyszukaj / Filtruj** według numeru rejestracyjnego, właściciela lub VIN  

> W AutoMagik „Klient” jest synonimem „Pojazdu” — każdy wpis pojazdu reprezentuje jednego klienta.

![Panel Samochodów / Klientów](/screenshots/manager-cars.jpg)

### 4. Menedżer: Panel Zadań / Pracowników

- **Cel:** Tworzenie, przydzielanie i monitorowanie zadań serwisowych.  
- **Pola Zadania:**  
  - ID Zadania (generowane automatycznie)  
  - Pojazd (powiązany z sekcją Samochody/Klienci)  
  - Przypisani Pracownicy  
  - Opis Zadania (np. „Wymiana oleju”, „Kontrola hamulców”)  
  - Priorytet (Niski / Średni / Wysoki)  
  - Status (Oczekujące / W toku / Zakończone)  
  - Szacowana Data Zakończenia  
- Dostępne akcje:  
  - **Utwórz Nowe Zadanie**  
  - **Edytuj Istniejące Zadanie**  
  - **Przydziel / Przypisz na nowo Pracowników**  
  - **Usuń Zadanie**  
  - **Wyświetl Szczegóły Zadania i Komentarze**  
- **Zarządzanie Pracownikami:**  
  - Przegląd listy wszystkich aktywnych Pracowników i ich statusu dostępności  
  - Aktywacja / Dezaktywacja kont  
  - Resetowanie haseł lub zmiana ról użytkowników  

![Panel Zadań / Pracowników](/screenshots/manager-tasks.jpg)

### 5. Pracownik: Panel Zadań

- **Cel:** Umożliwienie Pracownikowi przeglądania i aktualizacji przypisanych zadań.  
- **Wyświetlane Szczegóły Zadania:**  
  - Informacje o Pojeździe (marka, model, numer rejestracyjny)  
  - Opis Zadania i Priorytet  
  - Termin Wykonania  
  - Komentarze od Menedżera lub innych Pracowników  
- **Dostępne akcje Pracownika:**  
  - **Aktualizuj Status Zadania** (np. oznacz jako „W toku”)  
  - **Dodaj Komentarz** (aktualizacje postępu, pytania)  
  - **Oznacz Zadanie jako Zakończone**  
  - **Przeglądaj Historyczne Komentarze** w celu uzyskania kontekstu

![Panel Zadań Pracownika](/screenshots/worker-tasks.jpg)

---

## Instalacja

### Wymagania Wstępne
Zanim zainstalujesz i zbudujesz AutoMagik, upewnij się, że na systemie masz zainstalowane:

1. **Qt 6.x (Qt Widgets / Qt Core / Qt Network / Qt SQL)**  
   - Pobierz z [qt.io](https://www.qt.io/download).  
   - Upewnij się, że binaria deweloperskie Qt (qmake / wsparcie CMake) znajdują się w Twojej zmiennej PATH.

2. **Kompilator zgodny z C++17**  
   - Windows: MSVC 2019 lub nowszy (w ramach Visual Studio)  
   - macOS: Xcode (12.0 lub nowszy)  
   - Linux: GCC 9.0 lub nowszy / Clang 10.0 lub nowszy

3. **CMake 3.16+ (opcjonalnie)**  
   - Jeżeli wolisz kompilować za pomocą CMake zamiast qmake.

4. **Git**  
   - Do klonowania repozytorium i zarządzania wersjami.

5. **Konto Firebase i Projekt**  
   - Utwórz projekt Firebase na [console.firebase.google.com](https://console.firebase.google.com).  
   - Włącz **Realtime Database** (lub Firestore, jeśli dostosujesz kod).  
   - Wygeneruj fragment konfiguracji Web/App (Auth Domain, Database URL).  
   - Pobierz plik `google-services.json` (Android) lub `GoogleService-Info.plist` (iOS) jeśli planujesz testy na tych platformach — w przeciwnym razie potrzebujemy jedynie poświadczeń REST do klienta desktopowego.

6. **RapidJSON (lub nlohmann/json)**  
   - Do parsowania JSON-ów (integracja z Firebase REST).  
   - Projekt zawiera kopię RapidJSON w folderze `third_party/rapidjson/` domyślnie.

---
### Klonowanie i Konfiguracja
1. **Klonuj Repozytorium**  
   Otwórz terminal:
   ```bash
   git clone https://github.com/<twoja-nazwa-użytkownika>/AutoMagik.git
   cd AutoMagik
   ```

2. **Skonfiguruj Poświadczenia Firebase**					
   Zmień nazwę pliku config/firebase.example.json na config/firebase.json.
   Otwórz config/firebase.json i wklej dane swojego projektu Firebase:
       authDomain
       databaseURL
       projectId
       storageBucket
       messagingSenderId
       appId

Przykład:
```json
{
  "authDomain": "your-project.firebaseapp.com",
  "databaseURL": "https://your-project.firebaseio.com",
  "projectId": "your-project",
  "storageBucket": "your-project.appspot.com",
  "messagingSenderId": "1234567890",
  "appId": "1:1234567890:web:abcdef123456"
}
```

### Kompilacja na Windows/macOS/Linux
Używając Qt Creator:
    1. Otwórz plik AutoMagik.pro w Qt Creator.
    2. Skonfiguruj zestaw kompilacyjny dla swojego systemu (np. MinGW, MSVC, Clang).
    3. Kliknij Build > Run.

### Uruchamianie Aplikacji

Po skompilowaniu możesz uruchomić aplikację bezpośrednio:

- **Z Qt Creator:**
    - Naciśnij Run.
- **Z terminala:**
  - Na Linux/macOS:
    ```sh./AutoMagik```
  - Na Windows:
   ```shAutoMagik.exe```

Upewnij się, że Twoje poświadczenia i konfiguracja Firebase są poprawnie ustawione. Przy pierwszym uruchomieniu zaloguj się przy użyciu podanych danych menedżera.

---

## Keywords 
Qt C++ Zarządzanie Warsztatem Samochodowym Aplikacja Desktopowa Firebase Qt6 Motoryzacja Śledzenie Zadań Mechanik Zarządzanie Pracownikami Menedżer
AutoMagik System Zleceń Naprawczych Warsztat Mechanika Cross-Platformowy GUI CRM

