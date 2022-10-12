#include <Wire.h> //library for i2c comunitcation
#include "Adafruit_VL53L0X.h" //library for the sensors
#include "MIDI.h" //library for playing notes



Adafruit_VL53L0X lox1 = Adafruit_VL53L0X(); //sensor 1
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X(); //sensor 2

int note=0; //variable containing the note played on the channel, hence the active note


MIDI_CREATE_DEFAULT_INSTANCE(); //interface of the midi commands.

void setup() {
  // setting the mode of GPIOs PINs
  pinMode(27, OUTPUT);  //xshut of the sensor2
  pinMode(33, OUTPUT); //xshut of the sensor1
  Wire.begin(); //i2c channel initialization
  Serial.begin(115200); //serial channel initialization

//reset of the sensors
  digitalWrite(33, LOW);
  digitalWrite(27, LOW);
  delay(10);
  digitalWrite(33, HIGH);
  digitalWrite(27, HIGH);
  delay(10);


  digitalWrite(27, LOW);

  //setting address of sensor 1
  lox1.begin(0x31);
  lox1.startRangeContinuous();

  //power on of sensor 2
  digitalWrite(27, HIGH);
  delay(10);

  //setting address of sensor 2
  lox2.begin(0x32);
  lox2.startRangeContinuous();
  delay(10);

}

void loop() {
 //acquiring the measure from the sensors
   int dist1=round(lox1.readRange()/4);
   int dist2=round(lox2.readRange()/4);

    if (dist1<1000&dist2<1000&dist1!=note) //there are three conditions, the fist two are used to check if there is an object in front of each sensor,
    //the thirth checks if the note has been already played. In this way we send a serial command only if it is necessary and we are saving resources.
    {
      MIDI.sendNoteOff(note,dist2,1);  //before playing any new note we have to mute all the previous notes, otherwise the sounds start to overlap
      MIDI.sendProgramChange(0,1); //we set the instrument, the MIDI library each instrument is identified by a couple, bank and presetn, and plays on a given channel
      // in this case we are setting the preset 0 on the channel 1.
      MIDI.sendNoteOn(note,dist2,1);  //one we've setted the instrument and the channel we can play the note having the frequency "note" and the volume dist2
      note=dist1; //once we mute all the notes we save the value of the note active on the channel on the variable note

    }
    else
      if (dist1>1000& note!=0) //if there is nothing in front of both the sensors and there is an active note we have to mute it
      {
        MIDI.sendNoteOff(note,dist2,1);
        note=0; //since there are no active notes on the channel the value of the active note is setted to 0 and will be different to zero only after we've played a new note
      }


}
