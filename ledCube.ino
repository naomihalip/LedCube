#include <IRremote.h>
#include "LedControl.h"

#define RED 64
#define GREEN 16
#define BLUE 32
#define PURPLE 96
#define YELLOW 80
#define TURQUOISE 48
#define N 10000

//pentru telecomanda
#define ZERO 0xFF6897
#define ONE 0xFF30CF
#define TWO 0xFF18E7
#define THREE 0xFF7A85
#define FOUR 0xFF10EF
#define FIVE 0xFF38C7
#define SIX 0xFF5AA5
#define SEVEN 0xFF42BD
#define EIGHT 0xFF4AB5
#define NINE 0xFF52AD
#define MINUS 0xF076C13B
#define PLUS 0xFFA857
#define EQ 0xFF906F

//int culori[] = {0, 16, 32, 64, 48, 80, 96, 112}; 
int culori[] = { 96, 80, 16, 48, 64, 32}; 
int path[54];
int cube[6][3][3]; 
//Pin connected to ST_CP of 74HC595
int latchPin = 8;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
//Pin connected to DS of 74HC595
int dataPin = 11;

//pentru telecomanda
int RECV_PIN = 3;
IRrecv irrecv(RECV_PIN);
decode_results results;

//pentru delay
int prevMillis = 0;
int prevMillis1 = 0;
int prevMillis2 = 0;
int interval = 1000;

//pentru joystick-uri
//primul joystick
const int x1 = A0;
const int y1 = A1;
const int SW = 2;
int xPosition1 = 0;
int yPosition1 = 0;
int xTranslate1;
int yTranslate1;
int currentPosition1;

//al doilea joystick
const int x2 = A2;
const int y2 = A3;
int xPosition2 = 0;
int yPosition2 = 0;
int xTranslate2;
int yTranslate2;
int currentPosition2;

int ok = 0;
int prevMillis4 = 0;
int front1 = 2;
int line = 1;
int column = 1;
int sw1 = 1;
int lastSw1;
int buttonState = 1;
int reading;
int lastButtonState = 0;
int actualColor;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void makeColor(int color) {
  for (int i = 0; i < 9; i++) {
    shiftOut(dataPin, clockPin, LSBFIRST, color);
  }
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
}

void setAll()
{
  int j = 0, cont = 0;
  for(int i = 0; i < 54; i++){
     path[i] = culori[j];
     shiftOut(dataPin, clockPin, LSBFIRST,culori[j]);
     digitalWrite(latchPin, HIGH);
     cont++;
     if(cont == 9){
      j++;
      cont = 0;
     }
     digitalWrite(latchPin, LOW);
  } 
}

void setLed(int color, int front, int line, int column) {
    for( int i = 0; i < cube[front][line][column]-1; i++){
        shiftOut(dataPin, clockPin, LSBFIRST, path[i]);
    }
    shiftOut(dataPin, clockPin, LSBFIRST, color);
    path[cube[front][line][column]-1] = color;
    for( int i = cube[front][line][column]; i < 54; i++){
        shiftOut(dataPin, clockPin, LSBFIRST, path[i]);
    }
 digitalWrite(latchPin, HIGH);
 digitalWrite(latchPin, LOW);
}

int getColor(int front, int line, int column){
  int position = cube[front][line][column]-1;
    return path[position];
}

//front
// front = 2 este fata din mijloc
// se modifica fetele 0, 1, 3, 4 
void frontReverse(){
  int colorsToSwitch[] = {getColor(0,0,2), getColor(0,1,2), getColor(0,2,2),
                          getColor(1,2,0), getColor(1,2,1), getColor(1,2,2),
                          getColor(4,0,0), getColor(4,1,0), getColor(4,2,0),
                          getColor(3,0,0), getColor(3,0,1), getColor(3,0,2)};
 setLed(colorsToSwitch[0],1,2,2);
 setLed(colorsToSwitch[1],1,2,1);
 setLed(colorsToSwitch[2],1,2,0);
 setLed(colorsToSwitch[3],4,0,0);
 setLed(colorsToSwitch[4],4,1,0);
 setLed(colorsToSwitch[5],4,2,0);
 setLed(colorsToSwitch[6],3,0,2);
 setLed(colorsToSwitch[7],3,0,1);
 setLed(colorsToSwitch[8],3,0,0);
 setLed(colorsToSwitch[9],0,0,2);
 setLed(colorsToSwitch[10],0,1,2);
 setLed(colorsToSwitch[11],0,2,2);
}

