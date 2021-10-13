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
int LED_Armed = 13; //using the built in led as a armed status led
int LED_Armed2 = 5; //setting up one of the unused relays for the arming indicator as well

//buttons
RBD::Button trigger = 11; //can either have 11 be a button 

//timers
RBD::Timer air(1000); // how long the air will cycle for
RBD::Timer pause(2000); // how long it will wait before starting the pump
RBD::Timer dispense(5000); // how long the water pump will cycle for
RBD::Timer standby(5000); //lock the system from being triggered again for a given amount of time
RBD::Timer audio(100); //used to stop trigger for audio if needed
RBD::Timer led(100); // used to flash arming LED progressively faster as it gets ready to fire again

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
   
  air.onExpired(); //need to make sure the on.Expired value is called before entiring main program.
  audio.onExpired();
  pause.onExpired(); //need to make sure the on.Expired value is called before entiring main program.
  dispense.onExpired(); //need to make sure the on.Expired value is called before entiring main program.
  standby.restart(); // puts the system in 5 second standby on boot

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
    standby.stop(); // turns off standby so its not blinking in the active stage, it will start blinking once it is restarted att he end of the action.
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
    standby.setTimeout(30000); //sets the timeout for standby to be 30 seconds, so its only 5 seconds upon boot. (this helps with diagnosing a emf reboot)
    standby.restart(); // standby wait time once all the actions have completed, and starts blinking the armed led
    
  }



}
