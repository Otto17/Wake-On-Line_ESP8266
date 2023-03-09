#include "SmartDelay.h"

/*
  Pin 2 -- button.
  Pin 3-5 -- lamps.
  Lamps blinks with different frequency.
  Pressing button holds lamp 3.
*/

void setup() {
  pinMode(2,INPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
}

SmartDelay l3(800000UL); // blink 800ms
SmartDelay l4(1100000UL); // blink 1100ms
SmartDelay l5(230000UL); // blink 230ms
unsigned char l3state = LOW;
unsigned char l4state = LOW;
unsigned char l5state = LOW;


void loop () {
  if (l3.Now()) {
    digitalWrite(3,l3state);
    l3state=(l3state==LOW?HIGH:LOW);
  }
  if (l4.Now()) {
    digitalWrite(4,l4state);
    l4state=(l4state==LOW?HIGH:LOW);
  }
  if (l5.Now()) {
    digitalWrite(5,l5state);
    l5state=(l5state==LOW?HIGH:LOW);
  }
  if (digitalRead(2)) l3.Wait();
}
