// Arduino controlled Fox Hunting Xmtr 
// Code cobbled together from various sources
// Including N1RUU, KC9UNS and NC7R
// Make sure to put your call sign in beaconstring
// Pin 6 is the square wave output for the Morse Code
// Pin 7 is Push to Talk pin (low = PTT)
// DTMF Functions below
// 1 send random tones for about 30 seconds and ID
// 2 send random tones for about 10 seconds and ID
// 3 reset session counter and transmit session counter
// 8 send total number of activations since unit was powered up
// 9 send session counter

#include "DTMF.h"

//**************************************************************
// this is the only section that needs to be modified
char beaconstring[] = "N1CALL"; // your call sign
int tonelow = 120;    // low tone of the random tones
int tonehigh = 750;   // high tone of the radom tones
int tonelength = 200; // length in ms of tone
int tonepause = 250;  // length in ms of tone pause between tones
int cycles1 = 105;    // number of cycles for DTMF 1
int cycles2 = 40;     // number of cycles for DTMF 2
int cwpitch = 600;    // cw pitch
int dotinterval = 60; // interval of CW dot      
//**************************************************************


int sensorPin = A0;  //The following block used by DTMF
float n = 128.0;
float sampling_rate = 8926.0;
DTMF dtmf = DTMF(n, sampling_rate);
float d_mags[8];
char thischar;

// int pause = 220;   // pause between transmissions, 22 = 1 second (approx)
int keycount = 0;  // initialize transmit counter to 0
int sescount = 0; // initialize session counter to 0


#define PTT 7
// #define RELAY 3

#define BUZZER_PIN 6          // send tones and cw out this pin
#define BUZZER_FREQUENCY cwpitch  // cw pitch


char chartemp[4];
char incomingByte = 0;
//int morseOutput = 4; //Pin for receiving Morse output - attach LED to this pin
int interval = dotinterval;  //length of time for one dot - basic unit of measurement;  one interval occurs between each dot or dash; dash is equivalent to 3 dots; 7 dots between words
//int interval = 10;';kk'kgk';'g';k''0;  //length of time for one dot - basic unit of measurement;  one interval occurs between each dot or dash; dash is equivalent to 3 dots; 7 dots between words
String keyupstr = "1234";  // assume less than 4 digit number of keyups

String alphabet = "abcdefghijklmnopqrstuvwxyz1234567890/ ";
String ALPHABET = alphabet;

String morseCode[] = {
  ".-",     // A
  "-...",   // B
  "-.-.",   // C
  "-..",    // D
  ".",      // E
  "..-.",   // F
  "--.",    // G
  "....",   // H
  "..",     // I
  ".---",   // J
  "-.-",    // K
  ".-..",   // L
  "--",     // M
  "-.",     // N
  "---",    // O
  ".--.",   // P
  "--.-",   // Q
  ".-.",    // R
  "...",    // S
  "-",      // T
  "..-",    // U
  "...-",   // V
  ".--",    // W
  "-..-",   // X
  "-.--",   // Y
  "--..",   // Z
  ".----",  // 1
  "..---",  // 2
  "...--",  // 3
  "....-",  // 4
  ".....",  // 5
  "-....",  // 6
  "--...",  // 7
  "---..",  // 8
  "----.",  // 9
  "-----",  // 0
  "-..-.",  // forward slash
  " "       //space character
};

void setup() {

  pinMode(PTT, OUTPUT);
  digitalWrite(PTT, HIGH);
  // pinMode(RELAY, OUTPUT);
  // Serial.begin(9600);
  //  pinMode(morseOutput, OUTPUT);
  // pinMode(morseOutput, OUTPUT);

  ALPHABET.toUpperCase();
  randomSeed(analogRead(0));  // in case random delays between
  // transmissions are used
  Serial.println("Initialized");
}

