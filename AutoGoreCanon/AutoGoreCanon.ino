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
int air_relay = 2;
int pump_relay = 3;
int audio_out = 4; // output to audio controller device. 

//buttons
RBD::Button trigger = 11; can either have 11 be a button 

//timers
RBD::Timer air; // how long the air will cycle for
RBD::Timer pause; // how long it will wait before starting the pump
RBD::Timer dispense; // how long the water pump will cycle for
RBD::Timer standby; //lock the system from being triggered again for a given amount of time
RBd::Timer audio; //used to stop trigger for audio if needed

//global variables
int potRead = 0;
int pumptime = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Begin Initialization");
  pinMode(air_relay, OUTPUT);
  pinMode(pump_relay, OUTPUT);
  pinMode(pumpPot, INPUT);
  air.setTimeout(300); // set for air to trigger for 0.3 seconds
  pause.setTimeout(500); // set to wait for 0.5 second after canon has stopped to start pumping water
  dispense.setTimeout(5000); // dispense time will be set when the trigger button is pressed 
  standby.setTimeout(30000); // sets the system to standby for 30 seconds after last trigger 
  standby.restart(); // puts the system in 30 second standby on boot
}

void loop() {
  if(standby.onExpired()){
    Serial.println("Standby time has elapsed");
  }
  
  if((standby.isActive()) && (trigger.onPressed())){
    Serial.println("button was pressed, but still in stand by");
  }
  
  if((standby.isExpired()) && trigger.onPressed()){
    Serial.println("button was pressed");
    Serial.println("air was triggered");
    digitalWrite(air_relay, HIGH);
    digitalWrite(audio_out, HIGH);
    potRead = analogRead(pumpPot);
    air.restart();
    standby.restart(); // standby wait time starts when the canon was triggered
  }

  if(audio.onExpired()){
    digitalWrite(audio_out, LOW);
  }

  if(air.onExpired()){
    digitalWrite(air_relay, LOW);
    Serial.println("Air was turned off");
    Serial.println("waiting before turning on pump");
    pause.restart();
  }

  if(pause.onExpired()){
    digitalWrite(pump_relay, HIGH);
    Serial.println("Pump has been turned on");
    pumptime = map(potRead, 0, 1023, 1000, 10000); // map (input, input_low, input_high, output_low, output_high)
    // pump time has a range of 1 sec - 10 sec depending on position of potentiometer.
    dispense.setTimeout(pumptime);
    dispense.restart();
  }

  if(dispense.onExpired()){
    digitalWrite(pump_relay, LOW);
    Serial.println("Pump has been turned off");
    Serial.println("System is now on standby")
  }



}
