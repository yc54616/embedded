#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int fsrsensor = A0;
int ena = 2;
int in2 = 3;
int in1 = 4;
int button = 13;
int fs_led = 12;
int echo = 5;
int trig = 6;


bool warning = false;
bool push = false;
int trash_mount = 0;

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
  pinMode(ena, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(fs_led, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  digitalWrite(ena, 255);
  lcd.begin();
  lcd.createChar(0, quadrilateral);
  logo();
}

void logo(){
  lcd.setCursor(0, 0);
  lcd.print("Freeze Compress");
  lcd.setCursor(2, 1);
  lcd.print("Trash Can v1");
  delay(2000);
  lcd.clear();
}

void trash_size(int dis){
  trash_mount = dis;
  lcd.clear();
  for(int i = 0; i <= dis; i++){
    lcd.setCursor(i,0);
    lcd.write(byte(0)); 
    lcd.setCursor(i,1);
    lcd.write(byte(0));
  }   
  delay(500);
}

void loop() {
  int button_status = digitalRead(button);
  int fsr_status = analogRead(fsrsensor);
  
  if(fsr_status > 900){
    analogWrite(fs_led, 255);
    warning = true;
  }
  else
    analogWrite(fs_led, 0);
    
  if(warning){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    delay(5000);
    warning = false;
  }
  else if(button_status){
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  else if(trash_mount > 12 && !push){
    push = true;
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH); 
    delay(5000);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW); 
    delay(5000);
  }
  else{
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }

  digitalWrite(trig, LOW);
  delay(2);
  digitalWrite(trig, HIGH);
  delay(10);
  digitalWrite(trig, LOW);
  float dis = ((float)(340*pulseIn(echo, HIGH))/10000)/2;
  trash_size((36-dis)/2.125);
  Serial.println(dis);
  Serial.println((36-dis)/2.125);

  if(millis()%100000/10000 == 9){
    push = false;
  }
  
}
