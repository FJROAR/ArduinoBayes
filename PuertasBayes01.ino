#include <LiquidCrystal.h>

#define LED_PIN_1 13
#define LED_PIN_2 12
#define LED_PIN_3 11

#define BUTTON_CHOOSE 4
#define BUTTON_CHANGE 5

#define LCD_RS_PIN A5
#define LCD_E_PIN A4
#define LCD_D4_PIN 6
#define LCD_D5_PIN 7
#define LCD_D6_PIN 8
#define LCD_D7_PIN 9

LiquidCrystal lcd(LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN,
                  LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

// ---------------- CONFIG ----------------
const unsigned long LED_INTERVAL = 2000;
const unsigned long DEBOUNCE_TIME = 50;

// ---------------- ESTADOS ----------------
enum GameState {
  WAIT_CHOOSE,
  FINAL_CHOICE,
  RESULT
};

GameState state = WAIT_CHOOSE;

// ---------------- VARIABLES ----------------
int premio;
int puertaUsuario;
int puertaRevelada;
int puertaFinal;

int ledActual = 1;
unsigned long lastLedTime = 0;

// Debounce
bool chooseState = LOW, lastChooseReading = LOW;
bool changeState = LOW, lastChangeReading = LOW;
unsigned long lastChooseTime = 0;
unsigned long lastChangeTime = 0;

// ---------------- LEDS ----------------
void apagarLeds() {
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);
}

void encenderLed(int p) {
  apagarLeds();
  if (p == 1) digitalWrite(LED_PIN_1, HIGH);
  if (p == 2) digitalWrite(LED_PIN_2, HIGH);
  if (p == 3) digitalWrite(LED_PIN_3, HIGH);
}

// ---------------- DEBOUNCE ----------------
bool debounceButton(int pin, bool &stableState, bool &lastReading, unsigned long &lastTime) {
  bool reading = digitalRead(pin);

  if (reading != lastReading) {
    lastTime = millis();
  }

  if ((millis() - lastTime) > DEBOUNCE_TIME) {
    if (reading != stableState) {
      stableState = reading;
      if (stableState == HIGH) {
        lastReading = reading;
        return true;
      }
    }
  }

  lastReading = reading;
  return false;
}

// ---------------- RESET ----------------
void resetJuego() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting");

  premio = random(1, 4);
  ledActual = 1;
  lastLedTime = millis();

  apagarLeds();
  state = WAIT_CHOOSE;
}

// ---------------- SETUP ----------------
void setup() {
  lcd.begin(16, 2);

  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);

  pinMode(BUTTON_CHOOSE, INPUT);
  pinMode(BUTTON_CHANGE, INPUT);

  randomSeed(analogRead(A0));

  resetJuego();
}

// ---------------- LOOP ----------------
void loop() {

  if (state == WAIT_CHOOSE) {

    if (millis() - lastLedTime >= LED_INTERVAL) {
      lastLedTime = millis();
      encenderLed(ledActual);
      ledActual++;
      if (ledActual > 3) ledActual = 1;
    }

    if (debounceButton(BUTTON_CHOOSE, chooseState, lastChooseReading, lastChooseTime)) {

      puertaUsuario = ledActual - 1;
      if (puertaUsuario == 0) puertaUsuario = 3;

      encenderLed(puertaUsuario); // LED elegido queda encendido

      int opciones[2];
      int n = 0;
      for (int i = 1; i <= 3; i++) {
        if (i != puertaUsuario && i != premio) {
          opciones[n++] = i;
        }
      }
      puertaRevelada = opciones[random(n)];

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("No esta en ");
      lcd.print(puertaRevelada);

      state = FINAL_CHOICE;
    }
  }

  else if (state == FINAL_CHOICE) {

    if (debounceButton(BUTTON_CHOOSE, chooseState, lastChooseReading, lastChooseTime)) {
      puertaFinal = puertaUsuario;
      state = RESULT;
    }

    if (debounceButton(BUTTON_CHANGE, changeState, lastChangeReading, lastChangeTime)) {
      for (int i = 1; i <= 3; i++) {
        if (i != puertaUsuario && i != puertaRevelada) {
          puertaFinal = i;
        }
      }
      state = RESULT;
    }
  }

  else if (state == RESULT) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Premio en ");
    lcd.print(premio);

    lcd.setCursor(0, 1);
    lcd.print(puertaFinal == premio ? "Acierto" : "Fracaso");

    encenderLed(premio);

    if (debounceButton(BUTTON_CHOOSE, chooseState, lastChooseReading, lastChooseTime) ||
        debounceButton(BUTTON_CHANGE, changeState, lastChangeReading, lastChangeTime)) {
      resetJuego();
    }
  }
}
