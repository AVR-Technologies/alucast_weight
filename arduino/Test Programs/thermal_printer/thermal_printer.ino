#include <SoftwareSerial.h>

SoftwareSerial mySerial(6, 7); // RX, TX

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  print();
}

void loop() { // run over and over
//  if (mySerial.available()) {
//    Serial.write(mySerial.read());
//  }
//  if (Serial.available()) {
//    mySerial.write(Serial.read());
//  }
}
void print(){
  char buff[5];
  dtostrf(4.85, 4, 2, buff);
  mySerial.println();
  mySerial.println();
  mySerial.printf(F("ALUCAST MACHINES  %.2d\n"),   8);
  mySerial.printf(F("DENSITY ANALYZER  %.4s\n"),   "hehe");
  mySerial.println();
  mySerial.printf(F("OPERATOR NAME     %0.10s\n"), "admin12345");
  mySerial.printf(F("HEAT NO           %.2d\n"),   2);
  mySerial.println();
  mySerial.printf(F("DATE              %0.8s\n"),  "26/07/21");
  mySerial.printf(F("TIME              %0.8s\n"),  "12:59:59");
  mySerial.println();
  mySerial.printf(F("DENSITY           (g/cc)\n")  );
  mySerial.printf(F("VACUUM            %0.5s\n"),  buff);
  mySerial.printf(F("AIR               %0.5s\n"),  buff);
  mySerial.println();
  mySerial.printf(F("DENSITY INDEX     %0.5s\n"),  buff);
  mySerial.println();
  mySerial.println();
  mySerial.println();
}
