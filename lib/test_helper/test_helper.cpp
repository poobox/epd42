#include <Arduino.h>
#include <avr/pgmspace.h>
#include "test_helper.h"

bool ReadButton(int digital_pin){
  pinMode(SUCCESS_BUTTON, INPUT_PULLUP);
  pinMode(FAILURE_BUTTON, INPUT_PULLUP);
  return !digitalRead(digital_pin);
}

bool AskUserForTestSuccess(){
  bool result;
  int sb = ReadButton(SUCCESS_BUTTON);
  int fb = ReadButton(FAILURE_BUTTON);
  do {
    sb = ReadButton(SUCCESS_BUTTON);
    fb = ReadButton(FAILURE_BUTTON);
  } while(!sb && !fb);
  if (sb) {
    result = true;
  }else{
    result = false;
  }
  do {
    sb = ReadButton(SUCCESS_BUTTON);
    fb = ReadButton(FAILURE_BUTTON);
  } while(sb || fb);
  return result;
}