//front reverse
//se modifica fetele 0, 1, 3, 4 
void front(){
  int colorsToSwitch[] = {getColor(0,0,2), getColor(0,1,2), getColor(0,2,2),
                          getColor(1,2,0), getColor(1,2,1), getColor(1,2,2),
                          getColor(4,0,0), getColor(4,1,0), getColor(4,2,0),
                          getColor(3,0,0), getColor(3,0,1), getColor(3,0,2)};
 setLed(colorsToSwitch[0],3,0,0);
 setLed(colorsToSwitch[1],3,0,1);
 setLed(colorsToSwitch[2],3,0,2);
 setLed(colorsToSwitch[3],0,2,2);
 setLed(colorsToSwitch[4],0,1,2);
 setLed(colorsToSwitch[5],0,0,2);
 setLed(colorsToSwitch[6],1,2,0);
 setLed(colorsToSwitch[7],1,2,1);
 setLed(colorsToSwitch[8],1,2,2);
 setLed(colorsToSwitch[9],4,2,0);
 setLed(colorsToSwitch[10],4,1,0);
 setLed(colorsToSwitch[11],4,0,0);
}

//right
// se modifica fetele 1,2,3,5
void right(){
  int colorsToSwitch[] = {getColor(1,0,2), getColor(1,1,2), getColor(1,2,2),
                          getColor(2,0,2), getColor(2,1,2), getColor(2,2,2),
                          getColor(3,0,2), getColor(3,1,2), getColor(3,2,2),
                          getColor(5,0,0), getColor(5,1,0), getColor(5,2,0)};
 setLed(colorsToSwitch[0],2,0,2);
 setLed(colorsToSwitch[1],2,1,2);
 setLed(colorsToSwitch[2],2,2,2);
 setLed(colorsToSwitch[3],3,0,2);
 setLed(colorsToSwitch[4],3,1,2);
 setLed(colorsToSwitch[5],3,2,2);
 setLed(colorsToSwitch[6],5,2,0);
 setLed(colorsToSwitch[7],5,1,0);
 setLed(colorsToSwitch[8],5,0,0);
 setLed(colorsToSwitch[9],1,2,2);
 setLed(colorsToSwitch[10],1,1,2);
 setLed(colorsToSwitch[11],1,0,2);
}

//right reverse
// se modifica fetele 1,2,3,5
void rightReverse(){
   int colorsToSwitch[] = {getColor(1,0,2), getColor(1,1,2), getColor(1,2,2),
                          getColor(2,0,2), getColor(2,1,2), getColor(2,2,2),
                          getColor(3,0,2), getColor(3,1,2), getColor(3,2,2),
                          getColor(5,0,0), getColor(5,1,0), getColor(5,2,0)};
 setLed(colorsToSwitch[0],5,2,0);
 setLed(colorsToSwitch[1],5,1,0);
 setLed(colorsToSwitch[2],5,0,0);
 setLed(colorsToSwitch[3],1,0,2);
 setLed(colorsToSwitch[4],1,1,2);
 setLed(colorsToSwitch[5],1,2,2);
 setLed(colorsToSwitch[6],2,0,2);
 setLed(colorsToSwitch[7],2,1,2);
 setLed(colorsToSwitch[8],2,2,2);
 setLed(colorsToSwitch[9],3,2,2);
 setLed(colorsToSwitch[10],3,1,2);
 setLed(colorsToSwitch[11],3,0,2);
}

//down
// se modifica fetele 0, 2, 4, 5
void down(){
   int colorsToSwitch[] = {getColor(0,0,0), getColor(0,0,1), getColor(0,0,2),
                          getColor(2,0,0), getColor(2,0,1), getColor(2,0,2),
                          getColor(4,0,0), getColor(4,0,1), getColor(4,0,2),
                          getColor(5,0,0), getColor(5,0,1), getColor(5,0,2)};
 setLed(colorsToSwitch[0],5,0,0);
 setLed(colorsToSwitch[1],5,0,1);
 setLed(colorsToSwitch[2],5,0,2);
 setLed(colorsToSwitch[3],0,0,0);
 setLed(colorsToSwitch[4],0,0,1);
 setLed(colorsToSwitch[5],0,0,2);
 setLed(colorsToSwitch[6],2,0,0);
 setLed(colorsToSwitch[7],2,0,1);
 setLed(colorsToSwitch[8],2,0,2);
 setLed(colorsToSwitch[9],4,0,0);
 setLed(colorsToSwitch[10],4,0,1);
 setLed(colorsToSwitch[11],4,0,2);
}

//down reverse
// se modifica fetele 0, 2, 4, 5
void downReverse(){
  int colorsToSwitch[] = {getColor(0,0,0), getColor(0,0,1), getColor(0,0,2),
                          getColor(2,0,0), getColor(2,0,1), getColor(2,0,2),
                          getColor(4,0,0), getColor(4,0,1), getColor(4,0,2),
                          getColor(5,0,0), getColor(5,0,1), getColor(5,0,2)};
                          
 setLed(colorsToSwitch[0],2,0,0);
 setLed(colorsToSwitch[1],2,0,1);
 setLed(colorsToSwitch[2],2,0,2);
 setLed(colorsToSwitch[3],4,0,0);
 setLed(colorsToSwitch[4],4,0,1);
 setLed(colorsToSwitch[5],4,0,2);
 setLed(colorsToSwitch[6],5,0,0);
 setLed(colorsToSwitch[7],5,0,1);
 setLed(colorsToSwitch[8],5,0,2);
 setLed(colorsToSwitch[9],0,0,0);
 setLed(colorsToSwitch[10],0,0,1);
 setLed(colorsToSwitch[11],0,0,2);
}

