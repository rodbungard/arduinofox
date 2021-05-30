// code Source: wt4y - https://www.wt4y.com/arduino/arduino-fox-controller
// Arduino controlled Fox Hunting Xmtr and / or Beacon 
// Code cobbled together from various sources
// Pin 6 is the square wave output for the Morse Code
// Pin 7 is Push to Talk pin (low = PTT)
// A0 is the Analog Input for DTMF detection

#include <DTMF.h>
#include "pitches.h"

int sensorPin = A0;  //The following block used by DTMF
float n = 128.0;
float sampling_rate = 8926.0;
DTMF dtmf = DTMF(n, sampling_rate);
float d_mags[8];
char thischar;

int xmit = 0;  // if xmit = 0, don't transmit

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_E4, NOTE_G4, NOTE_C5, 
  NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_E4, NOTE_G4, NOTE_C5
};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

#define PTT  7
#define BUZZER_PIN 6  // send music and cw out this pin
#define BUZZER_FREQUENCY 600  // cw pitch
char beaconstring[] = "Your Callsign";

char chartemp[4] ;
char incomingByte = 0;

int interval = 80;   //length of time for one dot - basic unit of measurement;  one interval occurs between each dot or dash; dash is equivalent to 3 dots; 7 dots between words

String alphabet = "abcdefghijklmnopqrstuvwxyz1234567890/ ";
String ALPHABET = alphabet;

String morseCode[] = { 
    ".-", // A
    "-...", // B
    "-.-.", // C
    "-..", // D
    ".", // E
    "..-.", // F
    "--.", // G
    "....", // H
    "..", // I
    ".---", // J
    "-.-", // K
    ".-..", // L
    "--", // M
    "-.", // N
    "---",  // O
    ".--.", // P
    "--.-", // Q
    ".-.", // R
    "...",  // S
    "-", // T
    "..-", // U
    "...-", // V
    ".--", // W
    "-..-", // X
    "-.--", // Y
    "--..", // Z
    ".----", // 1
    "..---", // 2
    "...--", // 3
    "....-", // 4
    ".....", // 5
    "-....", // 6
    "--...", // 7
    "---..", // 8
    "----.", // 9
    "-----", // 0
    "-..-.", // forward slash
    " "   //space character
};

void setup() {
  
  pinMode(PTT, OUTPUT);
  digitalWrite(PTT, HIGH);
  Serial.begin(9600);
//  pinMode(morseOutput, OUTPUT);
  
  ALPHABET.toUpperCase();
  randomSeed(analogRead(0));  // in case random delays between 
                              // transmissions are used
  digitalWrite(PTT, LOW);
  delay(1000);
  sendLetter('h');
  sendLetter('i');
  digitalWrite(PTT, HIGH);
  xmit = 0;
}

void loop() {

//  Serial.println(x);
  dtmf.sample(sensorPin);
  dtmf.detect(d_mags, 506);
  thischar = dtmf.button(d_mags, 1800.);
  
  if (thischar) {  // decide what to do if DTMF tone is received
    switch (thischar) {
      case 49:  // the number 1
        
        xmit = 1;  // set the flag to enable transmissions
      
  }
   delay(1500); //delay before sending
}

if (xmit == 1)
  {
  digitalWrite(PTT, LOW);
  delay(1); // delay 2 seconds after PTT to account for race condition added to remove delay

// play a little melody
  for (int thisNote = 0; thisNote < 24; thisNote++) {
    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZER_PIN, melody[thisNote], noteDuration);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.00;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(BUZZER_PIN);
  }

  delay(500);  //one second between melody and beaconstring

  for (int i = 0; i < sizeof(beaconstring); i++){
    sendLetter(beaconstring[i]);
    }

  delay(500);
  digitalWrite(PTT, HIGH);  // drop the PTT and wait a while
  xmit = 0; //added hope it will only do one cycle

  }
}

// End of Loop()
// Functions follow

void sendLetter (char c) {
   int i;
   for (i = 0; i < alphabet.length(); i = i + 1) {
       if (alphabet[i] == c || ALPHABET[i] == c) {
           //Serial.print(c);
           sendMorseCode(morseCode[i]);
           return;
       }
   }
   if (c == '\n')
      Serial.println();
}

void sendMorseCode (String tokens) {
   int i;
   for (i = 0; i < tokens.length(); i = i + 1) {
       switch (tokens[i]) {
           case '-':
               sendDash();
               break;
           case '.':
               sendDot();
               break;
           case ' ':
               sendEndOfWord();
               break;
       }
   }
   morseOutputOff(2);
//   Serial.print(" ");
}

void sendEndOfWord() {
   morseOutputOff(4);
//   Serial.print("  ");
}

//basic functions - Morse code concepts  
void sendDot() {
   tone(BUZZER_PIN, BUZZER_FREQUENCY);
   morseOutputOn(1);
   noTone(BUZZER_PIN);
   morseOutputOff(1);
//   Serial.print(".");
}
void sendDash() {
   tone(BUZZER_PIN, BUZZER_FREQUENCY);
   morseOutputOn(3);
   noTone(BUZZER_PIN);
   morseOutputOff(1);
//   Serial.print("-");
}

//Low level functions - how the actions are accomplished
// n = number of intervals 
// interval is a fixed length of time determined at start, for example 200 milliseconds
void morseOutputOn (int n) {
//   example: morseOutputOn(1) means turn output on and keep it on for 1 interval 
//            morseOutputOn(3) means turn output on and keep it on for 3 intervals 
//   
//   digitalWrite(morseOutput, HIGH);
   delay(n * interval);
}

void morseOutputOff (int n) {
//   digitalWrite(morseOutput, LOW);
   delay(n * interval);
}
