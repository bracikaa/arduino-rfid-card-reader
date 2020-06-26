#include <hd44780.h>
#include <SPI.h> 
#include <MFRC522.h>
#include <Wire.h>                      // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define NOTE_SUSTAIN 100

#define SS_PIN 10 
#define RST_PIN 9 

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class
const int LCD_COLS = 16;
const int LCD_ROWS = 2;
const int sAudioPin = 7;
boolean stringComplete = false;  // whether the string is complete

String content = "";
String commandString = "";
char character;

hd44780_I2Cexp lcd;

void setup() {
  int status;
  Serial.begin(9600); 
  SPI.begin();       // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 
  Serial.println("hello");
  pinMode(7, OUTPUT);
  status = lcd.begin(LCD_COLS, LCD_ROWS);
  Serial.println(status);
  if(status) // non zero status means it was unsuccesful
  {
    hd44780::fatalError(status); // does not return
  }
  displayIntroMessage();
}

void loop() { 
  if(stringComplete)
  {
    stringComplete = false;
    getCommand();
    String text = getTextToPrint();
     if(commandString.equals("ERROR"))
    {
      errorMusic();

    } else if(commandString.equals("SUCCS")) {
      successMusic();
    }
    printText(text);
  }
  
  if ( mfrc522.PICC_IsNewCardPresent())
    {
        if ( mfrc522.PICC_ReadCardSerial())
        {
           lcd.clear();
           lcd.setCursor(0,0);
           lcd.print("Tag UID:");
           lcd.setCursor(0,1);
           char str[32] = "";
           array_to_string(mfrc522.uid.uidByte, 4, str); //Insert (byte array, length, char array for output)
           Serial.println(str); //Print the output uid string
           lcd.print(str);                 
           mfrc522.PICC_HaltA();
           delay(2000);
           reset_state();
        }
    }
}

void reset_state()
{
    lcd.clear();
    lcd.setCursor(0,0);
    content = "";
}

void displayIntroMessage(void) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Welcome to: ");
    lcd.setCursor(0, 1);
    lcd.print("Arnold's Gym!");
    delay(1200);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set your card");
    lcd.setCursor(0, 1);
    lcd.print("On The Reader!");
    delay(1200);
    reset_state();
}

  void serialEvent() {
    while (Serial.available()){
       char inchar = (char)Serial.read();
       content += inchar;
  
       if(inchar == '\n') {
        stringComplete = true;
       }
    }
  }

    
  void getCommand()
  {
    if(content.length()>0)
    {
       commandString = content.substring(1,6);
    }
  }


  String getTextToPrint()
  {
    String value = content.substring(1,content.length()-2);
    return value;
  }

  
void printText(String text)
{
  lcd.clear();
  lcd.setCursor(0,0);
    if(text.length()<16)
    {
      lcd.print(text);
    }else
    {
      lcd.print(text.substring(0,16));
      lcd.setCursor(0,1);
      lcd.print(text.substring(16,32));
    }
    
    delay(2000);
    reset_state();
}

void errorMusic() {
  tone(sAudioPin,NOTE_G4);
  delay(250);
  tone(sAudioPin,NOTE_C4);
  delay(500);
  noTone(sAudioPin);
}

void successMusic() {
   tone(sAudioPin,NOTE_A5);
    delay(NOTE_SUSTAIN);
    tone(sAudioPin,NOTE_B5);
    delay(NOTE_SUSTAIN);
    tone(sAudioPin,NOTE_C5);
    delay(NOTE_SUSTAIN);
    tone(sAudioPin,NOTE_B5);
    delay(NOTE_SUSTAIN);
    tone(sAudioPin,NOTE_C5);
    delay(NOTE_SUSTAIN);
    tone(sAudioPin,NOTE_D5);
    delay(NOTE_SUSTAIN);
    tone(sAudioPin,NOTE_C5);
    delay(NOTE_SUSTAIN);
    tone(sAudioPin,NOTE_D5);
    delay(NOTE_SUSTAIN);
    tone(sAudioPin,NOTE_E5);
    delay(NOTE_SUSTAIN);
    tone(sAudioPin,NOTE_D5);
    delay(NOTE_SUSTAIN);
    tone(sAudioPin,NOTE_E5);
    delay(NOTE_SUSTAIN);
    tone(sAudioPin,NOTE_E5);
    delay(NOTE_SUSTAIN);
    noTone(sAudioPin);
}

void array_to_string(byte array[], unsigned int len, char buffer[])
{
   for (unsigned int i = 0; i < len; i++)
   {
      byte nib1 = (array[i] >> 4) & 0x0F;
      byte nib2 = (array[i] >> 0) & 0x0F;
      buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
      buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
   }
   buffer[len*2] = '\0';
}