//up
//se modifica fetele 0,2,4,5
void up(){
   int colorsToSwitch[] = {getColor(0,2,0), getColor(0,2,1), getColor(0,2,2),
                          getColor(2,2,0), getColor(2,2,1), getColor(2,2,2),
                          getColor(4,2,0), getColor(4,2,1), getColor(4,2,2),
                          getColor(5,2,0), getColor(5,2,1), getColor(5,2,2)};
 setLed(colorsToSwitch[0],5,2,0);
 setLed(colorsToSwitch[1],5,2,1);
 setLed(colorsToSwitch[2],5,2,2);
 setLed(colorsToSwitch[3],0,2,0);
 setLed(colorsToSwitch[4],0,2,1);
 setLed(colorsToSwitch[5],0,2,2);
 setLed(colorsToSwitch[6],2,2,0);
 setLed(colorsToSwitch[7],2,2,1);
 setLed(colorsToSwitch[8],2,2,2);
 setLed(colorsToSwitch[9],4,2,0);
 setLed(colorsToSwitch[10],4,2,1);
 setLed(colorsToSwitch[11],4,2,2);
}

//up reverse
//se modifica fetele 0,2,4,5
void upReverse(){
  int colorsToSwitch[] = {getColor(0,2,0), getColor(0,2,1), getColor(0,2,2),
                          getColor(2,2,0), getColor(2,2,1), getColor(2,2,2),
                          getColor(4,2,0), getColor(4,2,1), getColor(4,2,2),
                          getColor(5,2,0), getColor(5,2,1), getColor(5,2,2)};
 setLed(colorsToSwitch[0],2,2,0);
 setLed(colorsToSwitch[1],2,2,1);
 setLed(colorsToSwitch[2],2,2,2);
 setLed(colorsToSwitch[3],4,2,0);
 setLed(colorsToSwitch[4],4,2,1);
 setLed(colorsToSwitch[5],4,2,2);
 setLed(colorsToSwitch[6],5,2,0);
 setLed(colorsToSwitch[7],5,2,1);
 setLed(colorsToSwitch[8],5,2,2);
 setLed(colorsToSwitch[9],0,2,0);
 setLed(colorsToSwitch[10],0,2,1);
 setLed(colorsToSwitch[11],0,2,2);
}

//left
//se modifica fetele 1,2,3,5
void left(){
  int colorsToSwitch[] = {getColor(1,0,0), getColor(1,1,0), getColor(1,2,0),
                          getColor(2,0,0), getColor(2,1,0), getColor(2,2,0),
                          getColor(3,0,0), getColor(3,1,0), getColor(3,2,0),
                          getColor(5,0,2), getColor(5,1,2), getColor(5,2,2)};
 setLed(colorsToSwitch[0],2,0,0);
 setLed(colorsToSwitch[1],2,1,0);
 setLed(colorsToSwitch[2],2,2,0);
 setLed(colorsToSwitch[3],3,0,0);
 setLed(colorsToSwitch[4],3,1,0);
 setLed(colorsToSwitch[5],3,2,0);
 setLed(colorsToSwitch[6],5,0,2);
 setLed(colorsToSwitch[7],5,1,2);
 setLed(colorsToSwitch[8],5,2,2);
 setLed(colorsToSwitch[9],1,0,0);
 setLed(colorsToSwitch[10],1,1,0);
 setLed(colorsToSwitch[11],1,2,0);
}

//left reverse
//se modifica fetele 1,2,3,5
void leftReverse(){
  int colorsToSwitch[] = {getColor(1,0,0), getColor(1,1,0), getColor(1,2,0),
                          getColor(2,0,0), getColor(2,1,0), getColor(2,2,0),
                          getColor(3,0,0), getColor(3,1,0), getColor(3,2,0),
                          getColor(5,0,2), getColor(5,1,2), getColor(5,2,2)};
 setLed(colorsToSwitch[0],5,0,2);
 setLed(colorsToSwitch[1],5,1,2);
 setLed(colorsToSwitch[2],5,2,2);
 setLed(colorsToSwitch[3],1,0,0);
 setLed(colorsToSwitch[4],1,1,0);
 setLed(colorsToSwitch[5],1,2,0);
 setLed(colorsToSwitch[6],2,0,0);
 setLed(colorsToSwitch[7],2,1,0);
 setLed(colorsToSwitch[8],2,2,0);
 setLed(colorsToSwitch[9],3,0,0);
 setLed(colorsToSwitch[10],3,1,0);
 setLed(colorsToSwitch[11],3,2,0);
}

