const int BUTTON_PIN = 2;
const int SWITCH_PIN = 3;
const int BUZZER_PIN = 8;
const int TRIG_PIN = 9;
const int ECHO_PIN = 10;

int maxDistance = 20;

// סולמות
int cMajor[]     = {262, 294, 330, 349, 392, 440, 494, 523, 587, 659};
int cMinor[]     = {262, 294, 311, 349, 392, 415, 466, 523, 587, 622};
int pentatonic[] = {262, 294, 330, 392, 440, 523, 587, 659, 784, 880};

int* scales[]       = {cMajor, cMinor, pentatonic};
String scaleNames[] = {"C Major", "C Minor", "Pentatonic"};
int numScales       = 3;
int currentScale    = 0; // מתחיל ב-C Major
int numNotes        = 10;

// מעקב כפתור נגינה
bool lastButtonState = HIGH;
bool buttonState     = HIGH;
unsigned long pressTime = 0;
const int SHORT_PRESS   = 250;

// מעקב מיקרו סוויץ'
bool lastSwitchState = HIGH;
bool switchState     = HIGH;

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

void changeScale() {
  currentScale = (currentScale + 1) % numScales;
  tone(BUZZER_PIN, scales[currentScale][0], 150);
  Serial.print("Scale: ");
  Serial.println(scaleNames[currentScale]);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(9600);
  Serial.println("Scale: C Major");
}

int lastNoteIndex = -1;

void loop() {
  unsigned long now = millis();

  // --- מיקרו סוויץ' - החלפת סולם ---
  switchState = digitalRead(SWITCH_PIN);
  if (lastSwitchState == HIGH && switchState == LOW) {
    changeScale();
  }
  lastSwitchState = switchState;

  // --- כפתור נגינה ---
  buttonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && buttonState == LOW) {
    pressTime = now;
  }

  bool isLongPress = (buttonState == LOW) && (now - pressTime > SHORT_PRESS);

  if (isLongPress) {
    long distance = getDistance();

    if (distance > maxDistance || distance < 2) {
      lastNoteIndex = -1;
      return;
    }

    int noteIndex = map(distance, 2, maxDistance, 0, numNotes - 1);

    if (noteIndex != lastNoteIndex) {
      tone(BUZZER_PIN, scales[currentScale][noteIndex]);
      lastNoteIndex = noteIndex;
    }

  } else if (buttonState == HIGH) {
    noTone(BUZZER_PIN);
    lastNoteIndex = -1;
  }

  lastButtonState = buttonState;
  delay(20);
}
