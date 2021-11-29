#include <SoftwareSerial.h>
#define endBytes  "\xFF\xFF\xFF"

  float Dvac,Dair,Di;
boolean  accept = false, Reading = false;
int blink = 0, on = 0;
long int tim = 0;
long milis;
int send_cnt = 0;
byte pageId ;
String Weight[4] = {"0.00", "0.00", "0.00", "0.00"};
SoftwareSerial nex(3, 2);
SoftwareSerial Printer(6, 7);
SoftwareSerial Wt_scale(4, 5);

#define logSerial Serial
#define nexSerial nex

int config_len[6];
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
  nex.begin(115200);
  //  Serial.println(readText("p0t3.txt"));
  //  delay(100);
  sendCommand("page 0");
}
void loop() {
  readNex();
  Display_Weight();

}
String readText(String objname) {
  nexSerial.print("get " + objname + endBytes);
  String input = nexSerial.readString();
  input = input.substring(1, input.length() - 3 );
  return input;
}

void sendCommand(String Command)
{
  nexSerial.print(Command + endBytes);
}

void readNex()
{

  if (nexSerial.available()) {
    /***Read Input Data from Display****/
    String Data = nexSerial.readString();
    pageId = (byte)Data[1];
    byte buttonId = (byte)Data[2];
    logSerial.println(Data);

    /***********Page 0****************/
    if (startTest1.page == pageId ) {
      if (startTest1.id == buttonId) {
        logSerial.println("page 0 start");
        int first = 0;
        Data = Data.substring(8);
        logSerial.println(Data);
        for (int i = 0; i < 6; i++) {
          int last = Data.indexOf(':');
          config[i] = Data.substring(0, last);
          Data = Data.substring(last + 1);
          config_len[i] = config[i].length();

        }
        for (auto val : config) logSerial.print(val), logSerial.print('\t');

        sendCommand("page 1");
        pageId = (byte)1;
        Reading = true;
        blink = 1;
        on = 0;

      }
      else if (reports0.id  == buttonId) {
        logSerial.println("page 0 reports");
      }
    }
    /***********Page 1,2,3,4****************/
    else if (pageId ==  (byte)1 || pageId ==  (byte)2 || pageId ==  (byte)3 || pageId ==  (byte)4) {
      
      if (accept1.id   == buttonId) {
        sendCommand("tm0.en=0");
        delay(100);
        sendCommand("vis t1,1");
        delay(100);
        nexSerial.readString();
        Weight[(int)pageId-1]=readText("t1.txt");
        logSerial.print(F("Reading  = "));logSerial.print(Weight[(int)pageId-1]);
        Reading = false;
        accept = true;
        logSerial.println("page " + String(pageId) + " accept");
//        WriteData("Press Continue to accept next weight or Press abort to\r\nre-enter the weight");
        sendCommand("t0.txt=va2.txt");
        delay(100);
        sendCommand("vis p0,0");
        
      }
      else if (abort1.id    == buttonId) {
//        accept = false;
        Reading = true;
        logSerial.println("page " + String(pageId) + " abort");
        sendCommand("tm0.en=1");
      }
      else if (continue1.id == buttonId && accept) {
        Reading = false;
        accept = false;
        logSerial.println("page " + String(pageId) + " continue");
        sendCommand("page " + String(pageId + (byte)1));
        delay(200);
        if (pageId != (byte)4) {Reading = true;}
        else
          Reading = false;
        pageId++;
      }
    }
    /***********Page 5****************/
    else if (pageId == (byte)5) {
      Reading = false;
      if (continue1.id   == buttonId) {
        logSerial.println("page " + String(pageId) + " Display");
        sendCommand("page 6");
        delay(500);
        DisplayResults();
      }
      else if (abort1.id  == buttonId ) {
        logSerial.println("page " + String(pageId) + " abort");
        sendCommand("page 0");
      }
    }
    /***********Page 6****************/
    else if (pageId ==  (byte)6) {
      if ((byte)5 == buttonId) {
        logSerial.println("page " + String(pageId) + " Print");
        delay(200);
        print();
      }
      else if ((byte)2 == buttonId) {
        logSerial.println("page " + String(pageId) + " NewTest");
        sendCommand("page 0");
        nexSerial.flush();

      }
    }
  }
}


//void Delay(int d)
//{
//  long milis = millis();
//  milis = milis + d;
//  while(millis()<milis)
//  {
//    readNex();
//  }
//}

void WriteData(String Write)
{
  String Command = "t0.txt=\"" + Write + "\"" + endBytes;
  nexSerial.print(Command);
}

