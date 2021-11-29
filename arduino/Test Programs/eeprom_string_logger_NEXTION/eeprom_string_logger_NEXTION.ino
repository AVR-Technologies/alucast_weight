#include <EEPROM.h>
#include <SoftwareSerial.h>
#define endBytes  "\xFF\xFF\xFF"

float Dvac,Dair,Di;
const int locations[10] = {0, 100, 200, 300, 400, 500, 600, 700, 800, 900};
String data[10]={"","","","","","","","","",""};
SoftwareSerial nex(12, A2);
String newData;
void setup() {
  Serial.begin(115200);
  Serial.println("booting");
//  saveToEeprom();
  readFromEeprom();
  for(auto &d: data) Serial.println(d);
}

void loop() {
  if(Serial.available()){
    newData = Serial.readString();
    addData();
  }
}
void addData(){
  for(int i = 0; i < 9; i++) data[i] = data[i+1];
  data[9] = newData;
//  for(auto &d: data) Serial.println(d);
  saveToEeprom();
}
void saveToEeprom(){
  for(int i = 0; i < 10; i++){
    int length = data[i].length();
    EEPROM.update(locations[i], length);
    Serial.print("=");
    for(int j = 0; j < length-1; j++){
      EEPROM.update(locations[i]+1+j, data[i][j]);
      Serial.print(data[i][j]);
    }
    Serial.println();
  }
}
void readFromEeprom(){
  for(int i = 0; i < 10; i++){
    int length = EEPROM.read(locations[i]);
    data[i] = "";
    for(int j = 0; j < length; j++){
      data[i] += (char)EEPROM.read(locations[i]+1+j);
    }
  }
}
