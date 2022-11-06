#include<Servo.h>
int buzzer = 3;
int trig = 7;
int echo = 8;
int servo = 13;
int pump = 4;
int sensor = 9;
int senIn = 6;
int senOut = 5;
int latchPin = 10;
int dataPin = 11;
int clockPin = 12;
int bottleHeight = 18;
int state = LOW;
int numbers[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
Servo barrier;
void setup()
{
    pinMode(pump, OUTPUT);
    pinMode(servo, OUTPUT);
    digitalWrite(pump, HIGH);
    Serial.begin(9600);
    barrier.attach(13);
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
    pinMode(servo, OUTPUT);         
    pinMode(sensor, INPUT);
    pinMode(senIn, INPUT);
    pinMode(senOut, INPUT);
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT); 
    barrier.write(0);
}

void printOut (int i) {
      if (i < 0) {i = 0;};
      if (i > 9) {i = 9;};
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST,numbers[i]);
      digitalWrite(latchPin, HIGH);
}

void measureFluid(void *pv){
        unsigned long duration;
        int distance;
        digitalWrite(trig, 0);
        delayMicroseconds(2);
        digitalWrite(trig, 1);
        delayMicroseconds(5);
        digitalWrite(trig, 0);
        duration = pulseIn(echo, HIGH);
        distance = int(duration / 2 / 29.412);
        int fluidLvl = 0;
        int fluidLvlPC = 0;
        if (bottleHeight > distance) {
           fluidLvl = bottleHeight - distance;
           fluidLvlPC = (fluidLvl * 10) / bottleHeight;
           }
        else {
           fluidLvl = 0;
        }
        printOut(fluidLvlPC);
        Serial.println(fluidLvlPC);
        delay(200);
}

void activatePump(){
    digitalWrite(pump, LOW);
    delay(2000);
    digitalWrite(pump, HIGH);
}

void cleanPump() {
    digitalWrite(pump, LOW);
    delay(10000);
    digitalWrite(pump, HIGH);
}

void getIn() {
   int val = digitalRead(senIn);
   barrier.write(0);
   if (val == HIGH) {
    Serial.println("IN");
    barrier.write(90);
    delay(1000);
    barrier.write(45);
    barrier.write(0);
    delay(5);
    }
}

void getOut() {
  int val = digitalRead(senOut);
  barrier.write(0);
  if (val == HIGH) {
    Serial.println("OUT");
    barrier.write(90);
    delay(1000);
    barrier.write(45);
    barrier.write(0);
    delay(5);
  }
}

int mainOperation(void* pv){
        int val = digitalRead(sensor);
        if (val == HIGH){
            if (state == LOW){
                Serial.println("Motion detected");
                activatePump();
                state = HIGH;
                return 1;
            }
        }

        else{
            if (state == HIGH){
                Serial.println("Motion stopped");
                digitalWrite(pump, HIGH);
                state = LOW;
                return 0;
            }
        }
}

void loop(){
  if (mainOperation(NULL)) {
    measureFluid(NULL);
    getIn();
    }
    getOut();
  }
