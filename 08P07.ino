// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300.0   // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL) // coefficent to convert duration to distance

unsigned long last_sampling_time = 0;   // unit: msec

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  // sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);   // sonar ECHO
  digitalWrite(PIN_TRIG, LOW);  // turn-off Sonar 

  Serial.begin(57600);
}

void loop() {
  // wait until next sampling time
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  float distance = USS_measure(PIN_TRIG, PIN_ECHO); // read distance

  // constrain distance
  if (distance < _DIST_MIN) distance = _DIST_MIN;
  if (distance > _DIST_MAX) distance = _DIST_MAX;

  // calculate LED brightness (active low)
  int ledVal;
  if (distance <= 150) {              // 100~150mm
    ledVal = map(distance, 100, 150, 255, 128); // 100->255(off), 150->128(50%)
  } else if (distance <= 200) {       // 150~200mm
    ledVal = map(distance, 150, 200, 128, 0);   // 150->128(50%), 200->0(max bright)
  } else if (distance <= 250) {       // 200~250mm
    ledVal = map(distance, 200, 250, 0, 128);   // 200->0(max bright), 250->128(50%)
  } else {                             // 250~300mm
    ledVal = map(distance, 250, 300, 128, 255); // 250->128(50%), 300->255(off)
  }

  analogWrite(PIN_LED, ledVal); // LED brightness control

  // output distance to serial
  Serial.print("distance: ");
  Serial.println(distance);

  // update last sampling time
  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}
