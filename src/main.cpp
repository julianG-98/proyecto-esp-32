#include <Arduino.h>
#include <LiquidCrystal_I2C.h> // pantalla
#include <ESP32Servo.h>		//library for ESP32 to use servos with PWM
#include <Wire.h> 


LiquidCrystal_I2C lcd(0X27, 16, 2);

int receivedInt =0;

int laser =18;

//servo
int serv1=12;
int serv2=14;
int serv3=27;
int serv4=26;

// I2C SDA 21 SCL 22

// motor paso a paso
class motorp{
  
  public:
    int pdirec;
    int ppasos;
    int step = 1.8;
    float gearR=1 ;
    int id = 0;
    int mo =0;

    void setup(){
    pinMode(pdirec, OUTPUT);
    pinMode(ppasos, OUTPUT);
    }

    void move(float angel){

      int mo = std::round((abs(angel)*gearR)/step);

      if (angel> 0){
        digitalWrite(pdirec, HIGH);

        for(int cn_pasos=0;cn_pasos< mo;cn_pasos++){
          digitalWrite(ppasos,HIGH);
          delay(3);
          digitalWrite(ppasos,LOW);}
      }

      if (angel < 0){
        digitalWrite(pdirec, LOW);
        
        for(int cn_pasos=0;cn_pasos<= mo;cn_pasos++){
          digitalWrite(ppasos,HIGH);
          delay(3);
          digitalWrite(ppasos,LOW);}
      }

      delay(3);
      Serial.print(id);
      Serial.println(angel);
      // Serial.print(",");
      // Serial.prinln(mo);
      

    }

};

//creat 3 servo objects for the pwm servos
Servo servo1; // rotacionb laser
Servo servo2; // inclinacion laser
Servo servo3; // paro disparo
Servo servo4; // paro 2

//motor paso a paso
motorp m1;
motorp m2;
motorp m3;


class panelcontrol{

  public:
  int Bot1=25;
  int Bot2=33;
  int Bot3=32;
  int Bot4=35;
  int conf= 34;

  int fin1=36; //rota
  int fin2=23; //inclinacion
  int fin3=39; //resorte


  int laser =18;

  float offsetlX=6.50;
  float offsetlY=16.0;

  float angus1=0;
  float angus2=0;


  float dist=30;
  float ydes=0;
  float angulo=0;
  float bangulo=0;
  float rota =0;
  float brota =0;
  float compresion =0;
  float bcompresion =0;




 void setup(){

  pinMode(Bot1, INPUT_PULLDOWN);
  pinMode(Bot2, INPUT_PULLDOWN);
  pinMode(Bot3, INPUT_PULLDOWN);
  pinMode(Bot4, INPUT);
  pinMode(conf, INPUT);

  pinMode(fin1, INPUT); //abierto 1 es que llega al final 
  pinMode(fin2, INPUT_PULLUP); //abierto 
  pinMode(fin3, INPUT); // 0 es que esta en el final

  pinMode(laser, OUTPUT);



 }
  
 void inclimove (motorp m){
  
  m.move(angulo-bangulo);
  bangulo=angulo;
 }

  void rotamove (motorp m){
  
  m.move(rota-brota);
  brota=rota;
 }

 void compremove (motorp m){
 
  m.move(compresion-bcompresion);
  bcompresion=compresion;
 }

 void distmove (){
  //calculo angulo1 y 2
  angus1= 90-(atan(dist/offsetlX)* 180 / 3.1416);
  angus2= 90-(atan((offsetlY + ydes)/(sqrt(pow(dist,2)+pow(offsetlX,2))))* 180 / 3.1416);
  Serial.print("A1= ");
  Serial.print(angus1);
  Serial.print(" ");
  Serial.print("A2= ");
  Serial.println(angus2); 

  servo1.write(angus1);
  servo2.write(angus2);

 }

