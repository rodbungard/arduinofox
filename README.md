# arduinofox
This project is for Amateur radio operators that participate in "Fox Hunting" or Hidden radio direction finding.  I built my first Fox Box using the Byonics PicCon V3.  I found that controller to be buggy and was not happy at all with the price of it. 

WT4Y has a arduino fox controller project that I used as a guide for my controller.  I used an arduino mico pro, 1 resistor and a MIC/Speaker cable to make this.  I have it connected to a Baofeng radio.  See https://www.wt4y.com/arduino/arduino-fox-controller  for the code.  Included is the code that I modified to work for me.  I also change the tune it plays to make it my own. Make sure you put your Call Sign in the beaconstring. Total cost is under $10.  Use at your own risk, it works good for me.

Copy the DTMF folder into your Arduino Libraries folder

The current code listens for a DTMF1 to start the transmission of the tones followed by the Call Sign.  It does this once then goes back to listening.