//back
//se modifica fetele 0, 1, 3, 4
void back(){
  int colorsToSwitch[] = {getColor(0,0,0), getColor(0,1,0), getColor(0,2,0),
                          getColor(1,0,0), getColor(1,0,1), getColor(1,0,2),
                          getColor(4,0,2), getColor(4,1,2), getColor(4,2,2),
                          getColor(3,2,0), getColor(3,2,1), getColor(3,2,2)};
 setLed(colorsToSwitch[0],3,2,0);
 setLed(colorsToSwitch[1],3,2,1);
 setLed(colorsToSwitch[2],3,2,2);
 setLed(colorsToSwitch[3],0,0,0);
 setLed(colorsToSwitch[4],0,1,0);
 setLed(colorsToSwitch[5],0,2,0);
 setLed(colorsToSwitch[6],1,0,0);
 setLed(colorsToSwitch[7],1,0,1);
 setLed(colorsToSwitch[8],1,0,2);
 setLed(colorsToSwitch[9],4,0,2);
 setLed(colorsToSwitch[10],4,1,2);
 setLed(colorsToSwitch[11],4,2,2);
}

//back reverse
//se modifica fetele 0, 1, 3, 4
void backReverse(){
  int colorsToSwitch[] = {getColor(0,0,0), getColor(0,1,0), getColor(0,2,0),
                          getColor(1,0,0), getColor(1,0,1), getColor(1,0,2),
                          getColor(4,0,2), getColor(4,1,2), getColor(4,2,2),
                          getColor(3,2,0), getColor(3,2,1), getColor(3,2,2)};
 setLed(colorsToSwitch[0],1,0,0);
 setLed(colorsToSwitch[1],1,0,1);
 setLed(colorsToSwitch[2],1,0,2);
 setLed(colorsToSwitch[3],4,0,2);
 setLed(colorsToSwitch[4],4,1,2);
 setLed(colorsToSwitch[5],4,2,2);
 setLed(colorsToSwitch[6],3,2,2);
 setLed(colorsToSwitch[7],3,2,1);
 setLed(colorsToSwitch[8],3,2,0);
 setLed(colorsToSwitch[9],0,0,0);
 setLed(colorsToSwitch[10],0,1,0);
 setLed(colorsToSwitch[11],0,2,0);
}


void buildCube(){
  Serial.println("help");
  setAll();
  Serial.println("I am trapped here forever");
  //1
   cube[0][0][0] = 4;
   cube[0][0][1] = 5;
   cube[0][0][2] = 6;
   cube[0][1][0] = 3;
   cube[0][1][1] = 2;
   cube[0][1][2] = 7;
   cube[0][2][0] = 1;
   cube[0][2][1] = 9;
   cube[0][2][2] = 8;
   
  //2 
   cube[1][0][0] = 13;
   cube[1][0][1] = 14;
   cube[1][0][2] = 15;
   cube[1][1][0] = 12;
   cube[1][1][1] = 11;
   cube[1][1][2] = 16;
   cube[1][2][0] = 10;
   cube[1][2][1] = 18;
   cube[1][2][2] = 17;

   //3
   cube[2][0][0] = 23;
   cube[2][0][1] = 21;
   cube[2][0][2] = 20;
   cube[2][1][0] = 24;
   cube[2][1][1] = 22;
   cube[2][1][2] = 19;
   cube[2][2][0] = 25;
   cube[2][2][1] = 26;
   cube[2][2][2] = 27;

   //4
   cube[3][0][0] = 33;
   cube[3][0][1] = 34;
   cube[3][0][2] = 36;
   cube[3][1][0] = 32;
   cube[3][1][1] = 35;
   cube[3][1][2] = 28;
   cube[3][2][0] = 31;
   cube[3][2][1] = 30;
   cube[3][2][2] = 29;

   //5
   cube[4][0][0] = 40;
   cube[4][0][1] = 38;
   cube[4][0][2] = 37;
   cube[4][1][0] = 41;
   cube[4][1][1] = 39;
   cube[4][1][2] = 45;
   cube[4][2][0] = 42;
   cube[4][2][1] = 43;
   cube[4][2][2] = 44;

   //6
   cube[5][0][0] = 49;
   cube[5][0][1] = 47;
   cube[5][0][2] = 46;
   cube[5][1][0] = 50;
   cube[5][1][1] = 48;
   cube[5][1][2] = 54;
   cube[5][2][0] = 51;
   cube[5][2][1] = 52;
   cube[5][2][2] = 53;
  
}

