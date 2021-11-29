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

boolean accept = false;
int blink=0;
long int tim=0;
SoftwareSerial nex(3, 2);
String config [6];
struct Button {
  byte page;
  byte id;
  Button (byte _page, byte _id) : page(_page), id(_id) {}
};
Button startTest1(0, 2);
Button reports0(0, 6);
Button accept1(1, 4);
Button abort1(1, 3);
Button continue1(1, 2);

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  nex.begin(9600);
  //  Serial.println(readText("p0t3.txt"));
  //  delay(100);
  sendCommand("page 0");
}
void loop() {
  readNex();

  if(blink)
  {
    WriteData("Please press accept button to accept the weight");
    sendCommand("vis t1,0");
    Delay(100);
    sendCommand("vis t1,1");
    Delay(100);
  }
}
String readText(String objname) {
  nex.print("get " + objname + endBytes);
  String input = nex.readString();
  input = input.substring(1, input.length() - 3 );
  return input;
}

void sendCommand(String Command)
{
  nex.print(Command + endBytes);
}

void readNex()
{
  if (nex.available())
  {
    Serial.println("Data received from Display");
    String Data = nex.readString();
    byte pageId = (byte)Data[1], buttonId = (byte)Data[2];

    
    if (startTest1.page == pageId )
    {
      if (startTest1.id == buttonId)
      {
        Serial.println("page 0 start");
        int first = 0;
        Data = Data.substring(8);
        Serial.println(Data);
        for (int i = 0; i < 6; i++)
        {
          int last = Data.indexOf(':');
          config[i] = Data.substring(0, last);
          Data = Data.substring(last + 1);
        }
        for (auto val : config) Serial.print(val), Serial.print('\t');
        sendCommand("page 1");
         blink=1;
      }
      else if (reports0.id  == buttonId)
      {
        Serial.println("page 0 reports");

      }
    }
    else if(pageId ==  (byte)1 || pageId ==  (byte)2 || pageId ==  (byte)3 || pageId ==  (byte)4)
    {
      
     
      if (accept1.id   == buttonId)
      {
        Serial.println("page "+String(pageId)+" accept");
        WriteData("Press Continue to accept next weight or Press abort to\r\nre-enter the weight");
        blink=0;
      }
      else if (abort1.id    == buttonId)
      {
        Serial.println("page "+String(pageId)+" abort");
        sendCommand("page 0");
        blink=0;
      }
      else if (continue1.id == buttonId)
      {
        accept = true;
        Serial.println("page "+String(pageId)+" continue");
        sendCommand("page "+String(pageId+(byte)1));
        delay(200);
        if(pageId!=(byte)4)
         blink=1;
      }
      Serial.println(blink);
    }
    
    else if(pageId ==(byte)5)
    {
      if (continue1.id   == buttonId)
      {
        Serial.println("page "+String(pageId)+" Display");
        sendCommand("page 6");
        delay(500);
        DisplayResults();
      }
      else if (abort1.id  == buttonId && accept==true)
      {
        accept = false;
        Serial.println("page "+String(pageId)+" abort");
        sendCommand("page 0");
//        blink=1;
      }
    }

    else if(pageId ==  (byte)6)
    {
      if ((byte)5 == buttonId)
      {
        Serial.println("page "+String(pageId)+" Print");
//        sendCommand("page 6");
        delay(200);
      }
      else if ((byte)2 == buttonId)
      {
        accept = false;
        Serial.println("page "+String(pageId)+" NewTest");
        sendCommand("page 0");
//        blink=1;
      }
    }
  }
}


void Delay(int d)
{
  long milis = millis();
  milis = milis + d;
  while(millis()<milis)
  {
    readNex();
  }
  return;
}

void WriteData(String Write)
{
    String Command = "t0.txt=\""+Write+"\""+endBytes;
    nex.print(Command);
}

void DisplayResults()
{
    nex.print("p6t5.txt=\""+config[0]+"\""+endBytes);  
    nex.print("p6t1.txt=\""+config[1]+"\""+endBytes);
    nex.print("p6t2.txt=\""+config[2]+"\""+endBytes);
    nex.print("p6t6.txt=\""+config[3]+"\""+endBytes);
    nex.print("p6t4.txt=\""+config[4]+"\""+endBytes);
}
