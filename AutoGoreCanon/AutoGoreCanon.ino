/*
Orginization: Scare for a Cure
Project: Auto-Gore Canon

Description:
Program used to control auto refill and firing gore-canon

Features:
-potentiometer to set pump time
-output to trigger outside audio device
-standby time to prevent re-triggering


Programmed by: James Manley
contact: jamesmanley1992@gmail.com

Last updated: 2021/09/30
*/

//LIBRARIES
#include <RBD_Timer.h>  // https://github.com/alextaujenis/RBD_Timer
#include <RBD_Button.h> // https://github.com/alextaujenis/RBD_Button


//inputs
int pumpPot = A0; // potentiometer that adjusts the water pump time.

//outputs
int air_relay = 3;
int pump_relay = 2;
int audio_out = 4; // output to audio controller device. 
int LED_Armed = 13;

//buttons
RBD::Button trigger = 11; //can either have 11 be a button 

//timers
RBD::Timer air; // how long the air will cycle for
RBD::Timer pause; // how long it will wait before starting the pump
RBD::Timer dispense; // how long the water pump will cycle for
RBD::Timer standby; //lock the system from being triggered again for a given amount of time
RBD::Timer audio; //used to stop trigger for audio if needed

//global variables
int potRead = 0;
int pumptime = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Begin Initialization");
  pinMode(air_relay, OUTPUT);
  pinMode(pump_relay, OUTPUT);
  pinMode(pumpPot, INPUT);
  air.setTimeout(1500); // set for air to trigger for 0.3 seconds
  if(air.onExpired()){ //need to make sure the on.Expired value is called beore entiring main program.
    delay(1);
  }
  pause.setTimeout(2000); // set to wait for 0.5 second after canon has stopped to start pumping water
  if(pause.onExpired()){ //need to make sure the on.Expired value is called beore entiring main program.
    delay(1);
  }
  dispense.setTimeout(5000); // dispense time will be set when the trigger button is pressed 
  if(dispense.onExpired()){ //need to make sure the on.Expired value is called beore entiring main program.
    delay(1);
  }
  standby.setTimeout(30000); // sets the system to standby for 30 seconds after last trigger 
  standby.restart(); // puts the system in 30 second standby on boot
  while (standby.isActive()){ //need to initiatilize all the timers
    delay(1);    
  }
}

void loop() {
  if(standby.onExpired()){
    Serial.println("Standby time has elapsed");
    digitalWrite(LED_Armed, HIGH);
  }
  
  if((standby.isActive()) && (trigger.onPressed())){
    Serial.println("button was pressed, but still in stand by");
  }
  
  if((standby.isExpired()) && trigger.onPressed()){
    Serial.println("button was pressed");
    Serial.println("air was triggered");
    digitalWrite(LED_Armed, LOW);
    digitalWrite(air_relay, HIGH);
    digitalWrite(audio_out, HIGH);
    //potRead = analogRead(pumpPot);
    potRead = 800; //not using potentiometer, just setting value in code.
    air.restart();
    
  }

  if( audio.onExpired()){
    digitalWrite(audio_out, LOW);
  }

  if( air.onExpired()){
    digitalWrite(air_relay, LOW);
    Serial.println("Air was turned off");
    Serial.println("waiting before turning on pump");
    pause.restart();
  }

  if( pause.onExpired()){
    digitalWrite(pump_relay, HIGH);
    Serial.println("Pump has been turned on");
    pumptime = map(potRead, 0, 1023, 1000, 10000); // map (input, input_low, input_high, output_low, output_high)
    // pump time has a range of 1 sec - 10 sec depending on position of potentiometer.
    dispense.setTimeout(pumptime);
    dispense.restart();
  }

  if( dispense.onExpired()){
    digitalWrite(pump_relay, LOW);
    Serial.println("Pump has been turned off");
    Serial.println("System is now on standby");
    standby.restart(); // standby wait time starts when the canon was triggered
  }



}