void setup() {
  //set pins to output because they are addressed in the main loop
  Serial.begin(9600);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  /*
  currentPosition1 = cube[2][1][1];
  currentPosition2 = currentPosition1;
  /*digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(12, LOW);
  digitalWrite(2, LOW);
  digitalWrite(4, LOW);*/
  //digitalWrite(3, LOW);
  //pentru joystick
  pinMode(x1, INPUT);
  pinMode(y1, INPUT);
  pinMode(x2, INPUT);
  pinMode(y2, INPUT);
  pinMode(SW, INPUT);
  digitalWrite(SW, HIGH);
  buildCube();
  irrecv.enableIRIn(); // Start the receiver
  //setLed(BLUE,2,2,2);
  /*for(int i = 0; i < 6; i++){
    for(int j = 0; j < 3; j++){
      for(int k = 0; k < 3; k++){
        Serial.println(getColor(i,j,k));
      }
    }
    Serial.println('\n');
  }*/
}

int thisColor = 0;
/*void blink(int frontH, int lineH, int columnH){
  front1 = frontH;
  line = lineH;
  column = columnH;
  actualColor = cube[frontH][lineH][columnH];
  int currentMillis = millis();
  if(currentMillis - prevMillis <= interval){
    prevMillis = currentMillis;
    if(thisColor == actualColor){
      thisColor = 0;
      setLed(thisColor, front, line, column);
    }
    else if(thisColor != actualColor){
      thisColor = actualColor;
      setLed(thisColor, front, line, column);
    }
    Serial.println("blink");
  }
}*/

void blink(int frontH, int lineH, int columnH){
  int currentMillis = millis();
  if(currentMillis - prevMillis <= interval){
    prevMillis = currentMillis;
    actualColor = cube[frontH][lineH][columnH];
    Serial.println(actualColor);
    //Serial.println(thisColor);
    if(thisColor == actualColor){
      Serial.println("if1");
      thisColor = 0;
      setLed(thisColor, frontH, lineH, columnH);
    }
    else if(thisColor != actualColor){
      //Serial.println("if2");
      thisColor = actualColor;
      setLed(thisColor, frontH, lineH, columnH);
    }
    //Serial.println("blink");
  }
 cube[frontH][lineH][columnH] = actualColor;
}

/*void blink(int front, int line, int column){
  int currentMillis = millis();
  int thisColor = 0;
  if(currentMillis - prevMillis <= interval){
    prevMillis = currentMillis;
    if(thisColor == 0){
      thisColor = getColor(front, line, column);
      setLed(thisColor, front, line, column);
    }
    else if(thisColor != 0){
      thisColor = 0;
      setLed(thisColor, front, line, column);
    }
     Serial.println("blink");
  }
}*/



//joy1
void chooseTheFace(int &front){
  Serial.println("chooseTheFace");
  int currentMillis1 = millis();
  if(currentMillis1 - prevMillis1 >= interval){
    prevMillis1 = currentMillis1;
    xPosition1 = analogRead(x1);
    yPosition1 = analogRead(y1);
    if(xPosition1 > 580 ){
      if( front == 2 || front == 0 || front == 4) 
        front = 1;
      else if( front == 1 ) front = 5;
      else if( front == 5 ) front = 3;
      else if( front == 3) front = 2;
    }
    if(xPosition1 < 450){
      if (front == 0  || front == 2 || front == 4) front = 3;
      else if (front == 1) front = 2;
      else if (front == 5) front = 1;
      else if (front == 3) front = 5;
    }
    if(yPosition1 > 580){
      if (front == 1  || front == 2 || front == 3) front = 0;
      else if (front == 4) front = 2;
      else if (front == 5) front = 4;
      else if (front == 0) front = 5;
    }
    if(yPosition1 < 450){
      if (front == 1  || front == 2 || front == 3) front = 4;
      else if (front == 4) front = 5;
      else if (front == 5) front = 0;
      else if (front == 0) front = 2;
    }
     currentPosition1 = cube[front][1][1];
     blink(front,1,1);
     cube[front][1][1] = actualColor;
     Serial.print(actualColor);
  }
}

void chooseTheLed(int &front, int &line, int &column){
  Serial.println("chooseTheLed");
  int currentMillis2 = millis();
  if(currentMillis2 - prevMillis2 >= interval){
    prevMillis2 = currentMillis2;
    xPosition1 = analogRead(x1);
    yPosition1 = analogRead(y1);
    if(thisColor == 0){
       setLed(actualColor, front, 1, 1);
       thisColor = actualColor;
    }
    if(xPosition1 > 580){ 
      if(column < 2){
        column = column + 1;
      }
      else{
        column = 0; 
      }
    }
    if(xPosition1 < 450){ 
      if(column > 0){
        column = column - 1;
      }
      else{
        column = 2;
      } 
    }
    if(yPosition1 > 580){ 
      if(line < 2){
        line = line + 1;
      }
      else{
        line = 0;
      } 
    }
    if(yPosition1 < 450){ 
      if(line > 0){
        line = line - 1;
      }
      else{
        line = 2;
      } 
    }
    Serial.print(front);
    Serial.print(line);
    Serial.print(column);
    currentPosition2 = cube[front][line][column];
    blink(front, line, column);   
    //cube[front][line][column] = actualColor;
  }
}

