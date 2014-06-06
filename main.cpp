/*
Copyright (c) 2014 NicoHood
See the readme for credit to other people.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

// include system librarys
#include <stdio.h> // printf
#include <stdlib.h> // exit(int);
#include <string.h> // errno
#include <errno.h> // error output
#include <stdint.h> // uint_t definitions
#include <stdbool.h> // bool type
#include <unistd.h> // sleep

// include library
#include "lib.h"

// wiring Pi
#include <wiringPi.h>
#include <wiringSerial.h>

// function prototypes
void setup(void);
void loop(void);
void heartbeat(uint16_t interval);
bool checkButton(void);
void buttonIsPressed(void);

// definitions
#define DEBOUNCE_DELAY 50/DEBOUNCE_READ
#define DEBOUNCE_READ 5
#define DEBOUNCE_SPACE 1000

// gpio pins
const int pinButton	= 25;
const int pinLed	= 24;

// led variable
static uint16_t ledInterval = 900;

// main function just calls setup once and loop infinite
int main(){ 
	setup();
	while(1)
		loop();
	return 0;
}

void setup(void){
	// put your setup code here, to run once:
	printf("%s \n", "ShutdownButton startup");

	//setup GPIO in Gpio mode 
	if (wiringPiSetupGpio () == -1){
		fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
		exit(1); //error
	}

	// pin setup
	pinMode (pinLed, OUTPUT);
	digitalWrite (pinLed, LOW);
	pinMode (pinButton, INPUT);
	pullUpDnControl (pinButton, PUD_UP);
}

void loop(void){
	// put your main code here, to run repeatedly:

	// check if button is pressed and execute buttonIsPressed() then
	while(checkButton());
	sleep(1);

	// blink the led
	heartbeat(ledInterval);
}

/*
Function that is executed on a button press
*/
void buttonIsPressed(void){
	//printf("%s \n", "Button pressed!");
	//system("echo System command test");
	//system("sudo reboot");

	// shut down system
	system("sudo halt");

	// blink led faster
	ledInterval = 100;
}

/*
Simple blink function without any delay
*/
void heartbeat(uint16_t interval){
	static unsigned long previousLed = 0;
	unsigned long currentLed = millis();
	if(currentLed - previousLed > interval) {
		previousLed = currentLed;   

		// if the LED is off turn it on and vice-versa:
		static bool ledState = LOW;
		digitalWrite (pinLed,  ledState);
		ledState^=1;
	}
}

/*
Button Debounce Function
Example values
#define DEBOUNCE_DELAY 50/DEBOUNCE_READ
#define DEBOUNCE_READ 5
#define DEBOUNCE_SPACE 300
*/
bool checkButton(void){

	static uint8_t debounce = 0;
	if(digitalRead(pinButton) == LOW){
		static unsigned long previousButton = 0;
		unsigned long currentButton = millis();

		// check if the space between the button presses passed
		if(debounce > DEBOUNCE_READ){
			if(currentButton - previousButton > DEBOUNCE_SPACE)
				debounce=0;	
			// if not do not check anything
			else currentButton=previousButton;
		}

		// check if next debounce delay passed
		if(currentButton - previousButton > DEBOUNCE_DELAY || debounce==0){
			previousButton = currentButton; 
			debounce++;
			//printf("%s %d \n", "Bounce ", debounce);

			if(debounce > DEBOUNCE_READ){
				// button is pressed down
				buttonIsPressed();
			}
		}
		return true;
	}
	else debounce=0;	
	return false;
}
