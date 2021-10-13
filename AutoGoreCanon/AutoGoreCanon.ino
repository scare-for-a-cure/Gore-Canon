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

#define pumprate 140 //gph of pump used to pump blood
#define ttl LOW // define wether the signal to the relay board is high or low

//inputs
int pumpPot = A0; // potentiometer that adjusts the water pump time.

//outputs
int air_relay = 3;
int pump_relay = 2;
int audio_out = 4; // output to audio controller device. 
int LED_Armed = 13;
int LED_Armed2 = 5; //setting up one of the relays for the arming indicator as well

//buttons
RBD::Button trigger = 11; //can either have 11 be a button 

//timers
RBD::Timer air; // how long the air will cycle for
RBD::Timer pause; // how long it will wait before starting the pump
RBD::Timer dispense; // how long the water pump will cycle for
RBD::Timer standby; //lock the system from being triggered again for a given amount of time
RBD::Timer audio; //used to stop trigger for audio if needed
RBD::Timer led; // used to flash arming LED progressively faster as it gets ready to fire again

//global variables
int potRead = 0;
int pumptime = 0;
int pumptime_max = 10000; // set the default max time to 10 seconds, this will adjust in the actual program based on the defined pump rate above
int ledState = LOW; //tracks whether the LED is currently high or low during its flashing state

void setup() {
  Serial.begin(9600);
  Serial.println("Begin Initialization");
  
  pinMode(air_relay, OUTPUT);
  digitalWrite(air_relay, !ttl);
  
  pinMode(pump_relay, OUTPUT);
  digitalWrite(pump_relay, !ttl);
  
  pinMode(audio_out, OUTPUT);
  digitalWrite(audio_out, !ttl);

  pinMode(LED_Armed, OUTPUT);
  digitalWrite(audio_out, LOW);

  pinMode(LED_Armed2, OUTPUT);
  digitalWrite(audio_out, !ttl);  
  
  pinMode(pumpPot, INPUT);
   
  air.setTimeout(1000); // set for air to trigger for 1.0 seconds
  air.onExpired(); //need to make sure the on.Expired value is called before entiring main program.

  audio.setTimeout(100); //used to stop trigger for audio if needed
  audio.onExpired();

  pause.setTimeout(2000); // set to wait for 0.5 second after canon has stopped to start pumping water
  pause.onExpired(); //need to make sure the on.Expired value is called before entiring main program.

  dispense.setTimeout(5000); // dispense time will be set when the trigger button is pressed 
  dispense.onExpired(); //need to make sure the on.Expired value is called before entiring main program.

  standby.setTimeout(30000); // sets the system to standby for 30 seconds after last trigger 
  standby.restart(); // puts the system in 30 second standby on boot

}

void loop() {
  if(standby.isActive()){ // this script handles the flashing of the armed led to go progressively faster as it gets ready to arm.
    led.setTimeout( map(standby.getInversePercentValue(), 0, 100, 50, 1000)) ;
    if(led.onRestart()){
      ledState = !ledState;
      digitalWrite(LED_Armed, ledState);
      digitalWrite(LED_Armed2, ledState);
    }
  }
  
  if(standby.onExpired()){
    Serial.println("Standby time has elapsed");
    digitalWrite(LED_Armed, HIGH);
    digitalWrite(LED_Armed2, ttl); 
  }
  
  
  if(standby.isExpired() && trigger.onPressed()){ 
  //if(standby.isExpired() && true ){   // this line is used to constantly trigger the cycle for testing
    standby.stop();
    Serial.println("button was pressed");
    Serial.println("air was triggered");
    digitalWrite(LED_Armed, LOW); // LED is iether built in or wired externally, so it doesn't use the relays ttl
    digitalWrite(LED_Armed2, !ttl);
    digitalWrite(air_relay, ttl);
    digitalWrite(audio_out, ttl);
    audio.restart();
    //potRead = analogRead(pumpPot);
    potRead = 512; //not using potentiometer, just setting value in code.
    air.restart();
    
  }

  if( audio.onExpired()){
    digitalWrite(audio_out, !ttl);
  }

  if( air.onExpired()){
    digitalWrite(air_relay, !ttl);
    Serial.println("Air was turned off");
    Serial.println("waiting before turning on pump");
    pause.restart();
  }

  if( pause.onExpired()){
    digitalWrite(pump_relay, ttl);
    Serial.println("Pump has been turned on");
    pumptime_max = 700000 / pumprate;
    pumptime = map(potRead, 0, 1023, 1000, pumptime_max ); // map (input, input_low, input_high, output_low, output_high)
    // pump time has a range of 1 sec - 10 sec depending on position of potentiometer.
    dispense.setTimeout(pumptime);
    dispense.restart();
  }

  if( dispense.onExpired()){
    digitalWrite(pump_relay, !ttl);
    Serial.println("Pump has been turned off");
    Serial.println("System is now on standby");
    standby.restart(); // standby wait time starts when the canon was triggered
  }



}