void loop() {

  // Serial.println(x);
  dtmf.sample(sensorPin);
  dtmf.detect(d_mags, 506);
  thischar = dtmf.button(d_mags, 1800.);

  // Serial.print("thischar=");
  // Serial.println(thischar);

  if (thischar) {  // decide what to do if DTMF tone is received
    switch (thischar) {
      case 49:  // the number 1 so enable standard fox transmission
        //  Serial.println("1 detected");
        keycount++;
        sescount++;
        // Serial.println("start transmitting");
        digitalWrite(PTT, LOW);
        delay(500);  // delay 0.5 second after PTT to account for race condition
        for (int cycles = 0; cycles < cycles1; cycles++) {
         
            tone(BUZZER_PIN,random(tonelow,tonehigh),tonelength); //play a random tone (low,high), duration
            // to distinguish the notes, set a minimum time between them.
            // the note's duration + 30% seems to work well:
            //int pauseBetweenNotes = 250;
            delay(tonepause); //delay between tones
            // stop the tone playing:
            noTone(BUZZER_PIN);
          //}
        }
        // fall out of cycles number of repeats of the tone then ID and stop
        delay(1000);  //one second between melody and beaconstrin
        for (int i = 0; i < sizeof(beaconstring); i++) {
          sendLetter(beaconstring[i]);
        }
        delay(500);
        digitalWrite(PTT, HIGH);  // drop the PTT and wait a while
        break;

        case 50:  //number 2 to send 1 series of tones faster
        keycount++;
        sescount++;
        digitalWrite(PTT, LOW);
        delay(500);  // delay 0.5 second after PTT to account for race condition
        for (int cycles = 0; cycles < cycles2; cycles++) {
         
            tone(BUZZER_PIN,random(tonelow,tonehigh),tonelength); //play a random tone (low,high), duration
            
            delay(tonepause); //delay between tones
            // stop the tone playing:
            noTone(BUZZER_PIN);
         
        }
          delay(1000);  //one second between melody and beaconstrin
        for (int i = 0; i < sizeof(beaconstring); i++) {
          sendLetter(beaconstring[i]);
        }
        delay(500);
        digitalWrite(PTT, HIGH);  // drop the PTT and wait a while
        break;
         
        break;

        case 51:  //number 3 to to clear session counter
        sescount = 0;
         delay(2000);
         keyupstr = String(sescount);
        digitalWrite(PTT, LOW);
        delay(2000);
        for (int i = 0; i < sizeof(keyupstr); i++) {
          sendLetter(keyupstr[i]);
        }
        digitalWrite(PTT, HIGH);
        delay(2000);
        break;
      //case 53:  //number 5 send a quick message
      
        //break;
      //case 54:  //number 6
    
        //break;
      
      //case 55:  //number 7 send a 73 message
    
       // break;
      case 56:  //number 8 send number of keyups message
        keyupstr = String(keycount);
        digitalWrite(PTT, LOW);
        delay(2000);
        for (int i = 0; i < sizeof(keyupstr); i++) {
          sendLetter(keyupstr[i]);
        }
        digitalWrite(PTT, HIGH);
        delay(2000);
        break;

      case 57:  //number 9 sends count of session counter
      keyupstr = String(sescount);
        digitalWrite(PTT, LOW);
        delay(2000);
        for (int i = 0; i < sizeof(keyupstr); i++) {
          sendLetter(keyupstr[i]);
        }
        digitalWrite(PTT, HIGH);
        delay(2000);
        break;

        
      default:  // any other number do nothing
        break;
    }
  }


delay(499);
}

// End of Loop()
// Functions follow

void sendLetter(char c) {
  int i;
  for (i = 0; i < alphabet.length(); i = i + 1) {
    if (alphabet[i] == c || ALPHABET[i] == c) {
      // Serial.print(c);
      sendMorseCode(morseCode[i]);
      return;
    }
  }
  if (c == '\n')
    Serial.println();
}

void sendMorseCode(String tokens) {
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
void morseOutputOn(int n) {
  //   example: morseOutputOn(1) means turn output on and keep it on for 1 interval   .
  //            morseOutputOn(3) means turn output on and keep it on for 3 interval   -
  //
  //   digitalWrite(morseOutput, HIGH);
  delay(n * interval);
}

void morseOutputOff(int n) {
  //   digitalWrite(morseOutput, LOW);
  delay(n * interval);
}
