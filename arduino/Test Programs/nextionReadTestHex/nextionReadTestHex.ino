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

#define endBytes  "\xFF\xFF\xFF"

SoftwareSerial nex(12, A2);
String Data = "";
#define nexSerial Serial
#define logSerial nex
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  logSerial.begin(9600);
  nexSerial.begin(115200);
//  nex.print("get p0t0.txt\xFF\xFF\xFF");
logSerial.println("Start");
}

// the loop routine runs over and over again forever:
void loop() {
 if (nexSerial.available()) {
    /***Read Input Data from Display****/
    String Data = nexSerial.readString();
    byte pageId = (byte)Data[1];
    byte buttonId = (byte)Data[2];
    logSerial.print("Input = ");
    logSerial.println(Data);

//    /***********Page 0****************/
    if ((byte)0 == pageId ) {
      if ((byte)2 == buttonId) {
        logSerial.println("page 0 start");
        int first = 0;
        Data  = readText("p0t0.txt");Data +=":";
        Data += readText("p0t1.txt");Data +=":";
        Data += readText("p0t2.txt");Data +=":";
        Data += readText("p0t3.txt");Data +=":";
        Data += readText("p0t4.txt");Data +=":";

        logSerial.println(Data);
        nexSerial.flush();
//        for (int i = 0; i < 6; i++) {
//          int last = Data.indexOf(':');
//          config[i] = Data.substring(0, last);
//          Data = Data.substring(last + 1);
//          config_len[i] = config[i].length();
//
//        }
//        logSerial.print("Data = ");
//        for (auto val : config) logSerial.print(val), logSerial.print('\t');
//
//        sendCommand("page 1");
//        pageId = (byte)1;
//        Reading = true;
//        blink = 1;
//        on = 0;
//
//      }
//      else if ((byte)6 == buttonId) {
//        logSerial.println("page " + String(pageId) + " Reports");
//        sendCommand("page 7");
//        nexSerial.flush();
//        delay(500);
//        pageNo = readText("t0.txt");
//        Reports_page = pageNo.toInt();
//        logSerial.print("Reports String= ");
//        logSerial.println(pageNo);
//
//        logSerial.print("Reports ID= ");
//        logSerial.println(Reports_page);
//        readFromEeprom(Reports_page);
      }
    }

  }
}

String readText(String objname) {
  nexSerial.print("get " + objname + endBytes);
  String input = nexSerial.readString();
  input = input.substring(1, input.length() - 3 );
  return input;
}
