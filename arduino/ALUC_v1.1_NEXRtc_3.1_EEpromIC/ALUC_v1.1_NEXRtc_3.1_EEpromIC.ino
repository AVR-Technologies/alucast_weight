/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogReadSerial
*/
#include <SoftwareSerial.h>
#include <Wire.h>
const byte i2cAddress     = 0x50;                                                               // i2c
const byte records_count  = 40;                                                                 // total records saved

String _data;

int eAddress(int index) {                                                                       // eeprom data address
  return  100 * index;                                                                           // data is stored in slots of 100 bytes
}
SoftwareSerial nex(7, 6);
SoftwareSerial Printer(9, 10);
SoftwareSerial Wt_scale(11, 12);

#define endBytes  "\xFF\xFF\xFF"
//#define //$logSerial nex
//#define Serial Serial
boolean  accept = false, Reading = false;
byte pageId ;
const int locations[10] = {0, 100, 200, 300, 400, 500, 600, 700, 800, 900};
int Reports_page = 0;
int blink = 0;

String data = "";
String Weight[4] = {"0.00", "0.00", "0.00", "0.00"};
String config [10];

void(* resetFunc) (void) = 0;

void setup() {
  Serial.begin(115200);
  nex.begin(115200);
  //$logSerial.println("Start Program");
  //  readDatbase();
  //  //  for(auto &d: data) ////$logSerial.println(d);
  //  for (int i = 0; i < 10; i++)
  //  {
  //    ////$logSerial.print(F("mem = "));
  //    ////$logSerial.println(data[i]);
  //  }
  sendCommand("page 0");
}
void loop() {
  if (int length = Serial.available()) {
    String Data = Serial.readString();
    nex.println(Data);
    pageId = (byte)Data[1];
    byte buttonId = (byte)Data[2];
    //    //$logSerial.print(F("Input = "));
    //    //$logSerial.println(Data);
    /***********Page 0****************/
    if ((byte)0 == pageId ) {
      if ((byte)2 == buttonId) {
        ////$logSerial.println("page 0 start");
        int first = 0;
        Data = Data.substring(8);

        //$logSerial.println(Data);
        for (int i = 0; i < 6; i++) {
          int last = Data.indexOf(':');
          config[i] = Data.substring(0, last);
          Data = Data.substring(last + 1);
          //          config_len[i] = config[i].length();
        }
        //        ////$logSerial.print("Data = ");
        //        for (auto val : config) ////$logSerial.print(val), ////$logSerial.print('\t');

        sendCommand("page 1");
        pageId = (byte)1;
        Reading = true;
        blink = 1;
      }
      else if ((byte)6 == buttonId) {
        //$logSerial.println("page " + String(pageId) + " Reports");
        sendCommand("page 7");
        delay(500);
        String pageNo = readText("t0.txt");
        Reports_page = pageNo.toInt();
        ////$logSerial.print(F("Reports String= "));
        ////$logSerial.println(pageNo);

        //$logSerial.print(F("Reports ID= "));
        //$logSerial.println(Reports_page);
        readFromEeprom(Reports_page);
      }
    }
    /***********Page 1,2,3,4****************/
    else if (pageId ==  (byte)1 || pageId ==  (byte)2 || pageId ==  (byte)3 || pageId ==  (byte)4) {

      if ((byte)4   == buttonId) {
        sendCommand("tm0.en=0");
        delay(100);
        sendCommand("tm1.en=0");
        delay(100);
        sendCommand("vis t1,1");
        delay(100);
        //        Serial.readString();
        Weight[(int)pageId - 1] = readText("t1.txt");
        //$logSerial.print(F("Reading  = ")); ////$logSerial.print(Weight[(int)pageId - 1]);
        Reading = false;
        accept = true;
        //$logSerial.println("page " + String(pageId) + " accept");
        //        WriteData("Press Continue to accept next weight or Press abort to\r\nre-enter the weight");
        sendCommand("t0.txt=va2.txt");
        delay(100);
        sendCommand("vis v0,0");//("vis p0,0");

      }
      else if ((byte)3   == buttonId) {
        //        accept = false;
        Reading = true;
        //$logSerial.println("page " + String(pageId) + " abort");
        sendCommand("tm0.en=1");
      }
      else if ((byte)2 == buttonId && accept) {
        Reading = false;
        accept = false;
        //$logSerial.println("page " + String(pageId) + " continue");
        sendCommand("page " + String(pageId + (byte)1));
        delay(200);
        if (pageId != (byte)4) {
          Reading = true;
        }
        else
          Reading = false;
        pageId++;
      }
    }
    /***********Page 5****************/
    else if (pageId == (byte)5) {
      Reading = false;
      if ((byte)2   == buttonId) {
        //$logSerial.println("page " + String(pageId) + " Display");
        sendCommand("page 6");
        delay(500);
        config[8] = readText("p6t0.txt");

        calculations();
        DisplayResults();
        addData();
      }
      else if ((byte)3  == buttonId ) {
        //$logSerial.println("page " + String(pageId) + " abort");
        sendCommand("page 0");
        delay(500);
      }
    }
    /***********Page 6****************/
    else if (pageId ==  (byte)6) {

      if ((byte)5 == buttonId) {
        //$logSerial.println("page " + String(pageId) + " Print");
        delay(200);
        //        readFromEeprom(Reports_page);
        print();
      }
      else if ((byte)2 == buttonId) {
        //$logSerial.println("page " + String(pageId) + " NewTest");
        //        sendCommand("page 0");
        resetFunc();
        for (int i = 0; i < 10; i++) config[i] = "";
        //        for (int i = 0; i < 10; i++) data[i] = "";
        delay(500);
      }
      else if ((byte)6 == buttonId) {
        for (int i = 0; i < 10; i++) {
          config[i] = "";
          //          data[i] = "";
          if (i < 4) Weight[i] = "";
        }
        delay(500);
        //$logSerial.println("page " + String(pageId) + " Reports");
        sendCommand("page 7");
        //        Serial.flush();
        delay(500);
        String pageNo = readText("t0.txt");
        Reports_page = pageNo.toInt();
        //$logSerial.print(F("Reports String= "));
        //$logSerial.println(pageNo);

        //$logSerial.print(F("Reports ID= "));
        //$logSerial.println(Reports_page);
        if (Reports_page > 1) Reports_page--; else Reports_page = 10;
        Serial.print("t0.txt=\"" + (String)Reports_page + "\"" + endBytes);
        readFromEeprom(Reports_page);
      }
    }
    /***********Page 7****************/
    else if (pageId == (byte)7) {

      if ((byte)2 == buttonId) {
        //$logSerial.println("page " + String(pageId) + " Print");
        delay(200);
        //        readFromEeprom(Reports_page);
        print();
      }
      else if ((byte)1 == buttonId) {
        //        $logSerial.println("page " + String(pageId) + " NewTest");
        //        sendCommand("page 0");
        resetFunc();
        for (int i = 0; i < 10; i++) config[i];
        Serial.flush();
      }
      else if ((byte)3 == buttonId) {
        for (int i = 0; i < 10; i++) config[i] = "";
        //$logSerial.println("page " + String(pageId) + " Next Page");
        String pageNo = readText("t0.txt");
        Reports_page = pageNo.toInt();
        //$logSerial.print(F("Reports String= "));
        //$logSerial.println(pageNo);

        //$logSerial.print(F("Reports ID= "));
        //$logSerial.println(Reports_page);
        if (Reports_page > 1) Reports_page--; else Reports_page = 10;
        Serial.print("t0.txt=\"" + (String)Reports_page + "\"" + endBytes);
        readFromEeprom(Reports_page);
      }
    }


  }
  Display_Weight();
}
void sendCommand(String Command) {
  Serial.print(Command + endBytes);
}
void WriteData(String Write) {
  String Command = "t0.txt=\"" + Write + "\"" + endBytes;
  Serial.print(Command);
}
void DisplayResults() {
  Serial.print("p6t0.txt=\"" + config[8] + "\"" + endBytes);
  Serial.print("p6t5.txt=\"" + config[0] + "\"" + endBytes);
  Serial.print("p6t1.txt=\"" + config[1] + "\"" + endBytes);
  Serial.print("p6t2.txt=\"" + config[2] + "\"" + endBytes);
  Serial.print("p6t6.txt=\"" + config[3] + "\"" + endBytes);
  Serial.print("p6t4.txt=\"" + config[4] + "\"" + endBytes);
  Serial.print("p6t7.txt=\"" + config[5] + "\"" + endBytes);
  Serial.print("p6t9.txt=\"" + config[6] + "\"" + endBytes);
  Serial.print("p6t8.txt=\"" + config[7] + "\"" + endBytes);
  Serial.print("p6t10.txt=\"" + config[7] + "\"" + endBytes);
}
void print() {
  //$logSerial.end();
  delay(100);
  Printer.begin(9600);
  delay(500);
  Printer.print(F("       ALUCAST MACHINES "));
  Printer.println();
  Printer.print(F("       DENSITY ANALYZER ")); 
  Printer.println();
  Printer.println();
  Printer.printf(F("TIME/DATE  ")); Printer.println(config[8]);
  Printer.println();
  Printer.print(F("PRODUCT NAME     ")); Printer.println(config[4]);
  Printer.print(F("ALUMINIUM TYPE   ")); Printer.println(config[1]);
  Printer.print(F("OPERATOR NAME    ")); Printer.println(config[3]);
  Printer.print(F("HEAT NO          ")); Printer.println(config[0]);
  Printer.print(F("FURNACE NO       ")); Printer.println(config[2]);
  Printer.println();
  //  Printer.printf(F("TIME            "),  "12:59:59");
  Printer.printf(F("DENSITY           (g/cc)\n")  );
  Printer.print(F("VACUUM            ")); Printer.println(config[5]);
  Printer.print(F("AIR               ")); Printer.println(config[6]);
  Printer.println();
  Printer.print(F("DENSITY INDEX     ")); Printer.println(config[7]);
//  Printer.print(F("RESULT            ")); Printer.println(config[7]);
  Printer.println();
  Printer.println();
  Printer.println();
  Printer.println();
  delay(100);
  //  Printer.end();
  delay(1000);
  nex.begin(115200);
}
void Display_Weight() {
  if (Reading)
  {
    ReadWeight();
    if (pageId == (byte)1) Serial.print("t1.txt=\"" + Weight[0] + "\"" + endBytes);
    else if (pageId == (byte)2) Serial.print("t1.txt=\"" + Weight[1] + "\"" + endBytes);
    else if (pageId == (byte)3) Serial.print("t1.txt=\"" + Weight[2] + "\"" + endBytes);
    else if (pageId == (byte)4) Serial.print("t1.txt=\"" + Weight[3] + "\"" + endBytes);
  }

}
void ReadWeight() {
  //$logSerial.end();
  delay(100);
  Wt_scale.begin(9600);
  delay(200);
  if (Wt_scale.available())
  {
    String x = Wt_scale.readStringUntil('g');
    int len = x.length();
    int dot = x.indexOf('.');
    x = x.substring(dot - 3, dot + 3);
    float y = x.toFloat();
    if (x.length() > 0)
    {
      if (pageId == (byte)1) Weight[0] = (String)y;
      else if (pageId == (byte)2)Weight[1] = (String)y;
      else if (pageId == (byte)3)Weight[2] = (String)y;
      else if (pageId == (byte)4) Weight[3] = (String)y;
    }
  }
  Wt_scale.end();
  delay(100);
  //$logSerial.begin(115200);
  delay(100);
}
void calculations() {
  float Dvac = 0, Dair = 0, Di = 0;
  float  Vaw1 = Weight[0].toFloat();
  float  Vww1 = Weight[1].toFloat();
  float  Aaw1 = Weight[2].toFloat();
  float  Aww1 = Weight[3].toFloat();
  //$logSerial.print(F("Vaw1 = ")); //$logSerial.print(Vaw1);
  //$logSerial.print(F("Vww1 = ")); //$logSerial.print(Vww1);
  //$logSerial.print(F("Aaw1 = ")); //$logSerial.print(Aaw1);
  //$logSerial.print(F("Aww1 = ")); //$logSerial.println(Aww1);
  if (Vaw1 > 0 && Vww1 > 0)
    Dvac = Vaw1 / (Vaw1 - Vww1);
  if (Aaw1 > 0 && Aww1 > 0)
    Dair = Aaw1 / (Aaw1 - Aww1);
  if (Dvac > 0 && Dair > 0)
    Di = ((Dair - Dvac) / Dair) * 100;
  else Di = 0.00;
  //$logSerial.print(F("Density Vac = "));
  //$logSerial.print(Dvac);
  //$logSerial.print(F("Density Air = "));
  //$logSerial.print(Dair);
  //$logSerial.print(F("Density Index = "));
  //$logSerial.println(Di);

  config[5] = (String)Dvac;
  config[6] = (String)Dair;
  config[7] = (String)Di;

}
void addData() {
  shiftRecords();
  data = "";
  for (int i = 0; i < 8; i++)  {
    data += config[i] + ":";
  }
  writeRecord(data, eAddress(39));
}
void readFromEeprom(int Report_page) {
  String ERead = "";
  ERead=readRecord(eAddress(Report_page));
  for (int i = 0; i < 10; i++) {
    int last = ERead.indexOf(':');
    config[i] = ERead.substring(0, last);
    ERead = ERead.substring(last + 1);
    //    config_len[i] = config[i].length();
  }
  config[8] = config[8] + ":" + config[9];
  //$logSerial.print(F("Data = "));
  for (auto val : config) nex.print(val), nex.print('\t');
  DisplayResults();
  sendCommand("vis v0,0");
  ERead = "";
  
}

