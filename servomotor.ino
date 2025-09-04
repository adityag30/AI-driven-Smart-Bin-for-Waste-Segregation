#include <ESP32Servo.h>

Servo myservo;  // Create servo object
int servoPin = 15;  // GPIO15 for servo signal (check your ESP32-CAM pinout)
int pos = 0;        // Servo position variable
bool increasing = true;  // Direction flag

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  // Attach servo to pin
  myservo.setPeriodHertz(50);    // Standard 50Hz servo
  myservo.attach(servoPin, 500, 2500); // Min and max pulse width (in microseconds)
}

void loop() {
  // Move servo from 0 to 90 and back
  if (increasing) {
    pos++;
    if (pos >= 90) {
      increasing = false;
    }
  } else {
    pos--;
    if (pos <= 0) {
      increasing = true;
    }
  }
  
  // Write position to servo
  myservo.write(pos);
  
  // Small delay for smooth movement
  delay(15);  // Adjust for desired speed (15ms for smooth rotation)
}
