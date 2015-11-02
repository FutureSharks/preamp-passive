///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// To do:
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// If enabled volume/input data will be printed via serial
bool debugEnabled = false;

// EEPROM related stuff to save volume level
//#include "EEPROM.h"
//int dBLevelEEPROMAddressBit = 0;
//bool isVolumeSavedToEeprom = true;
//unsigned long timeOfLastVolumeChange;
//unsigned long timeBetweenVolumeSaves = 60000;
//float maximumLevelToSave = -30.0;
//float currentSavedDbLevel;

// SPI library
#include "SPI.h"
// Arduino pin 9 & 10 = Input selector & Relay attenuator
// Arduino pin 11 = SDI
// Arduino pin 13 = CLK

// IR stuff
#include "IRremote.h"
int RECV_PIN = A0;  // IR Receiver pin
const int IRGroundPin = A1;
const int IRPowerPin = A2;
IRrecv irrecv(RECV_PIN);
decode_results results;
String lastIRoperation;
float iRIncrement = 5;
unsigned long timeOfLastIRChange;

// NeoPixel stuff
#include "Adafruit_NeoPixel.h"
const int NeoPixelPin = 8;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NeoPixelPin, NEO_GRB + NEO_KHZ800);
unsigned long timeOfLastNeopixelColourChange;
byte NeoPixelRed;
byte NeoPixelGreen;
byte NeoPixelBlue;

// Input selector stuff
int selectedInput = 0;
const int inputSelectorCSPin = 9;
long muteDelay = 1000;

// Relay attenuator stuff
const int AttenuatorCSPin = 10;
byte currentAttenuatorLevel = 30;
bool muteEnabled = false;
byte lastAttenuatorLevel;

// Encoder stuff
const int encoder0GroundPin = 6;
const int encoder0PowerPin = 7;
int encoder0PinA = 5;
int encoder0PinB = 4;
int encoder0Pos = 0;
int encoder0PinALast = HIGH;
int n = LOW;
float encoderIncrement = 2;

// Button stuff
const int buttonPin = 2;

// System stuff
unsigned long timeOfLastOperation;
unsigned long timeToSleep = 120000;
bool sleeping;

//////////////////////////////////////////////////////////////////////////////////////////////
// Setup
//////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // Serial
  if (debugEnabled) {
    Serial.begin (9600);
  }
  // SPI
  // set the CS pins as output:
  pinMode (inputSelectorCSPin, OUTPUT);
  digitalWrite(inputSelectorCSPin,HIGH);
  pinMode (AttenuatorCSPin, OUTPUT);
  digitalWrite(AttenuatorCSPin,HIGH);
  // Start SPI
  if (debugEnabled) {
    Serial.println ("Starting SPI..");
  }
  SPI.begin();
  // Set SPI selector IO direction to output for all pinds
  if (debugEnabled) {
    Serial.println ("Setting SPI selector IO direction control registers..");
  }
  digitalWrite(inputSelectorCSPin,LOW);
  SPI.transfer(B01000000); // Send Device Opcode
  SPI.transfer(0); // Select IODIR register
  SPI.transfer(0); // Set register
  digitalWrite(inputSelectorCSPin,HIGH);
  // Set up pins for encoder:
  pinMode (encoder0PinA,INPUT);
  pinMode (encoder0PinB,INPUT);
  pinMode (encoder0GroundPin,OUTPUT);
  pinMode (encoder0PowerPin,OUTPUT);
  digitalWrite(encoder0GroundPin,LOW);
  digitalWrite(encoder0PowerPin,HIGH);
  // IR
  irrecv.enableIRIn(); // Start the receiver
  pinMode(IRPowerPin, OUTPUT);
  pinMode(IRGroundPin, OUTPUT);
  digitalWrite(IRPowerPin, HIGH); // Power for the IR
  digitalWrite(IRGroundPin, LOW); // GND for the IR
  // NeoPixel
  strip.begin();
  // Buton setup
  pinMode (buttonPin, INPUT_PULLUP);
  // Attenuator setup
  // Set IO direction to output for all bank 0 (I/O pins 0-7)
  digitalWrite(AttenuatorCSPin,LOW);
  SPI.transfer(B01000000); // Send Device Opcode
  SPI.transfer(0);         // Select IODIR register for bank 0
  SPI.transfer(0);         // Set register
  digitalWrite(AttenuatorCSPin,HIGH);
  // Set IO direction to output for all bank 1 (I/O pins 8-17)
  digitalWrite(AttenuatorCSPin,LOW);
  SPI.transfer(B01000000); // Send Device Opcode
  SPI.transfer(1);         // Select IODIR register for bank 1
  SPI.transfer(0);         // Set register
  digitalWrite(AttenuatorCSPin,HIGH);
  // Set inital attenuator level
  setAttenuatorLevel(currentAttenuatorLevel);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Function to set NeoPixel colour to match current DAC level
