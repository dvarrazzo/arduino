/*
 * LUCKYLARRY.CO.UK - IR Remote control for Nikon using Arduino
 *
 * Mimics the infrared signal to trigger the remote for any Nikon camera which
 * can use the ML-L1 and ML-L3 remotes. Can be used as an intervalometer for
 * time lapse photography.
 *
 * The IR sequence I used is originally taken from:
 * http://www.bigmike.it/ircontrol/ You should be able to use my pulse methods
 * to alter to suit other cameras/ hardware.  micros() is an Arduino function
 * that calls the time in Microseconds since your program first ran. Arduino
 * doesn't reliably work with microseconds so we work our timings by taking
 * the current reading and then adding our delay on to the end of it rather
 * than rely on the in built timer.
 */

int pinIRLED = 13;                          // assign the Infrared emitter/ diode to pin 13

// sets the pulse of the IR signal.
void pulseON(int pulseTime) {
  unsigned long endPulse = micros() + pulseTime;    // create the microseconds to pulse for
  while( micros() < endPulse) {
    digitalWrite(pinIRLED, HIGH);           // turn IR on
    delayMicroseconds(13);                  // half the clock cycle for 38Khz (26.32×10-6s) - e.g. the 'on' part of our wave
    digitalWrite(pinIRLED, LOW);            // turn IR off
    delayMicroseconds(13);                  // delay for the other half of the cycle to generate wave/ oscillation
  }
}

void pulseOFF(unsigned long startDelay) {
  unsigned long endDelay = micros() + startDelay;   // create the microseconds to delay for
  while(micros() < endDelay);
}

void takePicture() {
  for (int i=0; i < 2; i++) {
    pulseON(2000);                          // pulse for 2000 uS (Microseconds)
    pulseOFF(27850);                        // turn pulse off for 27850 us
    pulseON(390);                           // and so on
    pulseOFF(1580);
    pulseON(410);
    pulseOFF(3580);
    pulseON(400);
    pulseOFF(63200);
  }                                         // loop the signal twice.
}

void setup() {
  pinMode(pinIRLED, OUTPUT);                // set the pin as an output
  delay(5000);                              // time to switch camera on bulb
}

void loop() {
    // Camera in B mode
    takePicture();                          // take the picture
    delay(20 * 1000);                       // delay in milliseconds which allows us to do timelapse photography - 1 second = 1000 milliseconds
    takePicture();                          // take the picture
    delay(150);  // required for bulb

    /*
     * Regular time lapse.
     *
     * For 10" exposure, delay 10000 + pause 200. With pause 100 1/2 shots
     * are lost.
     *
     * For 15" exposure, delay 16000" (sic) + pause 200, but timing between
     * mirror up and mirror down is irregular, and sporadic shots are lost.
     */

    /*
    takePicture();
    delay(16000);
    delay(200);
    */
}

