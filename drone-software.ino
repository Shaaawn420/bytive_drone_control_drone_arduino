#include <ESC.h>
#include <SoftwareSerial.h>

// Serial Msg Example: L,+,-,1025000

#define rxPin 10
#define txPin 11
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);


// Speed/PWM values for the ESCs
const long SPEED_MIN = 1000;
const long SPEED_MAX = 2000;
const long SPEED_NEU = 1500;
const long VR_VAL = 1500;

// Start and end values for the value mapping
const long INPUT_START = -1000;
const long INPUT_END = 1000;
const long OUTPUT_START = 1000;
const long OUTPUT_END = 2000;

// String for holding incomming serial chars
// String for chosing stick sight (L or R)
// String for x and y direction of the Stick (+ or -)
// String for combined values out of serial Msg
String readString, side, x_dir, y_dir, value;

// Long int for raw x and y value, Long int for translated and mapped x and y value
long x, y, x_t, y_t;

// Initializing ESCs to simulate the reciever
ESC Ry (11, SPEED_MIN, SPEED_MAX, 500);
ESC Rx (10, SPEED_MIN, SPEED_MAX, 500);
ESC Lx (6, SPEED_MIN, SPEED_MAX, 500);
ESC Ly (5, SPEED_MIN, SPEED_MAX, 500);
ESC VR (3, SPEED_MIN, SPEED_MAX, 500);

// Sets the both controll stick to their neutral position
void sticksNeutral() {
  Rx.speed(SPEED_NEU);
  Ry.speed(SPEED_NEU);
  Lx.speed(SPEED_NEU);
  Ly.speed(SPEED_NEU);
}

// Recreates needed stick-"combo" to tell the flight controller to start the motors
void startMotors() {
  Serial.println("Starting motors"); // DEBUG
  Rx.speed(2000);
  Ry.speed(1000);
  Lx.speed(1000);
  Ly.speed(1000);
  delay(1000);
  sticksNeutral();
  Serial.println("Motors should be spinning now"); // DEBUG
}

// Function for updating the ESCs with new PWM values
void updateESCs(String side, long x_val, long y_val) {
  // Check what stick side is being spoken to
  if (side == "L") {
    // Set ESCs to appropiat values
    Serial.println("Setting stick Lx on value: " + (String) x_val);  // DEBUG
    Lx.speed(x_val);
    Serial.println("Setting stick Ly on value: " + (String) y_val);  // DEBUG
    Ly.speed(y_val);
  } else if (side == "R") {
    Serial.println("Setting stick Rx on value: " + (String) x_val);  // DEBUG
    Rx.speed(x_val);
    Serial.println("Setting stick Ry on value: " + (String) y_val);  // DEBUG
    Ry.speed(y_val);
  } else if (side == "S") {
    startMotors();
  } else {
    // Error message for debugging if the given side is invalid
    Serial.println("Invalid stick side!");
  }
}


// Function for getting the x and y value out of the combined transmitted value. Div 1024 without rest for x, mod 1024 for y
int get_value(char dir, long value) {
  if (dir == 'x') {
    return (long) value / 1024;
  } else if (dir == 'y') {
    return value % 1024;
  }
}

// Maps values to the needed numeric range for the ESC's
long map_values(long value) {
  return map(value, INPUT_START, INPUT_END, OUTPUT_START, OUTPUT_END);
}

// Makes a number negativ again since the compressing "algorithm" cant be used with negativ values
long convert_minus(long val, String sign) {
  if (sign == "-") {
    return val * -1;
  } else {
    return val;
  }
}

void setup() {
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(3, OUTPUT);
  Serial.begin(57600);
  mySerial.begin(57600);
  Serial.println("Moin");

  VR.speed(VR_VAL);  // Setting flight controller into "Manual"-mode
  sticksNeutral();

}

void loop() {
  if (mySerial.available() > 0) {  // Read avaible chars in serial buffer and add them to readString
    char c = mySerial.read();
    readString += c;
  }

  // Check if readString is filled and contrains a '\n' to indicate the line end
  if (readString.length() > 0 && readString.indexOf("\n") > 0) {
    Serial.println(readString);

    // Create substring from the complete serial msg
    side = readString.substring(0, 1);
    x_dir = readString.substring(2, 3);
    y_dir = readString.substring(4, 5);
    value = readString.substring(6, readString.length());

    // Extract x and y value from conbined z value
    x = get_value('x', value.toInt());
    y = get_value('y', value.toInt());

    // Convert and map x and y value for use with the ESCs
    x_t = map_values(convert_minus(x, x_dir));
    y_t = map_values(convert_minus(y, y_dir));

    updateESCs(side, x_t, y_t);

    readString = "";
  }
}
