# Gore-Canon
Auto reloading gore canon program.

This program uses arduino IDE to program an arduino nano.

It also used the following libraries that will need to be installed locally to your arduino instance.
-RBD_Timer
-RBD_Button


Plan for canon.
1-Upon recieving a signal from a momentary button, the system is set to immediatly trigger the air solenoid that is meant to fire the blood from the canon.

2- after firing the canon it is to wait a few seconds before it attempts to start pumping in new blood

3- The software reads the state of a potentiameter to determine what the desired pump time will be, and then pumps for that given amount of time.
  (the pour time is contrained between 1sec and 10 seconds)
  
4- the system is then moved into standby where it can't be activated again until atleast 30 seconds have elapsed since its last triggering.

There is no need for code changes for a "manual" mode, as the only difference a manual mode would have would be that the pump would be unplugged.
There is no harm in leaving the code the same even if the pump isn't conencted.



# Arduino Nano Pin Plan.
The pinouts for this are trying to remain close to what is used by the octobanger software so it can be programmed with octobanger if needed without changing the wiring.
Octobanger adapter board -> https://www.scarecompany.com/product/octopill-prop-controller-shield-assembled/
OctoBanger software -> http://buttonbanger.com/?page_id=164
///outputs 
pins 2-9 : relays 1-8
pin 10 : trigger out
pin 12 : audio trigger out

///inputs
pin 11 : trigger in
