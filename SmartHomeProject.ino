
#include <Servo.h>
#include "pitches.h"
#include <SoftwareSerial.h>

Servo myservo; // 서보모터 객체 생성

#define REDPIN  31//RGB LED 모듈의 빨간색핀을 13번핀으로 선언
#define GREENPIN 33 //RGB LED 모듈의 초록색핀을 12번핀으로 선언
#define BLUEPIN 35 //RGB LED 모듈의 파란색핀을 11번핀으로 선언


SoftwareSerial softSerial(10,11); // 시리얼 통신핀 설정, 이름을 BT로 설정, 10번핀을 RX, 11 3번핀을 뭐시기로 설정
int data = 0;


int CDS = A0; //조도센서 pin 지정
int LEDPIN1 = 53; // 현관문 불빛
int motor1 = 4;// 조도센서 창문용 서보모터
int infrared = 8; //적외선
int buzzer = 6; //화장실 음악부저 
int pushbtn = 2; //화장실 버튼 
int LEDPIN3 = 51; //화장실 불
int LEDPIN2 = 13; //안방 LED
int motor2 = 49; //주차장 문 
int motor3 = 47; // 안방 창문
int cdspower = 7;


boolean ledstate = false;

byte r = 0; //앱에서 받아오는 빨간색 RGB 값을 저장하는 변수
byte g = 0; //앱에서 받아오는 초록색 RGB 값을 저장하는 변수
byte b = 0; //앱에서 받아오는 파란색 RGB 값을 저장하는 변수

int melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0, 0,
  NOTE_G6, 0, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0

};

int songLength = sizeof(melody) / sizeof(melody[0]);

int durations[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};



void setup() {

  Serial.begin(9600);
  pinMode(CDS, INPUT);
  pinMode(LEDPIN1, OUTPUT);
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(motor3, OUTPUT);
  myservo.attach(motor1);
  myservo.attach(motor2);
  myservo.attach(motor1);
  pinMode(infrared, INPUT);
  pinMode(cdspower, OUTPUT);
  pinMode(LEDPIN2, OUTPUT);
  pinMode(pushbtn, INPUT_PULLUP);
  pinMode(LEDPIN3, OUTPUT);
  attachInterrupt(0, swInterrupt, FALLING);
  digitalWrite(LEDPIN3, LOW);

  softSerial.begin(9600);

  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

}

void loop() {
if(softSerial.available() > 0){
      data = softSerial.read();
    
    if(data == 1) //거실 LED 켜짐
    { geosil(); }
    else if (data == 2) //거실 LED 꺼짐
    { geosil2(); }

    else if(data == 8) //조도센서 창문 자동
    {  digitalWrite(cdspower,HIGH);
       servojodo();
    }
    
    else if(data == 3) //조도센서 창문 열림.
    {  digitalWrite(cdspower,LOW);
       myservo.write(180);
       digitalWrite(motor1,HIGH);
    }

    else if(data == 4) //조도센서 창문 닫힘.
    {   digitalWrite(cdspower,LOW);
        myservo.write(-180);
        digitalWrite(motor1, LOW);
    }
    else if(data ==5) //차고 문 열림.
    {
      myservo.write(180);
      digitalWrite(motor2,HIGH);
    }
    else if(data == 6) //차고 문 닫힘.
    {
      myservo.write(180);
      digitalWrite(motor2,LOW);
    }

    else if(data == 7) //화장실 음악 재생.
    {
         toilet();
        
    }
//    else if(data == 8)
//    {
//      
//    }

}
jodo();
servojodo();
door();
toilet();

}








//조도센서 led on
void jodo(){
  int CDS = analogRead(A0);
  Serial.print("CDS_sensor: ");
  Serial.println(CDS);

  if(CDS > 200) {
    
    Serial.println("LED ON");
  }

  else{
  
    Serial.println("LED OFF");
  }
  delay(200);
  
}

//조도센서를 활용창문
void servojodo(){
  int a = analogRead(A0);
  Serial.print("bright = ");
  Serial.print(a);
  Serial.print("\n");
  delay(300);


  if(a < 200)
  {
    myservo.write(180);
    digitalWrite(motor1,HIGH);
    Serial.println("LED OFF");
  }
  else{
    myservo.write(-180);
    digitalWrite(motor1, LOW);
    Serial.println("LED ON");
  }
}

//현관문 적외선 감지 센서 
void door(){
  int state = digitalRead(infrared);

  if(state == 0){
    digitalWrite(LEDPIN1, HIGH); //현관문 불빛 on 
    softSerial.print(1);
  }
  else{

    digitalWrite(LEDPIN1, LOW); //현관문 불빛 off
    softSerial.print(0);
  }
}

//화장실 패시브부저,버튼
void toilet(){
  if (ledstate == true) {
   for (int thisNote = 0; thisNote < songLength; thisNote++) {
            int duration = 1000 / durations[thisNote];
            tone(buzzer, melody[thisNote], duration);
            int pause = duration * 1.3;
            delay(pause);
        }
     }
     else  noTone(buzzer);
}

void swInterrupt(){
   delayMicroseconds(2000);
  if (digitalRead(pushbtn) != LOW) return ;

  if (ledstate == false) {
     digitalWrite(LEDPIN3, HIGH);
     ledstate = true;
    
    }
    else {
         digitalWrite(LEDPIN3, LOW);
         ledstate = false;
           }
}

void geosil(){



//    analogWrite(REDPIN, 255);
//    analogWrite(GREENPIN, 255);
//    analogWrite(BLUEPIN, 255);

    if(softSerial.available() > 0){
    r = softSerial.read();
    g = softSerial.read();
    b = softSerial.read();

   softSerial.flush();

    Serial.print("Red = ");
    Serial.println(r);
    Serial.print("Green = ");
    Serial.println(g);
    Serial.print("Blue = ");
    Serial.println(b);

    analogWrite(REDPIN, r);
    analogWrite(GREENPIN, g);
    analogWrite(BLUEPIN, b);
  }
  
}

void geosil2(){ //거실 불 끈거



    analogWrite(REDPIN,0);
    analogWrite(GREENPIN, 0);
    analogWrite(BLUEPIN, 0);
}
