#include <Arduino.h>

const int LEDS[] = {3, 5, 6, 9, 10};   // 5 LEDs
const int N_LEDS = sizeof(LEDS) / sizeof(LEDS[0]);
const int BUZZ = 8;                    // buzzer pin

String buf; // holds incoming serial data

void setup() {
    Serial.begin(9600);
    for (int i = 0; i < N_LEDS; i++) pinMode(LEDS[i], OUTPUT);
    pinMode(BUZZ, OUTPUT);

  // startup blink
    for (int i = 0; i < N_LEDS; i++) { digitalWrite(LEDS[i], HIGH); delay(80); }
    for (int i = 0; i < N_LEDS; i++) { digitalWrite(LEDS[i], LOW);  delay(80); }

  Serial.println("READY");
}

void setBar(int n) {
  for (int i = 0; i < N_LEDS; i++) digitalWrite(LEDS[i], (i < n) ? HIGH : LOW);
}

void okChirp() {
  digitalWrite(BUZZ, HIGH); delay(100);
  digitalWrite(BUZZ, LOW);  delay(100);
  digitalWrite(BUZZ, HIGH); delay(80);
  digitalWrite(BUZZ, LOW);
}

void fistAlert() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < N_LEDS; j++) digitalWrite(LEDS[j], HIGH);
    digitalWrite(BUZZ, HIGH); delay(100);
    for (int j = 0; j < N_LEDS; j++) digitalWrite(LEDS[j], LOW);
    digitalWrite(BUZZ, LOW); delay(100);
  }
}

void handleCmd(const String &cmd) {
  if (cmd == "PING") {
    Serial.println("PONG");
    return;
  }

  if (cmd.startsWith("F")) {           // e.g. F3
    int n = cmd.substring(1).toInt();
    n = constrain(n, 0, N_LEDS);
    setBar(n);
    Serial.print("ACK F"); Serial.println(n);
    return;
  }

  if (cmd.startsWith("CMD:")) {
    String what = cmd.substring(4);
    if (what == "OPEN") {              // smooth fade pattern
      for (int v = 0; v <= 255; v += 5) { analogWrite(LEDS[0], v); delay(5); }
      for (int v = 255; v >= 0; v -= 5) { analogWrite(LEDS[0], v); delay(5); }
      Serial.println("ACK OPEN");
    } else if (what == "FIST") {
      fistAlert();
      Serial.println("ACK FIST");
    } else if (what == "OK") {
      okChirp();
      Serial.println("ACK OK");
    } else if (what.startsWith("BRIGHT:")) { // brightness control
      int val = what.substring(7).toInt();
      analogWrite(LEDS[1], constrain(val, 0, 255));
      Serial.println("ACK BRIGHT");
    }
    return;
  }

  Serial.println("ERR");
}

void loop() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (buf.length() > 0) {
        handleCmd(buf);
        buf = "";
      }
    } else {
      if (buf.length() < 64) buf += c;
    }
  }
}