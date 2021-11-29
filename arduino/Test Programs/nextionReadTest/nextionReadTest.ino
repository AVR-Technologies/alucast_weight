///*
//  AnalogReadSerial
//
//  Reads an analog input on pin 0, prints the result to the Serial Monitor.
//  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
//  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.
//
//  This example code is in the public domain.
//
//  http://www.arduino.cc/en/Tutorial/AnalogReadSerial
//*/
//#include <SoftwareSerial.h>
//
//SoftwareSerial nex(3,2);
//String Data="";
//// the setup routine runs once when you press reset:
//void setup() {
//  // initialize serial communication at 9600 bits per second:
//  Serial.begin(9600);
//  nex.begin(9600);
//  nex.print("get p0t3.txt");
//  Serial.print(nex.read());
//}
//
//// the loop routine runs over and over again forever:
//void loop() {
//
//  nex.print("get p0t3.txt");
//  Serial.print(nex.read());
//
//// if(Serial.available())
//// {
////    Data = "p6t1.txt=\""+Serial.readString()+"\"";
////    nex.print(Data);
////    nex.write(0xff);
////    nex.write(0xff);
////    nex.write(0xff);
////
//// }
//
//}






#include <SoftwareSerial.h>

SoftwareSerial nex(3, 2);
String Data = "";
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  nex.begin(9600);
  nex.print("get p0t0.txt\xFF\xFF\xFF");
}

// the loop routine runs over and over again forever:
void loop() {
  if (int length = nex.available())
  {
    //  while(int length= nex.available())
    //  {
    char buffer [length];
    char buffer2  [length - 4];
    Serial.println(length);
    nex.readBytes(buffer, length);
    memcpy(&buffer2, &buffer + 1, length - 4);

    for (auto c : buffer) Serial.print(c);
    Serial.print("\nbuff2: ");
    for (auto c : buffer2) Serial.print(c);
//    {
//      Data += c;
//    }
  }
//  Serial.print(Data);
  // }



}