int prevMillis3 = 0;
void chooseTheMove(int &front1, int &line, int &column){
   Serial.println("chooseTheMove");
  int currentMillis3 = millis();
  if(currentMillis3 - prevMillis3 >= interval){
    prevMillis3 = currentMillis3;
    xPosition2 = analogRead(x2);
    yPosition2 = analogRead(y2);
    switch(front1){
      case 0:
        if(xPosition2 > 580 && line == 0 && column == 0){
          back();
        }
        if(yPosition2 > 580 && line == 0 && column == 0){
          left();
        }
        if(xPosition2 > 580 && line == 0 && column == 2){
          back();
        }
        if(yPosition2 > 580 && line == 0 && column == 2){
          right();
        }
        if(xPosition2 > 580 && line == 2 && column == 0){
          front();
        }
        if(yPosition2 > 580 && line == 2 && column == 0){
          left();
        }
        if(xPosition2 > 580 && line == 2 && column == 2){
          front();
        }
        if(yPosition2 > 580 && line == 2 && column == 2){
          right();
        }
        if(xPosition2 > 580 && line == 0 && column == 1){
          back();
        }
        if(yPosition2 > 580 && line == 1 && column == 0){
          left();
        }
        if(xPosition2 > 580 && line == 2 && column == 1){
          front();
        }
        if(yPosition2 > 580 && line == 1 && column == 2){
          right();
        }
        if(xPosition2 < 450 && line == 0 && column == 0){
          backReverse();
        }
        if(yPosition2 < 450 && line == 0 && column == 0){
          leftReverse();
        }
        if(xPosition2 < 450 && line == 0 && column == 2){
          backReverse();
        }
        if(yPosition2 < 450 && line == 0 && column == 2){
          rightReverse();
        }
        if(xPosition2 < 450 && line == 2 && column == 0){
          frontReverse();
        }
        if(yPosition2 < 450 && line == 2 && column == 0){
          leftReverse();
        }
        if(xPosition2 < 450 && line == 2 && column == 2){
          frontReverse();
        }
        if(yPosition2 < 450 && line == 2 && column == 2){
          rightReverse();
        }
        if(xPosition2 < 450 && line == 0 && column == 1){
          backReverse();
        }
        if(yPosition2 < 450 && line == 1 && column == 0){
          leftReverse();
        }
        if(xPosition2 < 450 && line == 2 && column == 1){
          frontReverse();
        }
        if(yPosition2 < 450 && line == 1 && column == 2){
          rightReverse();
        }
      break;
      case 1:
        if(xPosition2 > 580 && line == 0 && column == 0){
            up();
          }
          if(yPosition2 > 580 && line == 0 && column == 0){
            front();
          }
          if(xPosition2 > 580 && line == 0 && column == 2){
            up();
          }
          if(yPosition2 > 580 && line == 0 && column == 2){
            back();
          }
          if(xPosition2 > 580 && line == 2 && column == 0){
            down();
          }
          if(yPosition2 > 580 && line == 2 && column == 0){
            front();
          }
          if(xPosition2 > 580 && line == 2 && column == 2){
            down();
          }
          if(yPosition2 > 580 && line == 2 && column == 2){
            back();
          }
          if(xPosition2 > 580 && line == 0 && column == 1){
            up();
          }
          if(yPosition2 > 580 && line == 1 && column == 0){
            front();
          }
          if(xPosition2 > 580 && line == 2 && column == 1){
            down();
          }
          if(yPosition2 > 580 && line == 1 && column == 2){
            back();
          }
          if(xPosition2 < 450 && line == 0 && column == 0){
            upReverse();
          }
          if(yPosition2 < 450 && line == 0 && column == 0){
            frontReverse();
          }
          if(xPosition2 < 450 && line == 0 && column == 2){
            upReverse();
          }
          if(yPosition2 < 450 && line == 0 && column == 2){
            backReverse();
          }
          if(xPosition2 < 450 && line == 2 && column == 0){
            downReverse();
          }
          if(yPosition2 < 450 && line == 2 && column == 0){
            frontReverse();
          }
          if(xPosition2 < 450 && line == 2 && column == 2){
            downReverse();
          }
          if(yPosition2 < 450 && line == 2 && column == 2){
            backReverse();
          }
          if(xPosition2 < 450 && line == 0 && column == 1){
            upReverse();
          }
          if(yPosition2 < 450 && line == 1 && column == 0){
            frontReverse();
          }
          if(xPosition2 < 450 && line == 2 && column == 1){
            downReverse();
          }
          if(yPosition2 < 450 && line == 1 && column == 2){
            backReverse();
          } 
      break;
      case 2:
        if(xPosition2 > 580 && line == 0 && column == 0){
          up();
        }
        if(yPosition2 > 580 && line == 0 && column == 0){
          left();
        }
        if(xPosition2 > 580 && line == 0 && column == 2){
          up();
        }
        if(yPosition2 > 580 && line == 0 && column == 2){
          right();
        }
        if(xPosition2 > 580 && line == 2 && column == 0){
          down();
        }
        if(yPosition2 > 580 && line == 2 && column == 0){
          left();
        }
        if(xPosition2 > 580 && line == 2 && column == 2){
          down();
        }
        if(yPosition2 > 580 && line == 2 && column == 2){
          right();
        }
        if(xPosition2 > 580 && line == 0 && column == 1){
          up();
        }
        if(yPosition2 > 580 && line == 1 && column == 0){
          left();
        }
        if(xPosition2 > 580 && line == 2 && column == 1){
          down();
        }
        if(yPosition2 > 580 && line == 1 && column == 2){
          right();
        }
        if(xPosition2 < 450 && line == 0 && column == 0){
          upReverse();
        }
        if(yPosition2 < 450 && line == 0 && column == 0){
          leftReverse();
        }
        if(xPosition2 < 450 && line == 0 && column == 2){
          upReverse();
        }
        if(yPosition2 < 450 && line == 0 && column == 2){
          rightReverse();
        }
        if(xPosition2 < 450 && line == 2 && column == 0){
          downReverse();
        }
        if(yPosition2 < 450 && line == 2 && column == 0){
          leftReverse();
        }
        if(xPosition2 < 450 && line == 2 && column == 2){
          downReverse();
        }
        if(yPosition2 < 450 && line == 2 && column == 2){
          rightReverse();
        }
        if(xPosition2 < 450 && line == 0 && column == 1){
          upReverse();
        }
        if(yPosition2 < 450 && line == 1 && column == 0){
          leftReverse();
        }
        if(xPosition2 < 450 && line == 2 && column == 1){
          downReverse();
        }
        if(yPosition2 < 450 && line == 1 && column == 2){
          rightReverse();
        }
      break;
      case 3: 
        if(xPosition2 > 580 && line == 0 && column == 0){
          up();
        }
        if(yPosition2 > 580 && line == 0 && column == 0){
          back();
        }
        if(xPosition2 > 580 && line == 0 && column == 2){
          up();
        }
        if(yPosition2 > 580 && line == 0 && column == 2){
          front();
        }
        if(xPosition2 > 580 && line == 2 && column == 0){
          down();
        }
        if(yPosition2 > 580 && line == 2 && column == 0){
          back();
        }
        if(xPosition2 > 580 && line == 2 && column == 2){
          down();
        }
        if(yPosition2 > 580 && line == 2 && column == 2){
          front();
        }
        if(xPosition2 > 580 && line == 0 && column == 1){
          up();
        }
        if(yPosition2 > 580 && line == 1 && column == 0){
          back();
        }
        if(xPosition2 > 580 && line == 2 && column == 1){
          down();
        }
        if(yPosition2 > 580 && line == 1 && column == 2){
          front();
        }
        if(xPosition2 < 450 && line == 0 && column == 0){
          upReverse();
        }
        if(yPosition2 < 450 && line == 0 && column == 0){
          backReverse();
        }
        if(xPosition2 < 450 && line == 0 && column == 2){
          upReverse();
        }
        if(yPosition2 < 450 && line == 0 && column == 2){
          frontReverse();
        }
        if(xPosition2 < 450 && line == 2 && column == 0){
          downReverse();
        }
        if(yPosition2 < 450 && line == 2 && column == 0){
          backReverse();
        }
        if(xPosition2 < 450 && line == 2 && column == 2){
          downReverse();
        }
        if(yPosition2 < 450 && line == 2 && column == 2){
          frontReverse();
        }
        if(xPosition2 < 450 && line == 0 && column == 1){
          upReverse();
        }
        if(yPosition2 < 450 && line == 1 && column == 0){
          backReverse();
        }
        if(xPosition2 < 450 && line == 2 && column == 1){
          downReverse();
        }
        if(yPosition2 < 450 && line == 1 && column == 2){
          frontReverse();
        }
      break;
      case 4:
        if(xPosition2 > 580 && line == 0 && column == 0){
          front();
        }
        if(yPosition2 > 580 && line == 0 && column == 0){
          left();
        }
        if(xPosition2 > 580 && line == 0 && column == 2){
          front();
        }
        if(yPosition2 > 580 && line == 0 && column == 2){
          right();
        }
        if(xPosition2 > 580 && line == 2 && column == 0){
          back();
        }
        if(yPosition2 > 580 && line == 2 && column == 0){
          left();
        }
        if(xPosition2 > 580 && line == 2 && column == 2){
          back();
        }
        if(yPosition2 > 580 && line == 2 && column == 2){
          right();
        }
        if(xPosition2 > 580 && line == 0 && column == 1){
          front();
        }
        if(yPosition2 > 580 && line == 1 && column == 0){
          left();
        }
        if(xPosition2 > 580 && line == 2 && column == 1){
          back();
        }
        if(yPosition2 > 580 && line == 1 && column == 2){
          right();
        }
        if(xPosition2 < 450 && line == 0 && column == 0){
          frontReverse();
        }
        if(yPosition2 < 450 && line == 0 && column == 0){
          leftReverse();
        }
        if(xPosition2 < 450 && line == 0 && column == 2){
          frontReverse();
        }
        if(yPosition2 < 450 && line == 0 && column == 2){
          rightReverse();
        }
        if(xPosition2 < 450 && line == 2 && column == 0){
          backReverse();
        }
        if(yPosition2 < 450 && line == 2 && column == 0){
          leftReverse();
        }
        if(xPosition2 < 450 && line == 2 && column == 2){
          backReverse();
        }
        if(yPosition2 < 450 && line == 2 && column == 2){
          rightReverse();
        }
        if(xPosition2 < 450 && line == 0 && column == 1){
          frontReverse();
        }
        if(yPosition2 < 450 && line == 1 && column == 0){
          leftReverse();
        }
        if(xPosition2 < 450 && line == 2 && column == 1){
          backReverse();
        }
        if(yPosition2 < 450 && line == 1 && column == 2){
          rightReverse();
        }
      break;
      case 5:
        if(xPosition2 > 580 && line == 0 && column == 0){
          down();
        }
        if(yPosition2 > 580 && line == 0 && column == 0){
          left();
        }
        if(xPosition2 > 580 && line == 0 && column == 2){
          down();
        }
        if(yPosition2 > 580 && line == 0 && column == 2){
          right();
        }
        if(xPosition2 > 580 && line == 2 && column == 0){
          up();
        }
        if(yPosition2 > 580 && line == 2 && column == 0){
          left();
        }
        if(xPosition2 > 580 && line == 2 && column == 2){
          up();
        }
        if(yPosition2 > 580 && line == 2 && column == 2){
          right();
        }
        if(xPosition2 > 580 && line == 0 && column == 1){
          down();
        }
        if(yPosition2 > 580 && line == 1 && column == 0){
          left();
        }
        if(xPosition2 > 580 && line == 2 && column == 1){
          up();
        }
        if(yPosition2 > 580 && line == 1 && column == 2){
          right();
        }
        if(xPosition2 < 450 && line == 0 && column == 0){
          downReverse();
        }
        if(yPosition2 < 450 && line == 0 && column == 0){
          leftReverse();
        }
        if(xPosition2 < 450 && line == 0 && column == 2){
          downReverse();
        }
        if(yPosition2 < 450 && line == 0 && column == 2){
          rightReverse();
        }
        if(xPosition2 < 450 && line == 2 && column == 0){
          upReverse();
        }
        if(yPosition2 < 450 && line == 2 && column == 0){
          leftReverse();
        }
        if(xPosition2 < 450 && line == 2 && column == 2){
          upReverse();
        }
        if(yPosition2 < 450 && line == 2 && column == 2){
          rightReverse();
        }
        if(xPosition2 < 450 && line == 0 && column == 1){
          downReverse();
        }
        if(yPosition2 < 450 && line == 1 && column == 0){
          leftReverse();
        }
        if(xPosition2 < 450 && line == 2 && column == 1){
          upReverse();
        }
        if(yPosition2 < 450 && line == 1 && column == 2){
          rightReverse();
        }
      break;
    }
  }
}