 void returncom(motorp m, Servo s){
  while(digitalRead(fin3)==HIGH){
    m.move(0.1);
    Serial.println(".");
  }
  Serial.println("CR");
  s.write(0);

  //m.move(1);
}
void returnrot(motorp m){
  while(digitalRead(fin1)==LOW){
    m.move(-1);
  }
  m.move(90);
}
void returnin(motorp m){
  while(digitalRead(fin2)==LOW){
    m.move(-1);
  }
  //m.move(1);
}
void shoot(){
  servo3.write(0);
  delay(2);
  servo4.write(100);

  //serial.println("disparo");
}

void hold(){
  servo3.write(100);
  delay(2);
  servo4.write(0);

  //serial.println("disparo");
}
};

panelcontrol p1;

int bb1=0;
bool b1 = false;
bool b2 = false;
bool b3 = false;
bool b4 = false;
int selm = 0;

//lectura de botones 


void updateLCD(){
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Y=");
  lcd.print((int)p1.ydes);

  lcd.print("D=");
  lcd.print((int)p1.dist);

  lcd.setCursor(15,0);
  lcd.print((int)selm);

  lcd.setCursor(0,1); 

  lcd.print("A=");
  lcd.print((int)p1.rota);

  lcd.print("B=");
  lcd.print((int)p1.angulo);

  lcd.print("C=");
  lcd.print((int)p1.compresion);

  b1 = false;
  b2 = false;
  b3 = false;
  b4 = false;

  //delay(5);

}

void read_bot(){
  if (digitalRead(p1.Bot1)== HIGH){
    delay(10);
    if (selm == 1 ){
      p1.distmove();
    }

    if (selm == 2 ){
      p1.rotamove(m1);
    }

    if (selm == 3 ){
      p1.inclimove(m2);

    }

    if (selm == 4 ){
      p1.compremove(m3);

    }

    selm = selm+1;

    if(selm > 4){
        selm=0;
      }

      // lcd.clear();
      //lcd.setCursor(0,0);
      //lcd.print("B1 on");
      //bb1=bb1+1;

      Serial.println("b1");
      Serial.println(selm);
      //delay(100);
      b1 = true;} 

  if (digitalRead(p1.Bot2)== HIGH){
    delay(10);

    switch (selm) {
    case 0:
      // code to be executed if expression == constant1
      p1.ydes=p1.ydes+1;

      Serial.println(p1.ydes);
      break;
    case 1:
      // code to be executed if expression == constant2
      p1.dist=p1.dist+1;
      Serial.println(p1.dist);
      break;

    case 2:
      p1.rota=p1.rota+1;
      Serial.println(p1.rota);

      
      // code to be executed if expression == constant2
      //select=select+1;
      break;

    case 3:
      p1.angulo=p1.angulo+1;
      Serial.println(p1.angulo);
      // code to be executed if expression == constant2
      //select=0;
      break;

    case 4:
      p1.compresion=p1.compresion+1;
      Serial.println(p1.compresion);
      // code to be executed if expression == constant2
      //select=0;
      break;

    default:
    
      break;
        // code to be executed if expression doesn't match any case
    }
    
    Serial.println("b2");
  

  }

  if (digitalRead(p1.Bot3)== HIGH){
    delay(10);
  switch (selm) {
    case 0:
      // code to be executed if expression == constant1
      p1.ydes=p1.ydes-1;
      Serial.println(p1.ydes);
      break;
    case 1:
      // code to be executed if expression == constant2
      p1.dist=p1.dist-1;
      Serial.println(p1.dist);
      break;
    case 2:
    p1.rota=p1.rota-1;
    Serial.println(p1.rota);
    // code to be executed if expression == constant2
    //select=0;
    break;

    case 3:
      p1.angulo=p1.angulo-1;
      Serial.println(p1.angulo);
      // code to be executed if expression == constant2
      //select=0;
      break;

    case 4:
      p1.compresion=p1.compresion-1;
      Serial.println(p1.compresion);
      // code to be executed if expression == constant2
      //select=0;
      break;
    default:
    
      break;
        // code to be executed if expression doesn't match any case
    }
    Serial.println("b3");
  

  }
  
  if (digitalRead(p1.Bot4) == HIGH){
    delay(10);

    if (selm == 1 ){
      p1.distmove();
    }

    if (selm == 2 ){
      p1.rotamove(m1);
    }
    
    if (selm == 3 ){
      p1.inclimove(m2);

    }

    if (selm == 4 ){
      p1.compremove(m3);

    }

    selm= selm-1;

    if(selm < 0){
      selm=4;
    }

    Serial.println("b4");
    Serial.println(selm);

  }    
 //return (0);
}

