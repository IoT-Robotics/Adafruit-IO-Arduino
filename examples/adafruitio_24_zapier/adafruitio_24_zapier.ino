// Adafruit IO Time Tracking Cube
// Tutorial Link: https://learn.adafruit.com/time-tracking-cube
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Brent Rubell for Adafruit Industries
// Copyright (c) 2019 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

/************************ Example Starts Here *******************************/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>

// Used for Pizeo
#define PIEZO_PIN 0

// NeoPixel Pin on Prop-Maker FeatherWing
#define NEOPIXEL_PIN 5

// # of Pixels Attached
#define NUM_PIXELS 8

// Prop-Maker Wing
#define NEOPIXEL_PIN 2
#define POWER_PIN    15
#define RED_LED      13
#define GREEN_LED    12
#define BLUE_LED     14

// Library Setup
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Used for software SPI
#define LIS3DH_CLK 13
#define LIS3DH_MISO 12
#define LIS3DH_MOSI 11
// Used for hardware & software SPI
#define LIS3DH_CS 10

// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// Set up the 'cubeTask' feed
AdafruitIO_Feed *cubetask = io.feed("cubetask");

// Set up the 'minutes' feed
AdafruitIO_Feed *cubeminutes = io.feed("cubeminutes");

/* Time Tracking Cube States
 * 0: Neutral, Cube on Base
 * 1: Cube Tilted, Left on X-Axis
 * 2: Cube Tilted, Right on X-Axis 
*/
int cubeState = 0;

// Previous cube orientation state
int lastCubeState = 0;

// Tasks (change these to what you're currently working on)
String taskOne = "Write Learn Guide";
String taskTwo = "Write Code";
String taskThree = "Taking a Break!"; 

// Adafruit IO Sending Delay, in seconds
int sendDelay = 0.5;

// Time-Keeping
unsigned long currentTime;
unsigned long prevTime;
int seconds = 0;
int minutes = 0;

void setup()
{
  // start the serial connection
  Serial.begin(9600);
  // wait for serial monitor to open
  while (!Serial)
    ;
  Serial.println("Adafruit IO Time Tracking Cube");

  // Enabling NeoPixel and PWR mode
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  analogWrite(RED_LED, 0);
  analogWrite(GREEN_LED, 0);
  analogWrite(BLUE_LED, 0);

   
  // Initialize LIS3DH
  if (!lis.begin(0x18))
  {
    Serial.println("Couldnt start");
    while (1)
      ;
  }
  Serial.println("LIS3DH found!");
  lis.setRange(LIS3DH_RANGE_4_G);

  // This initializes the NeoPixel library.
  strip.begin();
  Serial.println("Pixels init'd");

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while (io.status() < AIO_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

}

void updateTime()
{
  currentTime = millis() / 1000;
  seconds = currentTime - prevTime;
  // increase min. timer
  if (seconds == 60)
  {
    prevTime = currentTime;
    minutes++;
  }
}

void updatePixels(uint8_t red, uint8_t green, uint8_t blue)
{
  // Power on the prop maker wing
  digitalWrite(POWER_PIN, HIGH);
  analogWrite(RED_LED, 0);
  analogWrite(GREEN_LED, 0);
  analogWrite(BLUE_LED, 0);
  for (int p=0; p<NUM_PIXELS; p++) {
    strip.setPixelColor(p, red, green, blue);
  }
  strip.show();
}

void loop()
{
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  // Update the timer
  updateTime();

  // Get a normalized sensor reading
  sensors_event_t event;
  lis.getEvent(&event);


  // Detect Cube Face Orientation
  if (event.acceleration.x > 9 && event.acceleration.x < 10) // left-side up
  {
    //Serial.println("Cube TILTED: Left");
    cubeState = 1;
  }
  else if (event.acceleration.x < -9) // right-side up
  {
    //Serial.println("Cube TILTED: Right");
    cubeState = 2;
  }
  else if(event.acceleration.y < 0 && event.acceleration.y > -1) // top-side up
  {
    cubeState = 3;
  }
  else
  { // orientation not found
  }

  // return if the orientation hasn't changed
  if (cubeState == lastCubeState)
    return;


  // Send to Adafruit IO based off of the orientation of the Cube
  switch (cubeState)
  {
  case 1:
    Serial.println("Switching to Task 1");
    updatePixels(50, 0, 0);
    tone(PIEZO_PIN, 650, 300);
    Serial.print("Sending to Adafruit IO -> ");
    Serial.println(taskTwo);
    cubetask->save(taskTwo);
    // save previous task's minutes to a feed
    cubeminutes->save(minutes);
    // reset the timer
    minutes = 0;
    break;
  case 2:
    Serial.println("Switching to Task 2");
    updatePixels(0, 50, 0);
    tone(PIEZO_PIN, 850, 300);
    Serial.print("Sending to Adafruit IO -> ");
    Serial.println(taskOne);
    cubetask->save(taskOne);
    // save previous task's minutes to a feed
    cubeminutes->save(minutes);
    // reset the timer
    minutes = 0;
    break;
  case 3:
    Serial.println("Switching to Task 3");
    updatePixels(0, 0, 50);
    tone(PIEZO_PIN, 950, 300);
    Serial.print("Sending to Adafruit IO -> ");
    // prv task
    // save previous task's minutes to a feed
    cubeminutes -> save(minutes);
    // reset the timer
    minutes = 0;
    break;
  }

  // store last cube orientation state
  lastCubeState = cubeState;

  // Delay the send to Adafruit IO
  delay(sendDelay * 1000);
}
