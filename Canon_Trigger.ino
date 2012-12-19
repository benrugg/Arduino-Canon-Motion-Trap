
// The timing for the pulses used in this sketch is based on:
// http://www.doc-diy.net/photo/rc-1_hacked/
// http://arduino.cc/forum/index.php/topic,11167.0.html
// And the pulseIR() function is based on the great tutorial at:
// http://learn.adafruit.com/ir-sensor/making-an-intervalometer

// It runs at 32.6 kHz

// It uses two PIR motion sensors.
// It uses a four way switch (SP4T) to select which type of operation
// (motion sensor A, motion sensor B, either or both).
// And it uses a potentiometer to adjust the speed of pictures



// IR LED connected to digital pin 12
int IRledPin = 12;


// indicator LED connected to digital pin 13
int indicatorLedPin = 13;


// four way switch connected to digital pins 2-5
int modeSelectionPin1 = 5;
int modeSelectionPin2 = 4;
int modeSelectionPin3 = 3;
int modeSelectionPin4 = 2;


// motion sensor A connected to digital pin 6
int motionSensorPinA = 6;


// motion sensor B connected to digital pin 7
int motionSensorPinB = 7;


// potentiometer connected to ANALOG pin 2
int potentiometerSensorPin = 2;


// potentiometer voltage in connected to digital pin 8 (so we only have to run power
// through the potentiometer when we want to know its value)
int potentiometerVoltagePin = 8;


// true/false for debugging
boolean isDebugging = false;



// signal to tell the canon camera to take a picture immediately
int CanonSignal[] = {
// ON, OFF (in 10's of microseconds)
        50, 730,
        50, 730,
        50, 730,
        50, 730,
        50, 730,
        50, 730,
        50, 730,
        50, 730,
        50, 730,
        50, 730,
        50, 730,
        50, 730,
        50, 730,
        50, 730,
        50, 730,
        50, 0};



void setup() {
  
  // all pins are set as inputs by default, so we only need to set outputs
  
  
  // initialize the IR digital pin as an output
  pinMode(IRledPin, OUTPUT);
  
  
  // initialize the potentiometer voltage pin as an output
  pinMode(potentiometerVoltagePin, OUTPUT);
  
  
  // if we're debugging, start the serial port
  if (isDebugging) Serial.begin(9600);
}



void loop() {
  
  // determine which mode we're in
  int operationMode = determineMode();
  
  
  // initialize the variable for determining if we're going to take
  // a picture right now
  boolean doTakePicture = false;
  
  
  // if we only care about motion sensor A and it's showing motion, set the flag
  if (operationMode == 1 && digitalRead(motionSensorPinA) == HIGH) {
    
    doTakePicture = true;
    
    
  // else, if we only care about motion sensor B and it's showing motion, set the flag
  } else if (operationMode == 2 && digitalRead(motionSensorPinB) == HIGH) {
    
    doTakePicture = true;
    
    
  // else, if we only care about either sensor and one of them is showing motion, set the flag
  } else if (operationMode == 3 && (digitalRead(motionSensorPinA) == HIGH || digitalRead(motionSensorPinB) == HIGH)) {
    
    doTakePicture = true;
    
    
  // else, if we care about both sensors and they're both showing motion, set the flag
  } else if (operationMode == 4 && digitalRead(motionSensorPinA) == HIGH && digitalRead(motionSensorPinB) == HIGH) {
    
    doTakePicture = true;
  }
  
  
  // if we're debugging, output the values of the sensors
  if (isDebugging) {
    
    Serial.print("sensor A: ");
    Serial.print(digitalRead(motionSensorPinA));
    Serial.print(", sensor B: ");
    Serial.println(digitalRead(motionSensorPinB));
  }
  
  
  
  // if we want to take a picture now, send the code and then figure out how long to wait
  if (doTakePicture) {
    
    // if we're debugging, output a message
    if (isDebugging) Serial.println("picture!");
    
    
    // turn on the indicator LED
    digitalWrite(indicatorLedPin, HIGH);
    
    
    // send the code
    sendCode(CanonSignal, sizeof(CanonSignal) / sizeof(int));
    
    
    // turn the potentiometer pin voltage high
    digitalWrite(potentiometerVoltagePin, HIGH);
    
    
    // read the potentiometer value (and limit it so we can somewhat control
    // how sensitive the dial is)
    int potentiometerValue = constrain(analogRead(potentiometerSensorPin), 15, 390);
    
    
    // convert the value from the input voltage range to the output time range
    int delayTime = (int)map(potentiometerValue, 15, 390, 20000, 500);
    
    
    // turn the potentiometer pin voltage low
    digitalWrite(potentiometerVoltagePin, LOW);
    
    
    // delay for a moment to let the indicator LED be lit
    delay(200);
    
    
    // turn the indicator LED off
    digitalWrite(indicatorLedPin, LOW);
    
    
    // if we're debugging, output the value of the time we're going to delay
    if (isDebugging) {
      
      Serial.println(delayTime);
      delay(500);
      
      
    // else, wait amount of time we want to wait
    } else {
      
      delay(delayTime - 200);
    }
    
    
  // else, we don't want to take a picture now, so wait a moment before checking everything again
  } else {
    
    delay(200);
  }
}



