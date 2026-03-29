/**
 * @file lab1.ino
 * @brief Laboratorijska vježba 1 - obrada prekida na ESP32 platformi.
 *
 * Sustav demonstrira obradu više izvora događaja:
 * - vanjski prekidi s tri tipkala
 * - periodički timer prekid
 * - očitanje udaljenosti pomoću HC-SR04 senzora
 *
 * Događaji se obrađuju prema unaprijed definiranim prioritetima:
 * 1. timer
 * 2. high tipkalo
 * 3. medium tipkalo
 * 4. low tipkalo
 *
 * Alarm udaljenosti aktivira se kada je objekt bliže od zadanog praga.
 * ISR rutine su kratke i koriste zastavice, a obrada se izvršava u glavnoj petlji.
 */
const int LED_HIGH  = 18;
const int LED_MED   = 19;
const int LED_LOW   = 21;
const int LED_TIMER = 22;
const int LED_ALERT = 23;

const int BTN_HIGH = 25;
const int BTN_MED  = 26;
const int BTN_LOW  = 27;

const int TRIG_PIN = 5;
const int ECHO_PIN = 17;

const float ALERT_DISTANCE = 15.0;
const unsigned long debounceMs = 200;

volatile bool flagHigh = false;
volatile bool flagMed = false;
volatile bool flagLow = false;
volatile bool flagTimer = false;

volatile unsigned long lastHighTime = 0;
volatile unsigned long lastMedTime  = 0;
volatile unsigned long lastLowTime  = 0;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
hw_timer_t *timer1 = NULL;

/**
 * @brief ISR za tipkalo visokog prioriteta.
 */
void IRAM_ATTR isrHigh() {
  unsigned long now = millis();
  if (now - lastHighTime > debounceMs) {
    portENTER_CRITICAL_ISR(&mux);
    flagHigh = true;
    lastHighTime = now;
    portEXIT_CRITICAL_ISR(&mux);
  }
}

/**
 * @brief ISR za tipkalo srednjeg prioriteta.
 */
void IRAM_ATTR isrMed() {
  unsigned long now = millis();
  if (now - lastMedTime > debounceMs) {
    portENTER_CRITICAL_ISR(&mux);
    flagMed = true;
    lastMedTime = now;
    portEXIT_CRITICAL_ISR(&mux);
  }
}

/**
 * @brief ISR za tipkalo niskog prioriteta.
 */
void IRAM_ATTR isrLow() {
  unsigned long now = millis();
  if (now - lastLowTime > debounceMs) {
    portENTER_CRITICAL_ISR(&mux);
    flagLow = true;
    lastLowTime = now;
    portEXIT_CRITICAL_ISR(&mux);
  }
}

/**
 * @brief ISR za periodički timer događaj.
 */
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&mux);
  flagTimer = true;
  portEXIT_CRITICAL_ISR(&mux);
}

/**
 * @brief Mjeri udaljenost pomoću HC-SR04 senzora.
 * @return Udaljenost u centimetrima ili -1 ako nema valjanog očitanja.
 */
float readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration <= 0) {
    return -1;
  }

  return duration * 0.0343 / 2.0;
}

/**
 * @brief Obrada timer događaja.
 */
void handleTimerEvent() {
  Serial.println("TIMER interrupt - prioritet 1");
  digitalWrite(LED_TIMER, HIGH);
  delay(150);
  digitalWrite(LED_TIMER, LOW);
}

/**
 * @brief Obrada događaja visokog prioriteta.
 */
void handleHighEvent() {
  Serial.println("HIGH interrupt - prioritet 2");
  digitalWrite(LED_HIGH, HIGH);
  delay(300);
  digitalWrite(LED_HIGH, LOW);
}

/**
 * @brief Obrada događaja srednjeg prioriteta.
 */
void handleMedEvent() {
  Serial.println("MED interrupt - prioritet 3");
  digitalWrite(LED_MED, HIGH);
  delay(300);
  digitalWrite(LED_MED, LOW);
}

/**
 * @brief Obrada događaja niskog prioriteta.
 */
void handleLowEvent() {
  Serial.println("LOW interrupt - prioritet 4");
  digitalWrite(LED_LOW, HIGH);
  delay(300);
  digitalWrite(LED_LOW, LOW);
}

/**
 * @brief Obrada alarma blizine.
 * @param distance Trenutno izmjerena udaljenost u centimetrima.
 */
void handleAlertEvent(float distance) {
  Serial.print("ALARM - objekt preblizu: ");
  Serial.print(distance);
  Serial.println(" cm");
  digitalWrite(LED_ALERT, HIGH);
}

/**
 * @brief Inicijalizacija pinova, prekida i timer-a.
 */
void setup() {
  Serial.begin(115200);

  pinMode(LED_HIGH, OUTPUT);
  pinMode(LED_MED, OUTPUT);
  pinMode(LED_LOW, OUTPUT);
  pinMode(LED_TIMER, OUTPUT);
  pinMode(LED_ALERT, OUTPUT);

  pinMode(BTN_HIGH, INPUT_PULLUP);
  pinMode(BTN_MED, INPUT_PULLUP);
  pinMode(BTN_LOW, INPUT_PULLUP);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(BTN_HIGH), isrHigh, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_MED), isrMed, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_LOW), isrLow, FALLING);

  timer1 = timerBegin(1000000);
  timerAttachInterrupt(timer1, &onTimer);
  timerAlarm(timer1, 1000000, true, 0);
}

/**
 * @brief Glavna petlja programa.
 *
 * U petlji se preuzimaju zastavice postavljene u ISR rutinama,
 * obrađuju prema prioritetima i provjerava stanje senzora udaljenosti.
 */
void loop() {
  bool h = false;
  bool m = false;
  bool l = false;
  bool t = false;

  portENTER_CRITICAL(&mux);
  h = flagHigh;
  m = flagMed;
  l = flagLow;
  t = flagTimer;

  flagHigh = false;
  flagMed = false;
  flagLow = false;
  flagTimer = false;
  portEXIT_CRITICAL(&mux);

  if (t) {
    handleTimerEvent();
  }
  else if (h) {
    handleHighEvent();
  }
  else if (m) {
    handleMedEvent();
  }
  else if (l) {
    handleLowEvent();
  }

  float distance = readDistanceCm();

  if (distance > 0 && distance < ALERT_DISTANCE) {
    handleAlertEvent(distance);
  } else {
    digitalWrite(LED_ALERT, LOW);
  }

  delay(100);
}