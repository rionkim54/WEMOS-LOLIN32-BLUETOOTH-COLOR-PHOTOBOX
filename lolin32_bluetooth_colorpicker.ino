//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include "BluetoothSerial.h"

#include "esp32_digital_led_lib.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

strand_t STRANDS[] = { // Avoid using any of the strapping pins on the ESP32
  {.rmtChannel = 1, .gpioNum = 16, .ledType = LED_WS2812B_V3, .brightLimit = 255, .numPixels =  74,
   .pixels = nullptr, ._stateVars = nullptr},
//  {.rmtChannel = 2, .gpioNum = 18, .ledType = LED_WS2812B_V3, .brightLimit = 32, .numPixels =  93,
//   .pixels = nullptr, ._stateVars = nullptr},
//  {.rmtChannel = 3, .gpioNum = 19, .ledType = LED_WS2812B_V3, .brightLimit = 32, .numPixels =  64,
//   .pixels = nullptr, ._stateVars = nullptr},
//{.rmtChannel = 0, .gpioNum = 16, .ledType = LED_WS2812B_V3, .brightLimit = 32, .numPixels = 256,
// .pixels = nullptr, ._stateVars = nullptr},
//  {.rmtChannel = 0, .gpioNum = 16, .ledType = LED_SK6812W_V1, .brightLimit = 32, .numPixels = 300,
//   .pixels = nullptr, ._stateVars = nullptr},
//  {.rmtChannel = 0, .gpioNum = 16, .ledType = LED_WS2813_V2, .brightLimit = 32, .numPixels = 300,
//   .pixels = nullptr, ._stateVars = nullptr},
};
int STRANDCNT = sizeof(STRANDS)/sizeof(STRANDS[0]);

BluetoothSerial SerialBT;

void setup() {
  
  // pinMode(16, OUTPUT);
    
  Serial.begin(9600);
  SerialBT.begin("ESP32 ZEROWIN"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

 if (digitalLeds_initStrands(STRANDS, STRANDCNT)) {
      Serial.println("Init FAILURE: halting");
      while (true) {};
    }
    for (int i = 0; i < STRANDCNT; i++) {
      strand_t * pStrand = &STRANDS[i];
      Serial.print("Strand ");
      Serial.print(i);
      Serial.print(" = ");
      Serial.print((uint32_t)(pStrand->pixels), HEX);
      Serial.println();
      digitalLeds_resetPixels(pStrand);
    }   
}

String readString;

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
boolean bToogle = 0;

void setDraw(int r, int g, int b)
{
strand_t * pStrand = &STRANDS[0];
      for (uint16_t i = 0; i < pStrand->numPixels; i++) {
        pStrand->pixels[i].r = r;
        pStrand->pixels[i].g = g;
        pStrand->pixels[i].b = b;
      }    
      digitalLeds_updatePixels(pStrand);   
}

void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  /*
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }
  */

  //expect a string like wer,qwe rty,123 456,hyre kjhg,
  //or like hello world,who are you?,bye!,
  while (SerialBT.available()) {
    
    delay(10);  //small delay to allow input buffer to fill
    char c = SerialBT.read();  //gets one byte from serial buffer
    if (c == '\n') {
      break;
    }  //breaks out of capture loop to print readstring
    readString += c; 
  } //makes the string readString  

  if (readString.length() > 0) {
    
    Serial.println("" + readString + " " + readString.length()); //prints string to serial port out

    String sRed = getValue(readString, ',', 0);
    String sGreen = getValue(readString, ',', 1);
    String sBlue = getValue(readString, ',', 2);
    Serial.println(sRed);
    Serial.println(sGreen);
    Serial.println(sBlue);  

    int r = sRed.toInt();
    int g = sGreen.toInt();
    int b = sBlue.toInt();    
    
    setDraw(r, g, b);
 
    readString=""; //clears variable for new input  
  }

//  bToogle = !bToogle;
  
//  if(bToogle) digitalWrite(16, 1);
//  else digitalWrite(16, 0);
  
  delay(20);
}