int setNeoPixelColourFromAttenuator (int level) {
  byte redPixelColor = level;
  byte bluePixelColor = 255 - redPixelColor;
  setNeoPixelColour(redPixelColor, 0, bluePixelColor);
}
//////////////////////////////////////////////////////////////////////////////////////////////
// Function to multiple NeoPixels to a colour simultaneously
int setNeoPixelColour (byte Red, byte Green, byte Blue) {
  NeoPixelRed = Red;
  NeoPixelGreen = Green;
  NeoPixelBlue = Blue;
  strip.setPixelColor(0, Red, Green, Blue);
  strip.setPixelColor(1, Red, Green, Blue);
  strip.setPixelColor(2, Red, Green, Blue);
  strip.setPixelColor(3, Red, Green, Blue);
  strip.setPixelColor(4, Red, Green, Blue);
  strip.setPixelColor(5, Red, Green, Blue);
  strip.setPixelColor(6, Red, Green, Blue);
  strip.setPixelColor(7, Red, Green, Blue);
  strip.show();
}
//////////////////////////////////////////////////////////////////////////////////////////////
// Function to change to next input
int changeInput() {
  timeOfLastOperation = millis();
  sleeping = false;
  if (muteEnabled) { changeMute(); }
  selectedInput++;
  if (selectedInput > 3) { selectedInput = 1; }
  if (selectedInput < 1) { selectedInput = 3; }
  switch (selectedInput) {
    case 1:
      setMCP23Sxx(inputSelectorCSPin, 9, B01001011);
      delay(5);
      break;
    case 2:
      setMCP23Sxx(inputSelectorCSPin, 9, B00110011);
      delay(5);
      break;
    case 3:
      setMCP23Sxx(inputSelectorCSPin, 9, B00101101);
      delay(5);
      break;
  }
  setMCP23Sxx(inputSelectorCSPin, 9, B00000001);
  setNeoPixelColour(255, 0, 0);
  delay(40);
  setNeoPixelColour(0, 255, 0);
  delay(40);
  setNeoPixelColour(0, 0, 255);
  delay(40);
  setNeoPixelColourFromAttenuator(currentAttenuatorLevel);
  if (debugEnabled) {
    Serial.print ("Selected Input: ");
    Serial.println (selectedInput);
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////
// Function to send data to the MCP23S17 (18 = GPIOA, 19 = GPIOB)
int setMCP23Sxx (int CSPin, int select_register, int register_value) {
  digitalWrite(CSPin,LOW);
  SPI.transfer(B01000000);
  SPI.transfer(select_register);
  SPI.transfer(register_value);
  digitalWrite(CSPin,HIGH);
}
//////////////////////////////////////////////////////////////////////////////////////////////
// Function to mirror and reverse a byte. This is used because the relays connected to one side of the setMCP23Sxx are reversed to the other.
int reverseAndMirrorByte (byte myByte) {
  byte myByteReversedAndMirrored;
  for (int reverseBit = 0; reverseBit < 8; reverseBit++) {
    if (bitRead(myByte, reverseBit) == 1) {
      bitWrite(myByteReversedAndMirrored, (7 - reverseBit), 0);
    } else {
      bitWrite(myByteReversedAndMirrored, (7 - reverseBit), 1);
    }
  }
  return myByteReversedAndMirrored;
}
//////////////////////////////////////////////////////////////////////////////////////////////
// Function to reverse a byte. This is used because the relays are connected to the setMCP23Sxx in reverse order.
int reverseByte (byte myByte) {
  byte myByteReversed;
  for (int reverseBit = 0; reverseBit < 8; reverseBit++) {
    if (bitRead(myByte, reverseBit) == 1) {
      bitWrite(myByteReversed, (7 - reverseBit), 1);
    } else {
      bitWrite(myByteReversed, (7 - reverseBit), 0);
    }
  }
  return myByteReversed;
}
//////////////////////////////////////////////////////////////////////////////////////////////
// Function to set a specific attenuator level
int setAttenuatorLevel (byte level) {
  muteEnabled = false;
  sleeping = false;
  currentAttenuatorLevel = level;
  if (debugEnabled) {
    Serial.print ("Attenuator Level: ");
    Serial.println (level);
  }
  byte real_level = 255 - level;
  byte gpioaValue = reverseByte(real_level);
  byte gpiobValue = reverseAndMirrorByte(gpioaValue);
  setMCP23Sxx(AttenuatorCSPin, 18, gpioaValue);
  setMCP23Sxx(AttenuatorCSPin, 19, gpiobValue);
  delay(1);
  setMCP23Sxx(AttenuatorCSPin, 18, B00000000);
  setMCP23Sxx(AttenuatorCSPin, 19, B00000000);
  // Set NeoPixel as well
  setNeoPixelColourFromAttenuator(level);
}
//////////////////////////////////////////////////////////////////////////////////////////////
// Functions to change volume
int changeVolume(int increment) {
  timeOfLastOperation = millis();
  sleeping = false;
  if (muteEnabled) { changeMute(); }
  int newAttenuatorLevel = currentAttenuatorLevel + increment;
  if (newAttenuatorLevel <= 255 && newAttenuatorLevel >= 0) {
    if (muteEnabled) { changeMute(); }
    setAttenuatorLevel(newAttenuatorLevel);
  } else if (newAttenuatorLevel > 255 && currentAttenuatorLevel != 255) {
    if (muteEnabled) { changeMute(); }
    setAttenuatorLevel(255);
  } else if (newAttenuatorLevel < 0 && currentAttenuatorLevel != 0) {
    setAttenuatorLevel(0);
  } else {
    if (debugEnabled) {
      Serial.println ("No volume change");
    }
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////
// Function to change mute on input selector
int changeMute() {
  timeOfLastOperation = millis();
  sleeping = false;
  if (muteEnabled) {
    if (debugEnabled) {
      Serial.print ("Mute now disabled, setting volume to: ");
      Serial.println (lastAttenuatorLevel);
    }
    setAttenuatorLevel(lastAttenuatorLevel);
    muteEnabled = false;
  } else {
    lastAttenuatorLevel = currentAttenuatorLevel;
    setAttenuatorLevel(0);
    muteEnabled = true;
    if (debugEnabled) {
      Serial.println ("Mute enabled");
    }
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////
// Main loop
//////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Read the encoder
  n = digitalRead(encoder0PinA);
  if ((encoder0PinALast == LOW) && (n == HIGH)) {
    if (digitalRead(encoder0PinB) == LOW) {
      changeVolume(-encoderIncrement);
    } else {
      changeVolume(encoderIncrement);
    }
  }
  encoder0PinALast = n;
  // Decode the IR if recieved
  if (irrecv.decode(&results)) {
    if (results.value == 2011287694) {
      lastIRoperation = "volumeUp";
      changeVolume(iRIncrement);
      timeOfLastIRChange = millis();
    }
    if (results.value == 2011279502) {
      lastIRoperation = "volumeDown";
      changeVolume(-iRIncrement);
      timeOfLastIRChange = millis();
    }
    if ((results.value == 2011291790) or (results.value == 2011238542)) {
      lastIRoperation = "changeInput";
      changeInput();
      delay(500);
      timeOfLastIRChange = millis();
    }
    if (results.value == 2011265678) {
      lastIRoperation = "playPause";
      changeMute();
      timeOfLastIRChange = millis();
    }
    if (results.value == 4294967295 && lastIRoperation != "None") {
      if (lastIRoperation == "volumeUp") { changeVolume(iRIncrement); timeOfLastIRChange = millis(); }
      if (lastIRoperation == "volumeDown") { changeVolume(-iRIncrement); timeOfLastIRChange = millis(); }
      if (lastIRoperation == "changeInput") { changeInput(); delay(500); timeOfLastIRChange = millis(); }
      timeOfLastIRChange = millis();
    }
    irrecv.resume(); // Receive the next value
  }
  // Read button state
  int buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    changeInput();
    delay(500);
  }
  // Set the time. This is used by other functions
  unsigned long currentTime = millis();
  // Stop IR repeating after timeout to stop interference from other remotes
  if ((currentTime - timeOfLastIRChange) > 1000 && lastIRoperation != "None") {
    lastIRoperation = "None";
  }
  // Fade out NeoPixel after a timeout
  if ((currentTime - timeOfLastOperation) > timeToSleep && !sleeping) {
    if (debugEnabled) { Serial.println ("Sleeping..."); }
    while ((NeoPixelRed + NeoPixelGreen + NeoPixelBlue) > 0) {
      if (NeoPixelRed > 0) { NeoPixelRed--; }
      if (NeoPixelGreen > 0) { NeoPixelGreen--; }
      if (NeoPixelBlue > 0) { NeoPixelBlue--; }
      setNeoPixelColour(NeoPixelRed, NeoPixelGreen, NeoPixelBlue);
      delay(5);
    }
    sleeping = true;
  }
}