void DisplayResults()
{
  calculations();
  nexSerial.print("p6t5.txt=\"" + config[0] + "\"" + endBytes);
  nexSerial.print("p6t1.txt=\"" + config[1] + "\"" + endBytes);
  nexSerial.print("p6t2.txt=\"" + config[2] + "\"" + endBytes);
  nexSerial.print("p6t6.txt=\"" + config[3] + "\"" + endBytes);
  nexSerial.print("p6t4.txt=\"" + config[4] + "\"" + endBytes);
  nexSerial.print("p6t7.txt=\"" + (String)Dvac + "\"" + endBytes);
  nexSerial.print("p6t9.txt=\"" + (String)Dair+ "\"" + endBytes);
  nexSerial.print("p6t8.txt=\"" + (String)Di+ "\"" + endBytes);
}

void print() {
  Printer.begin(9600);
  delay(500);
  char buff[5];
  dtostrf(4.85, 4, 2, buff);
//  Printer.println();
//  Printer.println();
  Printer.print(F("ALUCAST MACHINES ")); Printer.println(config[4]);
  Printer.print(F("DENSITY ANALYZER ")); Printer.println(config[1]);
  Printer.println();
  Printer.print(F("OPERATOR NAME    ")); Printer.println(config[3]);
  Printer.print(F("HEAT NO          ")); Printer.println(config[0]);
  Printer.println();
  Printer.printf(F("DATE              %0.8s\n"),  "26/07/21");
  Printer.printf(F("TIME              %0.8s\n"),  "12:59:59");
  Printer.println();
  Printer.printf(F("DENSITY           (g/cc)\n")  );
  Printer.print(F("VACUUM            ")); Printer.println(Dvac);
  Printer.print(F("AIR               ")); Printer.println(Dair);
  Printer.println();
  Printer.print(F("DENSITY INDEX     ")); Printer.println(Di);
  Printer.println();
  Printer.println();
  Printer.println();
  Printer.println();
//  Printer.println();
  delay(1000);
  nexSerial.begin(115200);
}
void Display_Weight()
{
 
  if (Reading)
  {
    send_cnt++;
//    logSerial.println(send_cnt);
    ReadWeight();
//    if (send_cnt > 5)
    {
      
      send_cnt = 0;
      if (pageId == (byte)1) nexSerial.print("t1.txt=\"" + Weight[0] + "\"" + endBytes);
      else if (pageId == (byte)2) nexSerial.print("t1.txt=\"" + Weight[1] + "\"" + endBytes);
      else if (pageId == (byte)3) nexSerial.print("t1.txt=\"" + Weight[2] + "\"" + endBytes);
      else if (pageId == (byte)4) nexSerial.print("t1.txt=\"" + Weight[3] + "\"" + endBytes);
      //    Reading = false;
    }
  }

}

void ReadWeight()
{
  Wt_scale.begin(9600);
  delay(100);
   if (Wt_scale.available())
  {
    String x = Wt_scale.readStringUntil('g');
    int len = x.length();
    int dot = x.indexOf('.');
    x=x.substring(dot-3,dot+3);
    float y = x.toFloat();
//    logSerial.print(F("Weight = "));
//    logSerial.println(x);
    if (pageId == (byte)1) Weight[0]=(String)y;
    else if (pageId == (byte)2)Weight[1]=(String)y;
    else if (pageId == (byte)3)Weight[2]=(String)y;
    else if (pageId == (byte)4) Weight[3]=(String)y;
//     logSerial.print(F("Weight = "));
//     logSerial.println(Weight[0]);
  }
  nexSerial.begin(115200);
  delay(100);

}

void calculations()
{
  
  float  Vaw1 = Weight[0].toFloat();
  float  Vww1 = Weight[1].toFloat();
  float  Aaw1 = Weight[2].toFloat();
  float  Aww1 = Weight[3].toFloat();
  logSerial.print(F("Vaw1  = "));logSerial.print(Vaw1);
  logSerial.print(F("Vww1 = "));logSerial.print(Vww1);
  logSerial.print(F("Aaw1 = "));logSerial.print(Aaw1);
  logSerial.print(F("Aww1 = "));logSerial.println(Aww1);
  Dvac = Vaw1/(Vaw1-Vww1);
  Dair = Aaw1/(Aaw1-Aww1);
  Di = ((Dair-Dvac)/Dair)*100;
  logSerial.print(F("Density Vac = "));
  logSerial.print(Dvac);
  logSerial.print(F("Density Air = "));
  logSerial.print(Dair);
  logSerial.print(F("Density Index = "));
  logSerial.println(Di);
}