void setup(){
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Inicializado..");
  delay(1000);

  //motor 1 base

  m1.id =1;
  m1.gearR=6;
  m1.ppasos=15;
  m1.pdirec=2;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("motor 1 base");
  lcd.setCursor(0,1);
  lcd.print(m1.id);
  delay(1000);

  //motor 2 inclinacion

  m2.id =2;
  m2.gearR=4;
  m2.ppasos=4;
  m2.pdirec=16;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("motor 2 incl");
  lcd.setCursor(0,1);
  lcd.print(m2.id);
  delay(1000);


  //motor 3 tornillo

  m3.id =3;
  m3.gearR=360/1.25;
  m3.ppasos=17;
  m3.pdirec=5;

  m1.setup();
  m2.setup();
  m3.setup();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("motor 3 tor");
  lcd.setCursor(0,1);
  lcd.print(m3.id);
  delay(1000);


  //servo 
  servo1.attach(serv1);
  servo2.attach(serv2);
  servo3.attach(serv3);
  servo4.attach(serv4);

  //laser
  //pinMode(laser, OUTPUT);
  digitalWrite(laser,LOW);
  digitalWrite(laser,HIGH);

  //panel control
  p1.setup();

  Serial.begin(9600);


  // pinMode(emisor, INPUT_PULLDOWN);
  // pinMode(reseptor, INPUT_PULLDOWN);
  // pinMode(relaser, OUTPUT);

  // pinMode(secon_reci, INPUT_PULLDOWN);
  // pinMode(secon_laser, OUTPUT);


  // pinMode(LED_BUILTIN, OUTPUT);
  // pinMode(pasos, OUTPUT);
  // pinMode(direct, OUTPUT);

  // digitalWrite(emisor,HIGH);

  // attachInterrupt(digitalPinToInterrupt(emisor), startMeasuring, RISING);

  // attachInterrupt(digitalPinToInterrupt(reseptor), stopMeasuring , RISING);

  // attachInterrupt(digitalPinToInterrupt(secon_reci), changeSate , CHANGE);

  // attachInterrupt(digitalPinToInterrupt(p1.Bot1), readB1 , FALLING);
  // attachInterrupt(digitalPinToInterrupt(p1.Bot2), readB2 , FALLING);
  // attachInterrupt(digitalPinToInterrupt(p1.Bot3), readB3 , FALLING);
  // attachInterrupt(digitalPinToInterrupt(p1.Bot4), readB4 , FALLING);






  // digitalWrite(relaser, LOW);
  // digitalWrite(secon_laser, LOW);

  Serial.println("Start");
  //p1.returncom(m3,servo1);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ready ...");
}

void loop (){
 
  delay(125);
  
  updateLCD();
  read_bot();
  

  if (Serial.available() > 0) {
    receivedInt = Serial.parseInt(); // Read the integer
    //Serial.flush();
    Serial.print("You entered: ");
    Serial.println(receivedInt);
    //m1.move(receivedInt);
    //m2.move(receivedInt);
    //m3.move(receivedInt/10);
    // servo1.write(receivedInt);
    // delay(50);
    // servo3.write(receivedInt);
    // delay(50);
    // servo2.write(receivedInt);
    // delay(50);
    // servo4.write(receivedInt);
    //servo3.write(receivedInt);
    if(receivedInt==0){
      p1.hold();
    }
    if(receivedInt==1){
      p1.shoot();
    }
    }
   
}

