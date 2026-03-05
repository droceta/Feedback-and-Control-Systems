#include <Servo.h>

// Create a servo object
Servo myServo;

void setup() {
  // Attach the servo on pin 9 to the servo object
  myServo.attach(9);
  
  // Start serial communication at 9600 bits per second
  Serial.begin(9600);
  
  // Set initial position to 0
  myServo.write(0);
}

void loop() {
  // Check if data is available to read
  if (Serial.available() > 0) {
    // Read the incoming integer from the serial buffer
    int angle = Serial.parseInt();
    
    // ERROR PROOFING: Ensure the angle is within the physical limits of the servo
    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      // Optional: Send a confirmation back to Python
      Serial.print("Moved to: ");
      Serial.println(angle);
    } else {
      // Ignore invalid inputs (like 200 or -5)
      Serial.println("Invalid angle received. Must be 0-180.");
    }
  }
}