int determineMode() {
  
  // initialize the variable we'll use (with a default in case
  // we can't read the switch for some reason)
  int mode = 3;
  
  
  // set all four pins on the fourway switch to input_pullup mode
  pinMode(modeSelectionPin1, INPUT_PULLUP);
  pinMode(modeSelectionPin2, INPUT_PULLUP);
  pinMode(modeSelectionPin3, INPUT_PULLUP);
  pinMode(modeSelectionPin4, INPUT_PULLUP);
  
  
  // figure out which pin is now connected to ground
  if (digitalRead(modeSelectionPin1) == LOW) {
    
    mode = 1;
    
  } else if (digitalRead(modeSelectionPin2) == LOW) {
    
    mode = 2;
    
  } else if (digitalRead(modeSelectionPin3) == LOW) {
    
    mode = 3;
    
  } else if (digitalRead(modeSelectionPin4) == LOW) {
    
    mode = 4;
  }
  
  
  // set all pins back to input mode (so they won't draw current
  // when we don't need them)
  pinMode(modeSelectionPin1, INPUT);
  pinMode(modeSelectionPin2, INPUT);
  pinMode(modeSelectionPin3, INPUT);
  pinMode(modeSelectionPin4, INPUT);
  
  
  // if we're debugging, output the mode
  if (isDebugging) {
    
    Serial.print("mode: ");
    Serial.println(mode);
  }
  
  
  // return the mode
  return mode;
}



void sendCode(int signalArray[], int arraySize) {
  
  for (int i = 0; i < arraySize; i += 2) {
    
    // send the "on" signal
    pulseIR((long)signalArray[i] * 10);
    
    
    // then delay for the "off" signal (using either delayMicroseconds() or
    // delay() because delayMicroseconds() isn't accurate past 16383 microseconds)
    unsigned int delayTime = (unsigned int)signalArray[i + 1] * 10;
    
    if (delayTime <= 16383) {
      
      delayMicroseconds(delayTime);
      
    } else {
      
      delay(delayTime / 1000);
    }
  }
}



// This procedure sends a 32.6 kHz pulse to the IRledPin 
// for a certain # of microseconds. We'll use this whenever we need to send codes
void pulseIR(long microsecs) {
  
  // we'll count down from the number of microseconds we are told to wait
  
  cli(); // this turns off any background interrupts
  
  while (microsecs > 0) {
    
    // 32.6 kHz is about 15 microseconds high and 15 microseconds low
    digitalWrite(IRledPin, HIGH); // this takes about 3 microseconds to happen
    delayMicroseconds(12);     // hang out for 12 microseconds
    digitalWrite(IRledPin, LOW);  // this also takes about 3 microseconds
    delayMicroseconds(12);     // hang out for 12 microseconds
    
    // so 30 microseconds altogether
    microsecs -= 30;
  }
  
  sei();// this turns them back on
}
