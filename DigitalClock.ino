#define LATCH_PIN 5
#define CLOCK_PIN 6
#define DATA_PIN 4

#define HOUR_BUTTON 7
#define MINUTE_BUTTON 8
#define MODE_BUTTON 9

byte digitPatterns[] = {
  0b01111110,  // 0
  0b01001000,  // 1
  0b00111101,  // 2
  0b01101101,  // 3
  0b01001011,  // 4
  0b01100111,  // 5
  0b01110111,  // 6
  0b01001100,  // 7
  0b01111111,  // 8
  0b01001111   // 9
};

byte hour = 12;
byte min = 0;
bool pmCheck = false;

void setup() {
  Serial.begin(9600);

  // Hour and Minute Seperation Semicolon
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  // Shift Register Daisy Chain Setup
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  // Buttons
  pinMode(HOUR_BUTTON, INPUT_PULLUP);
  pinMode(MINUTE_BUTTON, INPUT_PULLUP);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
}

void loop() {
  // Output current pattern
  updateScreen(0,0);

  byte sec = 0;
  long unsigned start = millis();
  while(sec<60) {
    Serial.print(hour);
    Serial.print(":");
    Serial.print(min);
    Serial.print(".");
    Serial.println(sec);
    if(digitalRead(HOUR_BUTTON) == HIGH || digitalRead(MINUTE_BUTTON) == HIGH || digitalRead(MODE_BUTTON) == HIGH) {
      if(digitalRead(HOUR_BUTTON) == HIGH && digitalRead(MINUTE_BUTTON) == HIGH) {
        sec = 0;
        while(digitalRead(HOUR_BUTTON) == HIGH && digitalRead(MINUTE_BUTTON) == HIGH) {}
      } else if(digitalRead(HOUR_BUTTON) == HIGH) {
        updateScreen(1,0);
        while(digitalRead(HOUR_BUTTON) == HIGH) {}
      } else if(digitalRead(MINUTE_BUTTON) == HIGH) {
        updateScreen(0,1);
        while(digitalRead(MINUTE_BUTTON) == HIGH) {}
      }
    } else {
      if(millis() - start < 500) {
        digitalWrite(2, HIGH);
        digitalWrite(3, HIGH);
      } else if(millis() - start >= 500 && millis() - start < 998) {
        digitalWrite(2, LOW);
        digitalWrite(3, LOW);
      } else if(millis() - start >= 1000){
        start = millis();
        sec++;
      }
    }
  }
  min++;
}

void updateScreen(byte hPlus, byte mPlus) {
  if(hour + hPlus >= 13) {
    hour = 1;
    pmCheck = !pmCheck;
  } else { hour = hour + hPlus; }

  if(min + mPlus >= 60) { min = 0; } else { min = min + mPlus; }
  
  if (min >= 60) {
    min = 0;
    hour++;
    if(hour >= 13) {
      hour = 1;
      pmCheck = !pmCheck;
    }
  }
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, digitPatterns[hour / 10]);  //furthest from Arduino
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, digitPatterns[hour % 10]);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, digitPatterns[min / 10]);
  if(pmCheck == true) {
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, digitPatterns[min % 10] + 0b10000000);
  } else {
    byte newByte = digitPatterns[min % 10];
    newByte &= ~(1 << 7);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, newByte);
  }
  digitalWrite(LATCH_PIN, HIGH);
}