String readText(String objname) {
  Serial.print("get " + objname + endBytes);
  String input = Serial.readString();
  input = input.substring(1, input.length() - 3 );
  return input;
}

void shiftRecords() {
  for (byte index = 1; index < records_count; index++)
    writeRecord(readRecord(eAddress(index)), eAddress(index - 1));
}

//void readRecords() {
//  for (byte index = 0; index < records_count; index++)
//    Serial.println(readRecord(eAddress(index)));
//}

String readRecord(unsigned int _address) {
  _data  = "";

  for (byte index = 0, _length  = readByte(_address); index < _length; index++)
    _data += (char)readByte(_address + 1 + index);

  return _data;
}

void writeRecord(String _data, unsigned int _address) {
  byte _length = _data.length();
  writeByte(_address, _length);                                                                 // write length on first location

  for (byte index = 0; index < _length; index++)
    writeByte( _address + 1 + index, _data[index]);                                             // write data onwards
}

byte readByte(int _address) {
  byte _low     = _address & 0xFF;                                                              // lsb
  byte _high    = _address >> 8;                                                                // msb
  Wire.beginTransmission(i2cAddress);                                                           //
  Wire.write(_high);                                                                            // First Word Address
  Wire.write(_low);                                                                             // Second Word Address
  Wire.endTransmission();                                                                       // stop transmitting

  Wire.requestFrom(i2cAddress, (byte)1);                                                        // request 1 byte from slave
  return Wire.read();
}

void writeByte(int _address, byte _data) {
  byte _low     = _address & 0xFF;                                                              // lsb
  byte _high    = _address >> 8;                                                                // msb
  Wire.beginTransmission(i2cAddress);                                                           // begin transmission
  Wire.write(_high);                                                                            // First Word Address
  Wire.write(_low);                                                                             // Second Word Address
  Wire.write(_data);                                                                            // data
  Wire.endTransmission();                                                                       // stop transmitting
  delay(2);                                                                                     // small delay
}