void loop() {
  //makeColor(culori[culoare_n++ % 8]);
  //delay(1000);
  int currentMillis4 = millis();
  if(currentMillis4 - prevMillis4 >= interval/2){
    prevMillis4 = currentMillis4;
    sw1 = digitalRead(SW);
    //Serial.println("ok");
    //Serial.println(ok);
    //Serial.println("sw1");
    //Serial.println(sw1);
    
    if (ok == 0 && sw1 == 1){
      chooseTheFace(front1);
    }
    
    /*if( ok == 0 ) {
      //Serial.println("Ok este 0 si stiu asta");
    }
    if( sw1 == 0){
      //Serial.println("Sw1 este 0 si stiu asta");
    }*/
    
    if(sw1 == 0 && ok == 0){
      //Serial.println("Ok si sw1 sunt 0 si stiu asta");
      ok = 1;
      sw1 = 1;
    }
    if(ok == 1 && sw1 == 1){
      chooseTheLed(front1, line, column);
    } 
    if(sw1 == 0 && ok == 1){
      ok = 2;
      sw1 = 1;
    }
    if(ok == 2 && sw1 == 1){
      chooseTheMove(front1, line, column);
    }
    if(sw1 == 0 && ok == 2){
      ok = 0;
      sw1 = 1;
    }
  }
}
