int sensor1 = 13, sensor2 = 12, sensor3 = 11;
bool triggerSensor1 = false, triggerSensor2 = false, triggerSensor3 = false;
bool up = false, low = false, ready_ = true;
int angle = 90;

#include <Servo.h>
Servo myservo;

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <dht11.h>
 
dht11 DHT11;
 
#define DHT11PIN 2

unsigned long currentMillis, previousMillis = 0;

void setup() {
  myservo.attach(9);
  
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();

  lcd.print("hello world!");
  
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  Serial.begin(9600);
}

void loop() {
  int chk = DHT11.read(DHT11PIN);

  myservo.write(angle);
  
  // put your main code here, to run repeatedly:
  int x,y,z;

  if(ready_ == false) currentMillis = millis();

  if (previousMillis - currentMillis < -2000 && ready_ == false)
  {
    triggerSensor1 = false;
    triggerSensor2 = false;
    triggerSensor3 = false;
    Serial.println("重置，进入待机状态");
    lcd.clear();
    lcd.print("Temp: ");
    lcd.print((float)DHT11.temperature);
    lcd.setCursor(0, 1);
    lcd.print("ready!");
    
    ready_ = true;
  }

  if (ready_ == true) 
  {
    lcd.clear();
    lcd.print("Temp: ");
    lcd.print((float)DHT11.temperature);
    lcd.setCursor(0, 1);
    lcd.print("ready!");
    delay(10);
  }
  
  if (triggerSensor1 == false && triggerSensor2 == false && triggerSensor3 == false)
  {
    x = digitalRead(sensor1);
    y = digitalRead(sensor2);
    z = digitalRead(sensor3);
    if(x == 0)
    {
      triggerSensor1 = true;
      previousMillis = currentMillis;
      ready_ = false;
    }
    if(y == 0)
    {
      triggerSensor2 = true;
      previousMillis = currentMillis;
      ready_ = false;
    }
    if(z == 0) 
    {
      triggerSensor3 = true;
      previousMillis = currentMillis;
      ready_ = false;
    }
  }

  if (triggerSensor1 == true) {
    y = digitalRead(12);
    if (y == 0)
    {
      triggerSensor1 = false;
      triggerSensor2 = true;
      up = true;
      previousMillis = currentMillis;
      ready_ = false;
    }
  }
  if (triggerSensor3 == true) {
    y = digitalRead(12);
    if (y == 0)
    {
      triggerSensor3 = false;
      triggerSensor2 = true;
      low = true;
      previousMillis = currentMillis;
      ready_ = false;
    }
  }
  if (triggerSensor2 == true) {
    x = digitalRead(11);
    z = digitalRead(13);
    if (z == 0)
    {
      triggerSensor2 = false;
      low = true;
      previousMillis = currentMillis;
      ready_ = false;
    }
    if (x == 0)
    {
      triggerSensor2 = false;
      up = true;
      previousMillis = currentMillis;
      ready_ = false;
    }
  }
  
  if(up == true)
  {
    lcd.clear();
    lcd.print("temp up");
    up = false;
    if(angle > 40) angle -= 10;
    Serial.println("up = false");
  }

  if(low == true)
  {
    lcd.clear();
    lcd.print("temp low");
    low = false;
    if(angle < 140) angle += 10;
    Serial.println("low = false");
  }
  
}
