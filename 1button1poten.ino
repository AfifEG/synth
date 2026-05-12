#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int BUTTON_PIN = 2;
const int BUZZER_PIN = 9;
const int POT_NOTE   = A0;

const char* noteNames[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B","C"};
const int noteFreqs[] = {262,277,294,311,330,349,370,392,415,440,466,494,523};

int lastNote = -1;

void updateDisplay(int note) {
  lcd.setCursor(0, 0);
  lcd.print("Note:           ");
  lcd.setCursor(6, 0);
  lcd.print(noteNames[note]);
  lcd.setCursor(0, 1);
  lcd.print("Octave: 4       ");
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  Instrument!  ");
  lcd.setCursor(0, 1);
  lcd.print("   Ready...    ");
  delay(1500);
  lcd.clear();
}

void loop() {
  int raw = analogRead(POT_NOTE);
  int note = map(raw, 0, 1023, 0, 13);
  if (note > 12) note = 12;

  if (note != lastNote) {
    updateDisplay(note);
    lastNote = note;
  }

  if (digitalRead(BUTTON_PIN) == LOW) {
    tone(BUZZER_PIN, noteFreqs[note]);
  } else {
    noTone(BUZZER_PIN);
  }
}
