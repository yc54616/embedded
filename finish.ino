#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial esp(8, 9); // RX, TX

int tones = 7;
int ena = 2;
int in2 = 3;
int in1 = 4;
int echo = 5;
int trig = 6;

unsigned long time_previous[5], time_current[5];
int check = 0;
int trash = 0;
int percent = 0;

byte quadrilateral[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

void setup() {
  Serial.begin(9600);
  esp.begin(9600);
  pinMode(ena, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  digitalWrite(ena, 255);
  lcd.begin();
  lcd.createChar(0, quadrilateral);
  logo();
  esp.println("AT+CIPSTART=\"TCP\",\"121.129.110.5\",80");
  for(int i = 0; i < 5; i++)
    time_previous[i] = millis();
}

void logo(){
  lcd.setCursor(0, 0);
  lcd.print("Freeze Compress");
  lcd.setCursor(2, 1);
  lcd.print("Trash Can v3");
  delay(2000);
  lcd.clear();
}

int set_trash(float dis){
  if(22 < dis && dis <= 24)
    trash = 1;
  else if(21 < dis && dis <= 22)
    trash = 2;
  else if(19 < dis && dis <= 21)
    trash = 3;
  else if(18 < dis && dis <= 19)
    trash = 4;
  else if(17 < dis && dis <= 18)
    trash = 5;
  else if(15 < dis && dis <= 17)
    trash = 6;
  else if(14 < dis && dis <= 15)
    trash = 7;
  else if(13 < dis && dis <= 14)
    trash = 8;
  else if(11 < dis && dis <= 13)
    trash = 9;
  else if(10 < dis && dis <= 11)
    trash = 10;
  else if(9 < dis && dis <= 10)
    trash = 11;
  else if(7 < dis && dis <= 9)
    trash = 12;
  else if(6 < dis && dis <= 7)
    trash = 13;
  else if(5 < dis && dis <= 6)
    trash = 14;
  else if(4 <= dis && dis <= 5)
    trash = 15;
  else if(dis <= 3)
    trash = 16;
  return trash;
}

void trash_size(int dis){
  for(int i = 0; i <= dis; i++){
    lcd.setCursor(i,0);
    lcd.write(byte(0)); 
    lcd.setCursor(i,1);
    lcd.write(byte(0));
  }   
}

void loop() {
  for(int i = 0; i < 5; i++)
    time_current[i] = millis();

  digitalWrite(trig, LOW);
  if(time_current[0] - time_previous[0] >= 2){
    time_previous[0] = time_current[0];
    digitalWrite(trig, HIGH);
  }
  if(time_current[1] - time_previous[1] >= 10){
    time_previous[1] = time_current[1];
    digitalWrite(trig, LOW);
  }
  float dis = ((float)(340*pulseIn(echo, HIGH))/10000)/2; //쓰레기양 구하기
  
  if(dis > 1){
    Serial.print("dis :");
    Serial.println(dis);
    trash = set_trash(dis);
    if(time_current[2] - time_previous[2] >= 200){
      time_previous[2] = time_current[2];
      lcd.clear();
      trash_size(trash);
    }
    percent = (float)((int)(trash))/16*100; //LED 표시 후 퍼센트
    Serial.print("percent :");
    Serial.println(percent);
  }
  if(80 < percent && percent <= 100 && !check){
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      delay(20000);
      check = 1;
    }
  else{
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
    
  if(time_current[3] - time_previous[3] >= 300000){
    time_previous[3] = time_current[3];
    check = 0;
  } //시간 측정 후 압축
  tone(tones,20000);
  Serial.print("check :");
  Serial.println(check);
  
  if(time_current[4] - time_previous[4] >= 1000){
    time_previous[4] = time_current[4];
    if(percent < 100){
      String sendnum = "GET /iot/?title=%EB%8F%99%EA%B5%AD%EB%8C%80%EC%9E%85%EA%B5%AC%EC%95%B0%EB%B0%B0%EC%84%9C%EB%8D%94%ED%98%B8%ED%85%94&amount="+String(percent)+"&x=37.5597717&y=127.0026102 HTTP/1.1";
      int lengg = sendnum.length() + 4;
      String cipsend = "AT+CIPSEND="+String(lengg);
      esp.println(cipsend);
      esp.println(sendnum);
      esp.println();
    }
  } //data send
}
