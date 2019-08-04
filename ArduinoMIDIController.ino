/* Arduino MIDI Controller
 
Created by Viktor G. / https://viktorgordienko.com

This code is in the public domain.

You can: copy it, modify it, share it, buy it, use it, break it, fix it, trash it, change it, mail - upgrade it, etc.

*/

//Potentiometers
int numberOfPots = 2;     //Total number of petentiometers
int potValues[2];         //Array of current values
int potLastValues[2];     //Array of last values (used to compare)

//Buttons
int numberOfButtons = 2;  //Total number of buttons EXCEPT the switch button
int buttonsStates[2];     // the current state of the output pin
int buttonReadings[2];    // the current reading from the input pin
int buttonPrevious[2];    // the previous reading from the input pin
long time = 0;            // the last time the output pin was toggled
long debounce = 200;      // the debounce time, increase if the output flickers
// the last 2 are long's because the time, measured in msec, will quickly become a bigger number than can be stored in an int.

//Switch button
int controlSwitch = 0;    //=64, used to add another set of controls
int switchState = LOW;
int switchReading;
int switchPrevious = HIGH;


void setup() {
  for (int i = 0; i <= numberOfButtons; i++) {
    pinMode(i + 8, INPUT_PULLUP); //+8 because I started on D8
  }
  pinMode(13, OUTPUT);
  Serial.begin(9600); //Set the speed of the midi port to the same as we will be using in the Hairless Midi software
}

void loop()
{
  //Potentiometesrs on А0-A(numberOfPots-1)
  for (int i = 0; i < numberOfPots; i++) {
    potValues[i] = analogRead(i) / 8;                     // Divide by 8 to get range of 0-127 for MIDI
    if (potValues[i] != potLastValues[i])                 //If the value does not = the last value the following command is made. This is because the pot has been turned. Otherwise the pot remains the same and no midi message is output.
    {
      MIDImessage(176, i + controlSwitch, potValues[i]);  //176 = CC command (channel i control change), (i + controlSwitch) = Which Control, val = value read from Potentionmeter 1
    }
    potLastValues[i] = potValues[i];
  }
  //Potentiometesrs on А0-A(numberOfPots-1)

  //Buttons on D8-D9
  for (int i = 0; i < numberOfButtons; i++) {
    buttonReadings[i] = digitalRead(i + 8);
    if (buttonReadings[i] == HIGH && buttonPrevious[i] == LOW && millis() - time > debounce) {
      if (buttonsStates[i] == HIGH) {
        buttonsStates[i] = LOW;
        MIDImessage(144, i + controlSwitch, 127);         //144 - Note ON, (i + controlSwitch) - note number in MIDI [e.g. 69 = A4], 127 - note velocity
      } else {
        buttonsStates[i] = HIGH;
        MIDImessage(144, i + controlSwitch, 127);
        time = millis();
      }
    }
    buttonPrevious[i] = buttonReadings[i];
  }
  //Buttons on D8-D9

  //The control switch button on D10 and LED on D13
  switchReading = digitalRead(10);
  if (switchReading == HIGH && switchPrevious == LOW && millis() - time > debounce) {
    if (switchState == HIGH) {
      switchState = LOW;
      controlSwitch = 0;
    } else {
      switchState = HIGH;
      controlSwitch = 64;
    }
  }
  digitalWrite(13, switchState);
  switchPrevious = switchReading;
  //The control switch button on D10 and LED on D13

  delay(10); //here we add a short delay to help prevent slight fluctuations, knocks on the pots etc. Adding this helped to prevent my pots from jumpin up or down a value when slightly touched or knocked.
}

void MIDImessage(byte message, byte control, byte value)  //Pass values out through standard Midi Command
{
  Serial.write(message);
  Serial.write(control);
  Serial.write(value);
}
