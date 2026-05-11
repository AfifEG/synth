const int BUTTON_PIN = 2;
const int BUZZER_PIN = 8;
const int TRIG_PIN = 9;
const int ECHO_PIN = 10;

int maxDistance = 20;

// סולמות
int cMajor[]     = {262, 294, 330, 349, 392, 440, 494, 523, 587, 659};
int cMinor[]     = {262, 294, 311, 349, 392, 415, 466, 523, 587, 622};
int pentatonic[] = {262, 294, 330, 392, 440, 523, 587, 659, 784, 880};

int* scales[]    = {cMajor, cMinor, pentatonic};
String scaleNames[] = {"C Major", "C Minor", "Pentatonic"};
int numScales    = 3;
int currentScale = 0;
int numNotes     = 10;

// מעקב לחיצות
bool lastButtonState   = HIGH;
bool buttonState       = HIGH;
unsigned long pressTime    = 0;
unsigned long releaseTime  = 0;
int clickCount         = 0;
bool waitingForDouble  = false;

const int SHORT_PRESS  = 250;  // מתחת ל-250ms = לחיצה קצרה
const int DOUBLE_WAIT  = 400;  // זמן המתנה לדאבל קליק

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
  // פידבק קולי - מנגן תו אחד קצר להודיע על החלפה
  tone(BUZZER_PIN, scales[currentScale][0], 150);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(9600);
  Serial.println("Scale: C Major");
}

int lastNoteIndex = -1;

void loop() {
  buttonState = digitalRead(BUTTON_PIN);
  unsigned long now = millis();

  // זיהוי לחיצה
  if (lastButtonState == HIGH && buttonState == LOW) {
    pressTime = now;
  }

  // זיהוי שחרור
  if (lastButtonState == LOW && buttonState == HIGH) {
    unsigned long pressDuration = now - pressTime;
    releaseTime = now;

    if (pressDuration < SHORT_PRESS) {
      // לחיצה קצרה - מחכים לראות אם יש עוד אחת
      clickCount++;
      waitingForDouble = true;
    }
  }

  // בדיקה אם עבר הזמן לדאבל קליק
  if (waitingForDouble && (now - releaseTime > DOUBLE_WAIT)) {
    if (clickCount >= 2) {
      changeScale();
      Serial.print("Scale: ");
      Serial.println(scaleNames[currentScale]);
    }
    clickCount = 0;
    waitingForDouble = false;
  }

  // נגינה - רק כשהכפתור לחוץ לחיצה ארוכה
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

  } else if (buttonState == HIGH && !waitingForDouble) {
    noTone(BUZZER_PIN);
    lastNoteIndex = -1;
  }

  lastButtonState = buttonState;
  delay(20);
}
