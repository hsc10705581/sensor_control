/*
  //2018年6月7日晚，在getFingerprintEnroll()显示stored的时候加了return p 虽然不知道return有什么用，但似乎可以退出死循环
*/
//sensors
int sensor1 = 13, sensor2 = 12, sensor3 = 11;
bool triggerSensor1 = false, triggerSensor2 = false, triggerSensor3 = false;
bool up = false, low = false, ready_ = true;
int angle = 90;

//servo
int myServoPin = 1;
#include <Servo.h>
Servo myservo;

//lcd1602
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
  // to record which page you are at now
int page = 0;
bool enroll = false, IDez = false;
int enrollPage = 0, IDezPage = 0;

//dht11
#include <dht11.h>
dht11 DHT11;
#define DHT11PIN 0

//time
unsigned long currentMillis, previousMillis = 0;

//keypad
#include <Keypad.h>
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
  //define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {10, 9, 8, 7}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 5, 4, 15}; //connect to the column pinouts of the keypad
  //initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

//fingerPrint
#include <DYE_Fingerprint.h>
// pin #0 is IN from sensor (GREEN wire)
// pin #1 is OUT from arduino  (WHITE wire)
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3);

int storeType[10] = {0};
int address;

DYE_Fingerprint finger = DYE_Fingerprint(&mySerial);
uint8_t id;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  delay(100);
  Serial.println("\n\nfinger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  Serial.println("Waiting for valid finger...");
  
  myservo.attach(myServoPin);
  
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  lcd.print("hello world!");
  
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
}


uint8_t readnumber(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}


void loop() {
  // to get the temp (need to repair)
  //int chk = DHT11.read(DHT11PIN);
  // to control servo
  myservo.write(angle);
  // to record sensor reaction
  int x,y,z;
  // record current time
  if(ready_ == false) currentMillis = millis();
  // 检验是否2秒未操作，若是，则进入待机状态。
  if (previousMillis - currentMillis < -2000 && ready_ == false)
  {
    triggerSensor1 = false;
    triggerSensor2 = false;
    triggerSensor3 = false;
    //Serial.println("重置，进入待机状态");
    ready_ = true;
  }
  //如果进入待机状态，则显示。
  if (ready_ == true) 
  {
    // to get what user inputs
    char customKey = customKeypad.getKey();
    switch(page)
    {
      case 0:
        lcd.clear();
        lcd.print("Welcome!");
        lcd.setCursor(0, 1);
        lcd.print("*)no more #)next");
        break;
      case 1:
        lcd.clear();
        lcd.print("Temp: ");
        lcd.print((float)DHT11.temperature);
        lcd.setCursor(0, 1);
        lcd.print("*)back #)next");
        break;
      case 2:
        if (customKey == '1') 
        {
          enroll = true;
          customKey = NO_KEY; // without this sentence you will skip enrollPage0
          delay(100);
        }
        if (enroll == true) 
        {
          switch(enrollPage)
          {
            case 0:
              lcd.clear();
              lcd.print("type 1 to 9");
              lcd.setCursor(0, 1);
              lcd.print("to save finger");
              if (customKey != NO_KEY){
                id = customKey - 48;
                address = customKey - 48;
                enrollPage++;
                customKey = NO_KEY;
                while (!  getFingerprintEnroll() );
              }
              break;
            case 1:
              lcd.clear();
              lcd.print("choose type");
              lcd.setCursor(0, 1);
              lcd.print("1)H 2)C 3)W");
              if (customKey > 48 && customKey < 52)
              {
                storeType[address] = customKey - 48;
                enrollPage++;
              }
              break;
            case 2:
              lcd.clear();
              lcd.print("success!");
              lcd.setCursor(0, 1);
              lcd.print("type(1) to back");
              if ( customKey == 49)
              {
                page = 0;
              }
              break;
          }
        }
        else
        {
          lcd.clear();
          lcd.print("press(1) enroll");
          lcd.setCursor(0, 1);
          lcd.print("*)back #)next");
        }
        break;
      case 3:
        if (customKey == '1') 
        {
          IDez = true;
          customKey = NO_KEY; // without this sentence you will skip enrollPage0
          delay(100);
        }
        if (IDez == true)
        {
          switch(IDezPage)
          {
            case 0:
              lcd.clear();
              lcd.print("put your finger");
              address = getFingerprintIDez();
              delay(50);
              if (address != -1)
              {
                IDezPage++;
              }
              break;
            case 1:
              lcd.clear();
              lcd.print("hello user");lcd.print("address");
              lcd.setCursor(0,1);
              if (storeType[address] == 1) lcd.print("Hot");
              if (storeType[address] == 2) lcd.print("Cold");
              if (storeType[address] == 3) lcd.print("Warm");
              lcd.print(" Water");
          }
        }
        else
        {
          lcd.clear();
          lcd.print("press(1) check");
          lcd.setCursor(0, 1);
          lcd.print("*)back #)no more");
        }
        break;
    }
    if (customKey == '*' && page > 0) 
    {
      page--;
      delay(90); // must?
    }
    else if (customKey == '#' && page < 3) 
    {
      page++;
      delay(90); // must? 会不会只是按一下才有一个信号呢？
    }
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
    y = digitalRead(sensor2);
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
    y = digitalRead(sensor2);
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
    x = digitalRead(sensor3);
    z = digitalRead(sensor1);
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
    if(angle > 45) angle -= 15;
    Serial.print(angle);
    myservo.write(angle);
    //Serial.println("up = false");
    //Serial.println(angle);
    delay(2000);
    
    up = false;
  }
  if(low == true)
  {
    lcd.clear();
    lcd.print("temp low");
    if(angle < 135) angle += 15;
    Serial.print(angle);
    myservo.write(180);
    //Serial.println("low = false");
    //Serial.println(angle);
    delay(2000);
    
    low = false;
  }
}

















uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  lcd.setCursor(0,1);
  //Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  lcd.print("Found ID"); lcd.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  lcd.clear();
  lcd.print("wait for finger");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  lcd.clear();
  lcd.print("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  lcd.setCursor(0, 1);
  lcd.print("place again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    lcd.clear();
    lcd.print("Stored");
    return p;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}
