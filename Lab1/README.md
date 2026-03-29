# Laboratorijska vježba 1 – Obrada prekida na ESP32

## Opis zadatka
Ova laboratorijska vježba demonstrira obradu više izvora događaja na mikrokontrolerskoj platformi ESP32.  
Sustav koristi:
- tri tipkala kao vanjske izvore prekida
- periodički timer prekid
- HC-SR04 ultrazvučni senzor udaljenosti
- LED indikatore za prikaz pojedinih događaja

Cilj vježbe je prikazati:
- obradu više prekida
- definiranje prioriteta događaja
- minimiziranje rada unutar ISR rutina
- korištenje zastavica i kritičnih sekcija
- praćenje redoslijeda događaja pomoću logičkog analizatora

---

## Funkcionalni opis sustava

Sustav koristi tri tipkala koja generiraju vanjske prekide:
- **HIGH** tipkalo – događaj visokog prioriteta
- **MEDIUM** tipkalo – događaj srednjeg prioriteta
- **LOW** tipkalo – događaj niskog prioriteta

Dodatno se koristi:
- **timer prekid** koji se aktivira periodički svakih 1 sekundu
- **HC-SR04 senzor** koji mjeri udaljenost objekta

Ako je izmjerena udaljenost manja od zadanog praga, aktivira se **alarmni indikator**.

---

## Prioriteti događaja

Događaji se obrađuju u glavnoj petlji prema sljedećem redoslijedu prioriteta:

1. **TIMER**
2. **HIGH**
3. **MEDIUM**
4. **LOW**

Alarm udaljenosti radi kao dodatna funkcionalnost nadzora i aktivira se kada je objekt bliže od zadanog praga.

---

## Korišteni pinovi

### LED indikatori
- `GPIO 18` – LED_HIGH
- `GPIO 19` – LED_MED
- `GPIO 21` – LED_LOW
- `GPIO 22` – LED_TIMER
- `GPIO 23` – LED_ALERT

### Tipkala
- `GPIO 25` – BTN_HIGH
- `GPIO 26` – BTN_MED
- `GPIO 27` – BTN_LOW

### HC-SR04
- `GPIO 5` – TRIG
- `GPIO 17` – ECHO

---

## Način rada programa

ISR rutine su izvedene tako da budu što kraće.  
Njihova uloga je samo:
- prepoznati događaj
- postaviti odgovarajuću zastavicu
- izaći iz prekidne rutine

Na taj način se:
- smanjuje vrijeme provedeno unutar ISR-a
- izbjegava nepotrebno blokiranje sustava
- povećava preglednost i sigurnost rada

Stvarna obrada događaja izvodi se u `loop()` funkciji.  
U glavnoj petlji se:
1. čitaju zastavice postavljene u ISR rutinama
2. brišu zastavice unutar kritične sekcije
3. obrađuje samo događaj najvišeg prioriteta
4. neprekidno provjerava stanje senzora udaljenosti

---

## Debounce mehanizam

Za tipkala je implementiran **debounce** kako bi se izbjegla višestruka aktivacija prekida pri jednom pritisku tipkala.

Za svako tipkalo prati se vrijeme zadnje aktivacije, a novi prekid se prihvaća tek ako je prošlo dovoljno vremena od prethodnog događaja.

---

## Alarm udaljenosti

HC-SR04 senzor koristi se za mjerenje udaljenosti objekta od sustava.  
Ako je udaljenost manja od zadanog praga:

- aktivira se `LED_ALERT`
- u serijski monitor ispisuje se poruka o alarmu

U ovom rješenju alarmna LED ostaje upaljena dok god traje alarmno stanje, odnosno dok je objekt unutar definiranog praga udaljenosti.

---

## Logički analizator

Za provjeru redoslijeda događaja korišten je **Wokwi Logic Analyzer**.

Na analizator su spojeni izlazi LED indikatora:

- `D0` – TIMER
- `D1` – HIGH
- `D2` – MEDIUM
- `D3` – LOW
- `D4` – ALERT

Logički analizator korišten je za promatranje redoslijeda aktivacije događaja i potvrdu prioritetne logike sustava.  
Snimka analizatora spremljena je u `.vcd` formatu.

---

## Primjer ispitivanja

Tijekom ispitivanja aktivirani su različiti događaji:
- pritisak LOW tipkala
- pritisak MED tipkala
- pritisak HIGH tipkala
- periodički TIMER događaj
- aktivacija alarma približavanjem objekta senzoru

Na temelju izlaza i logičkog analizatora može se potvrditi:
- ispravno generiranje događaja
- ispravna reakcija sustava
- definirani redoslijed prioriteta u obradi događaja

---

## Zaključak

U ovoj laboratorijskoj vježbi uspješno je implementiran sustav za obradu više izvora događaja na ESP32 platformi.

Rješenje demonstrira:
- vanjske prekide
- timer prekid
- rad sa senzorom udaljenosti
- prioritizaciju događaja
- kratke ISR rutine
- korištenje kritičnih sekcija
- osnovnu verifikaciju rada logičkim analizatorom

---

## Sadržaj direktorija Lab1
- `Lab1.ino` – izvorni kod programa
- `README.md` – opis rješenja i dokumentacija
- `diagram.json` – Wokwi shema spojeva
- `wokwi-logic.vcd` – snimka logičkog analizatora
- `docs/` – generirana Doxygen dokumentacija
