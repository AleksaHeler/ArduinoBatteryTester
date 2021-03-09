/* EVERY 30s log data
 * Outputs a comma seperated value text file which can be used to make graphs
 * 
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 * 
 ** A0 - Battery voltage
 ** A1 - Battery charge module output voltage (across the discharge resistor)
*/

#include <SPI.h>
#include <SD.h>

#define RESISTOR 100        // Resistor value in ohms
#define LED_RED 5           // Red pin
#define LED_YELLOW 3        // Yellow pin
#define LED_GREEN 2         // Green pin
#define CUTOFF_VOLTAGE 3.0  // Volts
#define filename "test.txt"

File myFile;

void setup() {
  ///// LEDS //////////////////////////////////
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  setLED(0,1,0); // red, yellow, green enable

  ///// SERIAL ////////////////////////////////
  Serial.begin(9600);

  ///// SD CARD ///////////////////////////////
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    setLED(1,0,0);
    while (1);
  }
  Serial.println("initialization done.");
  
  ///// PREPARING FILE ////////////////////////
  myFile = SD.open(filename, FILE_WRITE);
  if (myFile) {
    myFile.println("TIME (s), VOLTAGE (V), CURRENT (mA)");
    myFile.close();
    Serial.println("File " filename " prepared: 'TIME (s), VOLTAGE (V), CURRENT (mA)'");
  } else{
    Serial.print("error opening " filename);
    setLED(1,0,0);
    while(1);
  }
  setLED(0,0,1);
}

void loop() {
  fileWrite(); // Write log
  delay(20000); // wait 20s
}

void fileWrite(){
  setLED(0,1,1);
  
  // open the file
  myFile = SD.open(filename, FILE_WRITE);

  // if the file opened okay, write to it
  if (myFile) {
    unsigned long t_seconds = millis() / (1000); // SECONDS
    double t = t_seconds / (60*60); // HOURS
    float v = 1.01 * ( (float)analogRead(A0)/(float)1024 ) * 5; // VOLTS
    float i = 1.01 * 1000.0 * ( (float)analogRead(A1)/(float)1024 ) * 5/(float)RESISTOR; // MILLIAMPERES
    String str = String(t) + String(", ") + String(v) + String(", ") + String(i);

    Serial.println(str);
    myFile.println(str);

    // If voltage is too low stop testing and turn on yellow light
    if(v < CUTOFF_VOLTAGE){
      setLED(1,1,0);
      Serial.println("REACHED CUTOFF VOLTAGE!");
      myFile.print("REACHED CUTOFF VOLTAGE ");
      myFile.println(CUTOFF_VOLTAGE);
      myFile.close();
      while(true);
    }
    if(i < 1){ // current less than 1mA
      setLED(1,1,0);
      Serial.println("NO CURRENT FLOWING!");
      myFile.println("NO CURRENT FLOWING!");
      myFile.close();
      while(true);
    }

    myFile.close();
  } else {
    setLED(1,0,0);
    Serial.println("error opening " filename);
    while(1);
  }

  setLED(0,0,1);
}

void setLED(bool r, bool y, bool g){
  digitalWrite(LED_RED, r);
  digitalWrite(LED_YELLOW, y);
  digitalWrite(LED_GREEN, g);
}
