#include <EEPROM.h>
const int locations[10] = {0, 100, 200, 300, 400, 500, 600, 700, 800, 900};
String data[10]={"","","","","","","","","",""};

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
    for(int j = 0; j < length; j++){
      EEPROM.update(locations[i]+1+j, data[i][j]);
      Serial.print(data[i][j]);
    }
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
//void addStudent(Student &student){
//  students[0] = students[1];
//  students[1] = students[2];
//  students[2] = students[3];
//  students[3] = students[4];
//  students[4] = student;
//  EEPROM.put(start_address, students);
//}
