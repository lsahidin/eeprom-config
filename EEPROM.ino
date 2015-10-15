// Copyright (c) 11-10-2015
// EEPROM save configuration
// Luqman Sahidin - Release for public domain
// Enjoy !
// lsahidin@yahoo.com

#include <EEPROM.h>

struct dataObject{
  int saklar;
  char nomer[15];
};

int eeAddress;   // Memory location we want the data to be put.

int readline(int readch, char *buffer, int len)
{
  static int pos = 0;
  int rpos;

  if (readch > 0) {
    switch (readch) {
      case '\n': // Ignore new-lines
        break;
      case '\r': // Return on CR
        rpos = pos;
        pos = 0;  // Reset position index ready for next time
        return rpos;
      default:
        if (pos < len-1) {
          buffer[pos++] = readch;
          buffer[pos] = 0;
        }
    }
  }
  // No end of line has been found, so return -1.
  return -1;
}

void softReset(){
  Serial.print("Rebooting ");
  for (int i = 0; i <= 5; i++){
    Serial.print(". ");
    delay(500);
  }
  Serial.write(27); // ESC 
  Serial.print("[2J"); // clear screen
  Serial.write(27); // ESC 
  Serial.print("[H"); // cursor to home
  delay(10);
  asm volatile ("  jmp 0");
}

void setup() {
  Serial.begin(9600);
  Serial.print("0");
  Serial.println(F("  second(s) configuration to exit"));
  Serial.println("a -command 1\r\nb -command 2\r\nc -command 3\r\n");
  for (int i = 0; i <= 3; i++){
    Serial.write(27); // ESC 
    Serial.print("[H"); // cursor to home 
    //Serial.print("[2J"); // clear screen
    Serial.print(i);
    Serial.write(27); // ESC
    Serial.print("[H"); // cursor to home 
    delay(1000);
  }
  Serial.write(27); // ESC 
  Serial.print("[2J"); // clear screen
  Serial.println(F("Command:"));
  Serial.println(F(" c: Clear memory"));
  Serial.println(F(" r: Read memory (block)"));
  Serial.println(F(" p: Put data"));
  Serial.println(F(" g: Get data"));
  Serial.println(F(" s: Reboot\r\n"));
  Serial.print(F("~> "));
}

void loop() {
  // put your main code here, to run repeatedly:
  static char buffer[80];
  while (Serial.available() > 0) {
    char key = Serial.read();
    Serial.print(key);

      int numchar = readline(key, buffer, 80);
      if (numchar > 0) {
        Serial.print(F("\r\n"));
    
        if (buffer[numchar-1] == 'c'){
          Serial.println("Cleaning memory..");
          digitalWrite(13, HIGH);
          for ( int i = 0 ; i < EEPROM.length() ; i++ )
            EEPROM.write(i, 0);
            
          // turn the LED on when we're done
          digitalWrite(13, LOW);
          Serial.println("Memory is clean");
          Serial.println();
        }
    
        else if (buffer[numchar-1] == 'r'){
          byte value;
          for (eeAddress = 0; eeAddress <= 25; eeAddress++){
            value = EEPROM.read(eeAddress);
          
            Serial.print(eeAddress);
            Serial.print("\t");
            Serial.print(value, DEC);
            Serial.println();
            delay(10);
          }
          Serial.println();
        }

        // Data to store.
        else if (buffer[numchar-1] == 'p'){
          eeAddress = 0; //go to zero
          float mem_id = 'yuganstudio';  // Variable to store in EEPROM.
          // One simple call, with the address first and the object second.
          EEPROM.put(eeAddress, mem_id);
          Serial.println(F("Create memory address!"));
          dataObject info = {
            1,
            "08111111"
          };
          eeAddress += sizeof(float); // Move address to the next byte
          EEPROM.put(eeAddress, info);
          Serial.println("Write data info done.");
          Serial.println();
        }
    
        else if (buffer[numchar-1] == 'g'){
          eeAddress = 0; //go to zero
          eeAddress = sizeof(float); //Move address to the next byte after float.
        
          dataObject info; //Variable to store custom object read from EEPROM.
          EEPROM.get(eeAddress, info);
          
          Serial.println(F("Read custom object from EEPROM: "));
          Serial.print(F("Status saklar: "));
          Serial.println(info.saklar);
          Serial.print(F("Nomer Owner: "));
          Serial.println(info.nomer);
          if (String(info.nomer) == "08111111") Serial.println(F("Number Macthed"));
          Serial.print(F("\r\n"));
        }

        else if (buffer[numchar-1] == 's'){
          softReset();
        }
  
        Serial.print(F("~> "));
      }
  } // end while
  delay(10);
}
