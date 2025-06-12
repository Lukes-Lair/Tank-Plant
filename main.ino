#include <Arduino.h>
#include <Adafruit_IS31FL3731.h>
#include <Adafruit_GFX.h>

Adafruit_IS31FL3731 matrix = Adafruit_IS31FL3731();

//motor pins
#define APhase 4
#define BPhase 7
#define BEnable 5 //enable is anologe
#define AEnable 6

//light and water sensor pins
#define watersen A2
#define L1 A3
#define L2 A1

//for the screen timer
unsigned long timemillis;


bool movebool = true;

void setup() {

  //front bumpers
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  //initilize the Screen
matrix.begin();
matrix.setTextColor(100);
matrix.setTextWrap(false);
//configure motor pins
  pinMode(APhase, OUTPUT);
  pinMode(BPhase, OUTPUT);
  pinMode(AEnable, OUTPUT);
  pinMode(BEnable, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  //light sensor prints for debugging
  Serial.print("L1: ");
  Serial.print(analogRead(L1));
  Serial.print("\n");
  Serial.print("L2: ");
  Serial.print(analogRead(L2));
  Serial.print("\n");

  // one of the bumpers in the front
  if (digitalRead(2) == LOW){
    movebool = false;
    backleft();
  }

  //light sensor values
  int L1value = analogRead(L1);
  int L2value = analogRead(L2);

  float percentdiff = percentlight(L1value, L2value);

  if (movebool == true){
    move(percentdiff, L1value, L2value);
  }
  if (millis() - timemillis >= 30000) {
    //reset cooldown
    timemillis = millis();

    //reset screen
    matrix.clear();

    //Mosture Sensor Stuff
    unsigned int mosture_level = analogRead(watersen);
    Serial.print("Mosture Level: ");
    Serial.print(mosture_level);
    Serial.print("\n");
    
    //testing the mosture levelss
    if (mosture_level <= 100){
      matrixtext("SCORCHED");
      } else if (mosture_level <= 250) {
        matrixtext("PARCHED");
      } else if (mosture_level <= 450) {
        matrixtext("THIRSTY");
      } else if (mosture_level <= 750) {
        matrixtext("SATISFIED");
      } else {
        matrixtext("DROWNING");
      }
    }
}

//function to write text on the screen
void matrixtext(const char* text) {
  int x = 0;
  int resetpoint = (strlen(text) * 6) + matrix.width();
      while (x < resetpoint) {
      matrix.clear();
      matrix.setCursor(matrix.width() - x, 1);
      matrix.print(text);
      x++;
      delay(100); 
      }
}


void move(float percent, int l1value, int l2value) {
percent = (1 - percent)/2;
  if (l1value > l2value){
  digitalWrite(APhase, LOW);
  digitalWrite(BPhase, LOW);
  analogWrite(AEnable, percent * 255);
  analogWrite(BEnable, 255);
  } else if (l2value > l1value){
    digitalWrite(APhase, LOW);
    digitalWrite(BPhase, LOW);
    analogWrite(AEnable, 255);
    analogWrite(BEnable, percent * 255);
  }
}


float percentlight(int l1, int l2) {
  if (l1 > l2) {
    return (l1-l2)/l1;
  } else if (l2 > l1) {
    return (l2-l1)/l2;
  }
}


void backleft() {
  unsigned long backmillis = millis();

  while (millis() - backmillis < 1300)
  {
    digitalWrite(APhase, HIGH);
    digitalWrite(BPhase, HIGH);
    analogWrite(AEnable, 255);
    analogWrite(BEnable, 255);
  }

    backmillis = millis();

  while (millis() - backmillis < 500)
  {
    digitalWrite(APhase, HIGH);
    digitalWrite(BPhase, LOW);
    analogWrite(AEnable, 255);
    analogWrite(BEnable, 255);
  }

    digitalWrite(APhase, LOW);
    digitalWrite(BPhase, LOW);
    analogWrite(AEnable, 0);
    analogWrite(BEnable, 0);
    movebool = true;
  
}