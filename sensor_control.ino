int sensor1 = 13, sensor2 = 12, sensor3 = 11;
bool triggerSensor1 = false, triggerSensor2 = false, triggerSensor3 = false;
bool up = false, low = false, ready_ = true;

unsigned long currentMillis, previousMillis = 0;

void setup() {
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int x,y,z;

  if(ready_ == false) currentMillis = millis();

  if (previousMillis - currentMillis < -2000 && ready_ == false)
  {
    triggerSensor1 = false;
    triggerSensor2 = false;
    triggerSensor3 = false;
    Serial.println("重置，进入待机状态");
    ready_ = true;
  }
  
  if (triggerSensor1 == false && triggerSensor2 == false && triggerSensor3 == false)
  {
    x = digitalRead(sensor1);
    y = digitalRead(sensor2);
    z = digitalRead(sensor3);
    if(x == 0)
    {
      triggerSensor1 = true;
      Serial.println("triggerSensor1 = true;");
      previousMillis = currentMillis;
      ready_ = false;
    }
    if(y == 0)
    {
      triggerSensor2 = true;
      Serial.println("triggerSensor2 = true;");
      previousMillis = currentMillis;
      ready_ = false;
    }
    if(z == 0) 
    {
      triggerSensor3 = true;
      Serial.println("triggerSensor3 = true;");
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
      Serial.println("up = true;");
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
      Serial.println("low = true");
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
      Serial.println("low = true");
      previousMillis = currentMillis;
      ready_ = false;
    }
    if (x == 0)
    {
      triggerSensor2 = false;
      up = true;
      Serial.println("up = true");
      previousMillis = currentMillis;
      ready_ = false;
    }
  }
  
  if(up == true)
  {
    Serial.println("temp up");
    up = false;
    Serial.println("up = false");
  }

  if(low == true)
  {
    Serial.println("temp low");
    low = false;
    Serial.println("low = false");
  }
  
}
