#include <SoftwareSerial.h>

#define L_IN1      2
#define L_IN2     3
#define R_IN1     6
#define R_IN2     7
#define L_ENABLE    4
#define R_ENABLE    9
#define L_PWM     5
#define R_PWM     10
#define LEFT_PWM_VALUE  100
#define RIGHT_PWM_VALUE 100

// Bluetooth PIN 설정
int blueTx=A0;   //Tx (보내는핀 설정)
int blueRx=11;   //Rx (받는핀 설정)
SoftwareSerial mySerial(blueTx, blueRx);  //시리얼 통신을 위한 객체선언
String myString=""; //받는 문자열

//초음파 센서의 핀번호를 설정한다.
int echoPin = 12;
int trigPin = 13;
 
void init_motor();
void move_forward();
void spin_left();
void spin_right();
void stop();
float distance =0;
int cnt = 0;

//distance
int BACKDISTANCE=15;
int SHORTDISTANCE=30;
int MIDDISTANCE=50;
int LONGDISTANCE=80;
int FINDLIMIT=16;
//bluetooth
int bt=0;

int LED=1;
int BTLED=0;

// time
unsigned long time;
unsigned long stoppedtime;
int STOPACTIONDELAY = 5000;

void setup() {
  // put your setup code here, to run once:
  pinMode(L_IN1, OUTPUT);
  pinMode(L_IN2, OUTPUT);
  pinMode(L_ENABLE, OUTPUT);
  pinMode(L_PWM, OUTPUT);

  pinMode(R_IN1, OUTPUT);
  pinMode(R_IN2, OUTPUT);
  pinMode(R_PWM, OUTPUT);
  pinMode(R_ENABLE, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  //블루투스 시리얼 개방
  mySerial.begin ( 9600 );

  pinMode(LED, OUTPUT);

  getTime();

}

void loop() {
  while(mySerial.available())  //mySerial 값이 있으면
  {
    char myChar = (char)mySerial.read();  //mySerial int형식의 값을 char형식으로 변환
    myString+=myChar;   //수신되는 문자열을 myString에 모두 붙임 (1바이트씩 전송되는 것을 모두 붙임)
    delay(5);           //수신 문자열 끊김 방지
  }

  if (myString!=""){
      Serial.print("Debug:");
      Serial.println(myString);
    if(myString=="on" || myString=="onon") {
      //init
      mySerial.write("Good afternoon Mr. Jung Hoon Lee. Welcome to Incehon Airport Terminal 1\n");
      mySerial.write("Your flight schedule from Korea to San Francisco with Gate 42 at 16:00\n");
      mySerial.write("Do you need guide with Robot Cart? Press Guide! \n");
      mySerial.write("\n");
      mySerial.write("Airport map information:  Press Airport Infomation! \n");
      mySerial.write("\n");
      myString="";
      bt=1;
      resetAlertCount();
    }else if(myString=="off" || myString=="offoff"){
      mySerial.write("San Francisco's weather is raining.\n");
      mySerial.write("Have a nice flight Mr. Jung Hoon Lee.\n");
      bt=0;
      myString="";
      setBluetoothOff();
    }else if(myString=="stop" || myString=="stopstop"){
      Serial.println("stopped");
      bt=2;
      myString="";
      setBluetoothStop();
      getStoppedTime();  //stop time set
    }else if(myString=="def"){
      myString="";
    }else if(myString=="coupon"){
      getCoupon();
      myString="";
    }else if(myString=="go" || myString=="gogo"){
      forward();
      delay(200);
      stop();
      myString="";
    }else if(myString=="back" || myString=="backback"){
      backward();
      delay(200);
      stop();
      myString="";
    }else if(myString=="right"  || myString=="rightright"){
      right();
      delay(100);
      stop();
      myString="";
    }else if(myString=="left" || myString=="leftleft"){
      left();
      delay(100);
      stop();
      myString="";
    }else if(myString=="manual"  || myString=="manualmanual"){
      myString="";
    }else{
      myString="";
    }
  }
      
  if (bt ==0){
    delay(1000);
    setBluetoothOff();
  }if (bt ==2){
    getTime();
    setStoppedAction();
    delay(STOPACTIONDELAY);
  }else if (bt ==1){
    setBluetoothOn();
    getDistance();
    delay(10);
    Serial.print("distance:");
    Serial.println(distance);
  
    delAlarm();
    if (distance>SHORTDISTANCE && distance<=MIDDISTANCE){
      resetAlertCount();
    }
    
    if (distance>BACKDISTANCE && distance <=SHORTDISTANCE && cnt<FINDLIMIT){
      stop();
    }else if (distance>SHORTDISTANCE && distance<=MIDDISTANCE && cnt<FINDLIMIT){
      forward();
    }else if (distance>MIDDISTANCE && distance<LONGDISTANCE && cnt<FINDLIMIT){
      forwardfast();
    }else if (distance==0 || distance>=LONGDISTANCE && cnt<FINDLIMIT){
      setAlarm();
      waitPerson();
      if (distance==0 || distance>=LONGDISTANCE && cnt<FINDLIMIT){
        for (int i=0; i<FINDLIMIT; i++){
          findPerson();
          waitPerson();
          if (distance > 0 && distance <LONGDISTANCE) {
            resetAlertCount();     
            break;
          }
        } 
      }
    }else if (distance>0 && distance <=BACKDISTANCE && cnt<FINDLIMIT){
      //stop();
      backward();
    }
  }
}
void setStoppedAction(){
  Serial.print("stopped: ");
  Serial.print(time-stoppedtime);
  Serial.print("\n");
  if((time - stoppedtime) > STOPACTIONDELAY*1) {
     if ((time - stoppedtime) < STOPACTIONDELAY*2) {
        mySerial.write("New Coupon is arrived with 20% discount\n");
        mySerial.write("linkurl : ");
        mySerial.write("http://www.shilladfs.com/estore/kr/ko/couponzone?actr=1\n");
        mySerial.write("\n");
        mySerial.write("\n");
     }else if((time - stoppedtime) < STOPACTIONDELAY*3) {
        mySerial.write("Do you need coffee or teas? There is one near by you\n");
        mySerial.write("linkurl : ");
        mySerial.write("http://starbucks.com\n");
        mySerial.write("\n");
        mySerial.write("\n");
     }else if((time - stoppedtime) < STOPACTIONDELAY*4) {
        mySerial.write("Would you like to see San Francisco sightseeing spots?\n");
        mySerial.write("linkurl : ");
        mySerial.write("http://www.verygoodtour.com/sanfrancisco\n");
        mySerial.write("\n");
        mySerial.write("\n");
     }else if((time - stoppedtime) < STOPACTIONDELAY*5){
        mySerial.write("Please click coupon button to get discount coupon\n");
        mySerial.write("\n");
        mySerial.write("\n");
     }
  }
}
void getCoupon(){
  mySerial.write("Hermes bag discount coupon is arrived\n");
  mySerial.write("linkurl : ");
  mySerial.write("http://www.hermes.com/estore/kr/ko/couponzone?actr=1\n");
}

void getTime(){  
  Serial.print("Time: ");
  time = millis();
  Serial.println(time);
}
void getStoppedTime(){  
  Serial.print("Time: ");
  stoppedtime = millis();
  Serial.println(time);
}
void getDistance(){
  float duration;
  // 초음파를 보낸다. 다 보내면 echo가 HIGH 상태로 대기하게 된다.
  digitalWrite(trigPin, HIGH);
  delay(10);
  digitalWrite(trigPin, LOW);
  
  // echoPin 이 HIGH를 유지한 시간을 저장 한다.
  duration = pulseIn(echoPin, HIGH); 

  // HIGH 였을 때 시간(초음파가 보냈다가 다시 들어온 시간)을 가지고 거리를 계산 한다.
  distance = ((float)(340 * duration) / 10000) / 2;  
  Serial.print("\nDIstance : ");
  Serial.println(distance);
}

void setAlarm(){
  digitalWrite( LED , HIGH); //LED ON
  delay(10);
}
void delAlarm(){
  digitalWrite( LED , LOW); //LED OFF  
}

void setMessage(){
  
}

void setBluetoothOn(){
  digitalWrite( BTLED , HIGH); //LED ON
  delay(100);
  digitalWrite( BTLED , LOW); //LED OFF
}
void setBluetoothOff(){
  digitalWrite( BTLED , LOW); //LED ON
}
void setBluetoothStop(){
  digitalWrite( BTLED , HIGH); //LED ON
  delay(200);
  digitalWrite( BTLED , LOW); //LED OFF
}

void resetAlertCount(){
  cnt=0;
}

void waitPerson(){
    delay(1000);
    getDistance();
}

void findPerson(){
    right();
    delay(200);
    stop();
    delay(100);
    getDistance();
    cnt++;
}

void forward(){
    move();
    delay(100);
    stop();
    delay(100);
    getDistance();
}

void forwardfast(){
    move();
    delay(200);
    stop();
    delay(50);
    getDistance();
}

void move(){
  analogWrite(L_PWM, LEFT_PWM_VALUE);
  analogWrite(R_PWM,RIGHT_PWM_VALUE);
  digitalWrite(L_ENABLE, 0);
  digitalWrite(L_IN1, 1);
  digitalWrite(L_IN2, 0);
  digitalWrite(R_ENABLE, 0);
  digitalWrite(R_IN1, 1);
  digitalWrite(R_IN2, 0);
  delay(100);
}


void left(){
  digitalWrite(L_ENABLE, 0);
  digitalWrite(L_IN1, 0);
  digitalWrite(L_IN2, 1);
  digitalWrite(R_ENABLE, 0);
  digitalWrite(R_IN1, 1);
  digitalWrite(R_IN2, 0);
  analogWrite(L_PWM, LEFT_PWM_VALUE);
  analogWrite(R_PWM,RIGHT_PWM_VALUE);
}

void right(){
  analogWrite(L_PWM, LEFT_PWM_VALUE);
  analogWrite(R_PWM,RIGHT_PWM_VALUE);
  digitalWrite(L_ENABLE, 0);
  digitalWrite(L_IN1, 1);
  digitalWrite(L_IN2, 0);
  digitalWrite(R_ENABLE, 0);
  digitalWrite(R_IN1, 0);
  digitalWrite(R_IN2, 1);
}

void stop(){
  digitalWrite(L_IN1, 1);
  digitalWrite(L_IN2, 1);
  digitalWrite(R_IN1, 1);
  digitalWrite(R_IN2, 1);
}

void backward(){
  analogWrite(L_PWM, LEFT_PWM_VALUE);
  analogWrite(R_PWM,RIGHT_PWM_VALUE);
  digitalWrite(L_ENABLE, 0);
  digitalWrite(L_IN1, 0);
  digitalWrite(L_IN2, 1);
  digitalWrite(R_ENABLE, 0);
  digitalWrite(R_IN1, 0);
  digitalWrite(R_IN2, 1);
    delay(200);
    stop();
    delay(100);
}
