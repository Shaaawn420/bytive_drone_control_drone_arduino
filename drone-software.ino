#include <ESC.h>
#include <SoftwareSerial.h>

const long SPEED_MIN (1150);
const long SPEED_MAX (2000);
const long SPEED_NEU (1150);

ESC Ry (11, SPEED_MIN, SPEED_MAX, 500);
ESC Rx (10, SPEED_MIN, SPEED_MAX, 500);
ESC Lx (6, SPEED_MIN, SPEED_MAX, 500);
ESC Ly (5, SPEED_MIN, SPEED_MAX, 500);

#define rxPin 2
#define txPin 3
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);

String readString;

long values[] = {SPEED_MIN, SPEED_MIN, SPEED_MIN, SPEED_MIN};
ESC escs[] = {Rx, Ry, Lx, Ly};

void send_values() {
  for (int i = 0; i <= 3; i++) {
    escs[i].speed(values[i]);
  }
}

void sticksNeutral() {
  for (int i = 0; i <= 3; i++) {
    values[i] = SPEED_NEU;
  }
}

void armMotors() {
  for (int i = 0; i <= 3; i++) {
    escs[i].arm();
    delay(100);
  }
  Serial.println("Caution: motors are armed");
}

void startMotors() {
  Serial.println("Starting motors");
  armMotors();
  delay(1000);
  sticksNeutral();
  delay(500);
  Serial.println("Motors should be spinning now");
}

void updateESCvalues(String side, long x_val, long y_val) {
  if (side == "L") {
    values[2] = x_val;
    values[3] = y_val;
  } else if (side == "R") {
    values[0] = x_val;
    values[1] = y_val;
  } else if (side == "S") {
    startMotors();
  } else if (side == "C") {
    sticksNeutral();
  } else {
    Serial.println("Invalid stick side!");
    Serial.println("Side: " + side);
  }
}

long get_value(char dir, long value) {
  if (dir == 'x') {
    return (long) value / 1024;
  } else if (dir == 'y') {
    return (long) value % 1024;
  }
}

void process_message(String msg) {
    String side = readString.substring(1, 2);
    String x = readString.substring(3, 7);
    String y = readString.substring(8, 12);

    updateESCvalues(side, x.toInt(), y.toInt());
}

void setup() {
  Serial.begin(57600);
  mySerial.begin(57600);
  Serial.println("Moin");
  delay(500);
  armMotors();
  Serial.println("Done Arming");
  delay(1000);
}

void loop() {
  if (mySerial.available() > 0) {
    char c = mySerial.read();
    readString += c;
  }
  if (readString.length() > 0 && readString.indexOf("!") > 0) {
    mySerial.flush();
    Serial.println(readString);

    process_message(readString);

    readString = "";
  }

  send_values();
}
