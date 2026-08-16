
#include <Controllino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>


long TIME = 0;
long START = 0;
long AKT = 0;
long WAIT = 30000;
int SPEED = 15;
int INTENS = 0;
int Schlafzimmer = 32;
int Ankleide = 33;
int Bad = 34;
int Kinderzimmer = 4;
int Kueche = 36;
int KuecheLED = 2;
int Wohnzimmer1 = 37;
int Wohnzimmer2 = 25;
int SZDIM = 5;
int AUF = 30;
int ZU = 31;
int LEDSZSoll = 0;
int LEDSZIst = 0;
int LEDAKSoll = 0;
int LEDAKIst = 0;
int SZRTastAUF = A1;
int SZRTastZU = A0;
int ANKLTast = A2;
int SZTast = A3;
int FLTast = A15;
int BWTuer = A13;
int BWEcke = A14;
int BWGWC = A10;
int ROLLOSZSTATE = 0;
int ROLLORUNTIME = 0;
int TasterAK = 0;
int LICHTAK = 0;
int SZTASTLO = 0;
int SZTASTLU = 0;
int SZTASTRO = 0;
int SZTASTRU = 0;
int SZM = 0;
int SZCOUNTaLED = 0;
int SZCOUNTbLED = 0;
int WZLAUF = 27;
int WZLZU = 24;
int WZRAUF = 28;
int WZRZU = 29;


int WZLEDTVa = 0;
int WZLAMPa = 0;
int TVLAMPa = 0;
int WZLEDESSa = 0;
int WZLAMPESSa = 0;
int KLAMPa = 0;
int KLEDa = 0;
int BadA = 0;  // zuletzt gesendeter AN/AUS-Zustand fuer den Badspiegel-Relais (fuer die Web-UI-Statusanzeige)
int WZLEDTV;
int WZLEDESS;
int WZLEDTVSoll = 0;
int WZLEDESSSoll = 0;
int WZLdrive = 0;
int WZRdrive = 0;
int krdrive = 0;
int longpress = 0;
int dimmingess = 0;
int directioness = 0;
int dimmingb1 = 0;
int directionb1 = 0;
int dimmingb2 = 0;
int directionb2 = 0;


int LEDFL;
int LEDFLSoll;
int LEDFLa;
int FLLEDMAX = 700;

int LEDGWC;
int LEDGWCSoll;
int LEDGWCa;
int GWCLEDMAX = 240;

int LEDPROG = 0;
int LEDSTATE = 0;
int TuerSTATE = 0;
int EckeSTATE = 0;
int BADTASTLO = 0;
int BADTASTLOa = 0;
int BADTASTLU = 0;
int BADTASTLUa = 0;
int BADTASTRO = 0;
int BADTASTROa = 0;
int BADTASTRU = 0;
int BADTASTRUa = 0;
int BADTASTLORO = 0;
int BADTASTLOROa = 0;
int BADTASTLURU = 0;
int BADTASTLURUa = 0;
int LEDBAD1 = 0;
int LEDBAD2 = 0;
float LEDBAD1Soll = 0;
float LEDBAD2Soll = 0;
float LEDBAD1d = 1;
float LEDBAD2d = 1;
int DIMM = 10;
int DIMMcount = 0;
int BADF1 = 0;                 //Umwälzpumpe
unsigned long BADF1Count = 0;  //Zähler
int BADF1Time = 80;            //Umwälzlaufzeit
int BADF1MaxSpeed = 750;       //Pumpen Maximalgeschwindigkeit
int BADF1Soll = 0;
unsigned long BADF1Wait = 3000;
unsigned long BADF1WaitCount = 0;
int BADF2 = 0;  //Spiegelschrank

unsigned long AKTSek = 0;

String valueStringSZ = String(0);
String valueStringWZ = String(0);
String valueStringFL = String(0);
String valueStringWZESS = String(0);
String valueswitch = String(0);
String valuebutton = String(0);
String valuestate = String(0);

int Switch = 0;
int Button = 0;
int State = 0;
int hold = 0;

int pos1 = 0;
int pos2 = 0;
int pos3 = 0;
int pos4 = 0;
int pos5 = 0;
int pos6 = 0;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  //physical mac address
byte ip[] = { 192, 168, 2, 200 };                     // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte gateway[] = { 192, 168, 2, 1 };                  // internet access via router
byte subnet[] = { 255, 255, 255, 0 };                 //subnet mask
EthernetServer server(80);                            //server port
String readString;
String readString2;
char packet[255];
char packet2[255];

unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement

IPAddress C2(192, 168, 2, 201);
EthernetUDP UDP;
EthernetUDP UDP2;

// LED-Aussen Empfaenger fuer die Sequenz-Ansteuerung per UDP (vorher bei jedem loop()-Durchlauf neu angelegt)
IPAddress LED1(192, 168, 2, 101);
IPAddress LED2(192, 168, 2, 102);
IPAddress LED3(192, 168, 2, 103);
IPAddress LED4(192, 168, 2, 104);
IPAddress LED5(192, 168, 2, 105);
IPAddress LED6(192, 168, 2, 106);
IPAddress LED7(192, 168, 2, 107);
IPAddress LED8(192, 168, 2, 108);
IPAddress LED9(192, 168, 2, 109);
IPAddress LED10(192, 168, 2, 110);

struct LedSend {
  IPAddress ip;
  long offset;
};

// Ersetzt die vorher 3x kopierten Bloecke aus je 20 UDP.beginPacket()/print()-Aufrufen.
void sendLEDPacket(IPAddress dest, long start, long offset, int speed, int intens) {
  long akt = millis();
  long time = start - akt - offset;
  if (time <= -1) {
    time = 0;
  }
  UDP.beginPacket(dest, 4210);
  UDP.print("LEDT");
  UDP.print(time);
  UDP.print("tS");
  UDP.print(speed);
  UDP.print("sI");
  UDP.print(intens);
  UDP.println("i");
  UDP.endPacket();
  delay(5);
}

void sendLEDSequence(LedSend seq[], int count, int speed, int intens) {
  START = millis() + 3000;
  for (int i = 0; i < count; i++) {
    sendLEDPacket(seq[i].ip, START, seq[i].offset, speed, intens);
  }
}

// Ersetzt die vorher 15x kopierten "EthernetClient client; connect(C2,80); println(...); stop();" Bloecke.
void sendRelayCommand(const char* code) {
  EthernetClient client;
  if (client.connect(C2, 80)) {
    client.println(code);
    delay(10);
  }
  client.stop();
}

// Web-UI: Karte mit zwei Buttons (z.B. AN/AUS oder Start/STOP).
// Alle Strings kommen ueber F() aus dem Flash, damit die Web-UI kein zusaetzliches RAM kostet.
// stateKey markiert beide Buttons mit data-k/data-v, damit das Polling-JS im /state-Status
// den jeweils aktuellen Button hervorheben kann (siehe pollState() im Seiten-Script).
void printToggleCard(EthernetClient &c, const __FlashStringHelper* title, const __FlashStringHelper* stateKey,
                      const __FlashStringHelper* onHref, const __FlashStringHelper* onLabel,
                      const __FlashStringHelper* offHref, const __FlashStringHelper* offLabel) {
  c.print(F("<div class='card'><h3>"));
  c.print(title);
  c.print(F("</h3><div class='btnrow'><a class='btn on' data-k='"));
  c.print(stateKey);
  c.print(F("' data-v='1' href='"));
  c.print(onHref);
  c.print(F("'>"));
  c.print(onLabel);
  c.print(F("</a><a class='btn off' data-k='"));
  c.print(stateKey);
  c.print(F("' data-v='0' href='"));
  c.print(offHref);
  c.print(F("'>"));
  c.print(offLabel);
  c.println(F("</a></div></div>"));
}

// Web-UI: Karte mit drei Buttons (z.B. AUF/STOP/ZU oder Sequenz1/Sequenz2/AUS).
void printLinkCard3(EthernetClient &c, const __FlashStringHelper* title,
                     const __FlashStringHelper* href1, const __FlashStringHelper* label1,
                     const __FlashStringHelper* href2, const __FlashStringHelper* label2,
                     const __FlashStringHelper* href3, const __FlashStringHelper* label3) {
  c.print(F("<div class='card'><h3>"));
  c.print(title);
  c.print(F("</h3><div class='btn3'><a class='btn on' href='"));
  c.print(href1);
  c.print(F("'>"));
  c.print(label1);
  c.print(F("</a><a class='btn stop' href='"));
  c.print(href2);
  c.print(F("'>"));
  c.print(label2);
  c.print(F("</a><a class='btn off' href='"));
  c.print(href3);
  c.print(F("'>"));
  c.print(label3);
  c.println(F("</a></div></div>"));
}

// Web-UI: Rollo-Karte mit AUF/STOP/ZU.
void printRolloCard(EthernetClient &c, const __FlashStringHelper* title,
                     const __FlashStringHelper* upHref, const __FlashStringHelper* stopHref,
                     const __FlashStringHelper* downHref) {
  printLinkCard3(c, title, upHref, F("AUF"), stopHref, F("STOP"), downHref, F("ZU"));
}

// Web-UI: Karte mit Dimmer-Slider.
void printSliderCard(EthernetClient &c, const __FlashStringHelper* title,
                      const __FlashStringHelper* sliderId, const __FlashStringHelper* spanId,
                      int value) {
  c.print(F("<div class='card'><h3>"));
  c.print(title);
  c.print(F("</h3><p class='val'>LED: <span id='"));
  c.print(spanId);
  c.print(F("'>"));
  c.print(value);
  c.print(F("</span></p><input type='range' min='0' max='1023' class='slider' id='"));
  c.print(sliderId);
  c.print(F("' value='"));
  c.print(value);
  c.println(F("'></div>"));
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for Leonardo only
  }


  TCCR0A = 0;
  TCCR0B = 0;
  TCCR0A = (1 << WGM00) | (1 << WGM01) | (1 << COM0A1) | (1 << COM0A0) | (1 << COM0B1) | (1 << COM0B0);
  TCCR0B = (1 << CS01) | (1 << CS00);

  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1A = (1 << WGM10) | (1 << WGM11) | (1 << COM1B1);
  TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);

  TCCR2A = 0;
  TCCR2B = 0;
  TCCR2A = (1 << WGM20) | (1 << WGM21) | (1 << COM2A1) | (1 << COM2A0) | (1 << COM2B1) | (1 << COM2B0);
  TCCR2B = (1 << CS21) | (1 << CS20);

  TCCR3A = 0;
  TCCR3B = 0;
  TCCR3A = (1 << WGM30) | (1 << WGM31) | (1 << COM3A1) | (1 << COM3C1) | (1 << COM3C0);
  TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);

  TCCR4A = 0;
  TCCR4B = 0;
  TCCR4A = (1 << WGM40) | (1 << WGM41) | (1 << COM4A1) | (1 << COM4A0) | (1 << COM4B1) | (1 << COM4B0) | (1 << COM4C1) | (1 << COM4C0);
  TCCR4B = (1 << WGM42) | (1 << CS41) | (1 << CS40);

  DDRH |= (1 << DDH3);
  DDRH |= (1 << DDH4);
  DDRH |= (1 << DDH5);
  DDRH |= (1 << DDH6);
  DDRE |= (1 << DDE3);
  DDRE |= (1 << DDE5);
  DDRB |= (1 << DDB4);
  DDRB |= (1 << DDB6);
  DDRB |= (1 << DDB7);




  pinMode(Schlafzimmer, OUTPUT);
  pinMode(Ankleide, OUTPUT);
  pinMode(Bad, OUTPUT);
  pinMode(Kinderzimmer, OUTPUT);
  pinMode(Kueche, OUTPUT);
  pinMode(KuecheLED, OUTPUT);
  pinMode(Wohnzimmer1, OUTPUT);
  pinMode(Wohnzimmer2, OUTPUT);
  pinMode(SZDIM, OUTPUT);
  pinMode(AUF, OUTPUT);
  pinMode(ZU, OUTPUT);
  pinMode(WZLAUF, OUTPUT);
  pinMode(WZLZU, OUTPUT);
  pinMode(WZRAUF, OUTPUT);
  pinMode(WZRZU, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  // start the Ethernet connection and the server:

  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  UDP.begin(4210);
  UDP2.begin(1412);
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {

  AKTSek = (millis() / 1000);
  int SZRSTATEAUF = analogRead(SZRTastAUF);
  int SZRSTATEZU = analogRead(SZRTastZU);
  int ANKLSTATE = analogRead(ANKLTast);
  int TAST1STATE = analogRead(SZTast);
  int TAST2STATE = analogRead(A6);
  int FLSTATE = analogRead(FLTast);
  int TuerSTATE = analogRead(BWTuer);
  int EckeSTATE = analogRead(BWEcke);
  int GWCSTATE = analogRead(BWGWC);
  //Serial.println(LEDSZSoll);
  //Serial.println(valueString);
  // Serial.println(SZRSTATEAUF);
  //Serial.println(SZRSTATEZU);
  //Serial.println(LEDBAD1);
  //Serial.println( LEDBAD2);

  int packetSize = UDP2.parsePacket();
  if (packetSize) {
    int len = UDP2.read(packet2, 255);
    if (len > 0) {
      packet2[len] = '\0';
    }
    readString2 = packet2;
  }

  if (readString2.indexOf("Schalter") >= 0) {
    pos1 = readString2.indexOf('r');
    pos2 = readString2.indexOf('b');
    valueswitch = readString2.substring(pos1 + 2, pos2 - 1);
    Switch = (valueswitch.toInt());

    pos3 = readString2.indexOf('u');
    pos4 = readString2.indexOf('S');
    valuebutton = readString2.substring(pos3 + 2, pos4 - 1);
    Button = (valuebutton.toInt());

    pos5 = readString2.indexOf('=');
    pos6 = readString2.indexOf('=');
    valuestate = readString2.substring(pos5 + 1, pos6 + 3);
    State = (valuestate.toInt());
    readString2 = "0";
    Serial.println("data=");
    Serial.println(Switch);
    Serial.println(Button);
    Serial.println(State);
    //________________________________________

    /*Kueche, HIGH);
                KLAMPa=1;*/

    switch (Switch) {
      case 50:
        switch (Button) {
          case 0:
            if (State == 1) {
              if (KLAMPa == 1) {
                digitalWrite(Kueche, LOW);
                KLAMPa = 0;

              } else if (KLAMPa == 0) {
                digitalWrite(Kueche, HIGH);
                KLAMPa = 1;
              }
            }
            if (State == 11) {}
            if (State == 10 && hold == 1) {}
            if (State == 0) {}

            break;
          case 4:
            break;
          case 5:
            break;
          case 12:
            if (State == 1) {
              if (KLEDa == 1) {
                digitalWrite(KuecheLED, HIGH);
                KLEDa = 0;

              } else if (KLEDa == 0) {
                digitalWrite(KuecheLED, LOW);
                KLEDa = 1;
              }
            }
            if (State == 11) {}
            if (State == 10 && hold == 1) {}
            if (State == 0) {}

            break;
          case 16:
            break;
        }
        break;
      //_______________________________________________________________________________________________________________

      /*if (readString.indexOf("?krbuttonhoch") > 0 ) {
            digitalWrite (23, LOW);
            delay(20);
            digitalWrite (22, HIGH);
            krdrive=1;
          }
          if (readString.indexOf("?krbuttonrunter") > 0 ) {
            digitalWrite (22, LOW);
            delay(20);
            digitalWrite (23, HIGH);
            krdrive=1;
          }
          if (readString.indexOf("?krbuttonstop") > 0 ) {
            digitalWrite (22, LOW);
            digitalWrite (23, LOW);
            krdrive=0;
          }*/
      case 51:
        switch (Button) {
          case 0:
            if (State == 1) {
              if (krdrive == 1) {
                digitalWrite(22, LOW);
                digitalWrite(23, LOW);
                krdrive = 0;

              } else if (krdrive == 0) {
                digitalWrite(23, HIGH);
                krdrive = 1;
              }
            }
            if (State == 11) {
              digitalWrite(23, HIGH);
              hold = 1;
            }
            if (State == 10 && hold == 1) {
              digitalWrite(23, LOW);
              hold = 0;
            }
            if (State == 0) {
            }
            break;
          case 4: /*
            if (State == 1) {

              if (Case == 41) {
                Case = 40;
              } else if (Case == 42) {
                Case = 40;
              } else if (Case == 40) {
                Case = 42;
              }
            }
            if (State == 11) {
              Case = 42;
              hold = 1;
            }
            if (State == 10 && hold == 1) {
              Case = 40;
              hold = 0;
            }
            if (State == 0) {

            }*/
            break;
          case 5:
            if (State == 1) {
              if (krdrive == 1) {
                digitalWrite(22, LOW);
                digitalWrite(23, LOW);
                krdrive = 0;

              } else if (krdrive == 0) {
                digitalWrite(22, HIGH);
                krdrive = 1;
              }
            }
            if (State == 11) {
              digitalWrite(22, HIGH);
              hold = 1;
            }
            if (State == 10 && hold == 1) {
              digitalWrite(22, LOW);
              hold = 0;
            }
            if (State == 0) {
            }
            break;
          case 12: /*
            if (State == 1) {

              if (Case == 41) {
                Case = 40;
              } else if (Case == 42) {
                Case = 40;
              } else if (Case == 40) {
                Case = 41;
              }
            }
            if (State == 11) {
              Case = 41;
              hold = 1;
            }
            if (State == 10 && hold == 1) {
              Case = 40;
              hold = 0;
            }
            if (State == 0) {

            }*/
            break;
          case 16:
            break;
        }
        break;
      //______________________________________________________________________________________________________________________
      case 52:
        switch (Button) {
          case 0: /*
            if (State == 1) {

              if (Case == 41) {
                Case = 40;
              } else if (Case == 42) {
                Case = 40;
              } else if (Case == 40) {
                Case = 41;
              }
            }
            if (State == 11) {
              Case = 41;
              hold = 1;
            }
            if (State == 10 && hold == 1) {
              Case = 40;
              hold = 0;
            }
            if (State == 0) {

            }*/
            break;
          case 4:
            if (State == 1) {
              if (krdrive == 1) {
                digitalWrite(22, LOW);
                digitalWrite(23, LOW);
                krdrive = 0;

              } else if (krdrive == 0) {
                digitalWrite(22, HIGH);
                krdrive = 1;
              }
            }
            if (State == 11) {
              digitalWrite(22, HIGH);
              hold = 1;
            }
            if (State == 10 && hold == 1) {
              digitalWrite(22, LOW);
              hold = 0;
            }
            if (State == 0) {
            }
            break;
          case 5:
            /*if (State == 1) {

              if (Case == 41) {
                Case = 40;
              } else if (Case == 42) {
                Case = 40;
              } else if (Case == 40) {
                Case = 42;
              }
              }
              if (State == 11) {
              Case = 42;
              hold = 1;
              }
              if (State == 10 && hold == 1) {
              Case = 40;
              hold = 0;
              }
              if (State == 0) {

              }*/
            break;
          case 12:
            if (State == 1) {
              if (krdrive == 1) {
                digitalWrite(22, LOW);
                digitalWrite(23, LOW);
                krdrive = 0;

              } else if (krdrive == 0) {
                digitalWrite(23, HIGH);
                krdrive = 1;
              }
            }
            if (State == 11) {
              digitalWrite(23, HIGH);
              hold = 1;
            }
            if (State == 10 && hold == 1) {
              digitalWrite(23, LOW);
              hold = 0;
            }
            if (State == 0) {
            }
            break;
          case 16:
            break;
        }
        break;
      //______________________________________________________________________________________________________________________
      case 53:
        switch (Button) {
          case 0:
            if (State == 1) {
              if (WZRdrive == 1) {
                digitalWrite(WZRAUF, LOW);
                digitalWrite(WZRZU, LOW);
                WZRdrive = 0;

              } else if (WZRdrive == 0) {
                digitalWrite(WZRZU, HIGH);
                WZRdrive = 1;
              }
            }
            if (State == 11) {
              digitalWrite(WZRZU, HIGH);
              hold = 1;
            }
            if (State == 10 && hold == 1) {
              digitalWrite(WZRZU, LOW);
              hold = 0;
            }
            if (State == 0) {
            }
            break;
          case 4:
            if (State == 1) {
              if (WZRdrive == 1) {
                digitalWrite(WZRAUF, LOW);
                digitalWrite(WZRZU, LOW);
                WZRdrive = 0;

              } else if (WZRdrive == 0) {
                digitalWrite(WZRAUF, HIGH);
                WZRdrive = 1;
              }
            }
            if (State == 11) {
              digitalWrite(WZRAUF, HIGH);
              hold = 1;
            }
            if (State == 10 && hold == 1) {
              digitalWrite(WZRAUF, LOW);
              hold = 0;
            }
            if (State == 0) {
            }
            break;
          case 5:
            if (State == 1) {
              if (WZLdrive == 1) {
                digitalWrite(WZLAUF, LOW);
                digitalWrite(WZLZU, LOW);
                WZLdrive = 0;

              } else if (WZLdrive == 0) {
                digitalWrite(WZLAUF, HIGH);
                WZLdrive = 1;
              }
            }
            if (State == 11) {
              digitalWrite(WZLAUF, HIGH);
              hold = 1;
            }
            if (State == 10 && hold == 1) {
              digitalWrite(WZLAUF, LOW);
              hold = 0;
            }
            if (State == 0) {
            }
            break;
          case 12:
            if (State == 1) {
              if (WZLdrive == 1) {
                digitalWrite(WZLAUF, LOW);
                digitalWrite(WZLZU, LOW);
                WZLdrive = 0;

              } else if (WZLdrive == 0) {
                digitalWrite(WZLZU, HIGH);
                WZLdrive = 1;
              }
            }
            if (State == 11) {
              digitalWrite(WZLZU, HIGH);
              hold = 1;
            }
            if (State == 10 && hold == 1) {
              digitalWrite(WZLZU, LOW);
              hold = 0;
            }
            if (State == 0) {
            }
            break;
          case 16:
            break;
        }
        break;
        //______________________________________________________________________________________________________________________

        /*          if (readString.indexOf("?button6on") > 0 ) {
                  digitalWrite(Wohnzimmer1, HIGH);
                  WZLAMPa = 1;


                }
                if (readString.indexOf("?button6off") > 0) {
                  digitalWrite(Wohnzimmer1, LOW);
                  WZLAMPa = 0;
                }
                if (readString.indexOf("?button7on") > 0 ) {
                  digitalWrite(Wohnzimmer2, HIGH);
                  TVLAMPa = 1;

                }
                if (readString.indexOf("?button7off") > 0) {
                  digitalWrite(Wohnzimmer2, LOW);
                  TVLAMPa = 0;
                }*/

      case 54:
        switch (Button) {
          case 0:
            if (State == 1) {
              if (TVLAMPa == 1) {
                digitalWrite(Wohnzimmer2, LOW);
                TVLAMPa = 0;

              } else if (TVLAMPa == 0) {
                digitalWrite(Wohnzimmer2, HIGH);
                TVLAMPa = 1;
              }
            }
            if (State == 11) {}
            if (State == 10 && hold == 1) {}
            if (State == 0) {}

            break;

            /* if (WZLEDTVSoll < 49) {
              WZLEDTVSoll = 0;
              WZLEDTVa = 0;
            }

            if (WZLEDTV > WZLEDTVSoll) {
              WZLEDTV--;
              delay(2);
            }
            if (WZLEDTV < WZLEDTVSoll) {
              WZLEDTV++;
              delay(2);
            }

            if (WZLEDESSSoll < 49) {
              WZLEDESSSoll = 0;
              WZLEDESSa = 0;
            }

            if (WZLEDESS > WZLEDESSSoll) {
              WZLEDESS--;
              delay(2);
            }
            if (WZLEDESS < WZLEDESSSoll) {
              WZLEDESS++;
              delay(2);
            }*/

          case 4:  //ess
            if (State == 1) {
            }
            if (State == 11) {
              dimmingess = 1;
              longpress = 1;
              if (directioness == 1) {
                directioness = 0;
              } else {
                directioness = 1;
              }
            }
            if (State == 10) {
              dimmingess = 0;
            }
            if (State == 0) {
              if (WZLEDESSSoll == 0 && longpress == 0) {
                WZLEDESSSoll = 60;
              } else if (WZLEDESSSoll >= 49 && longpress == 0) {
                WZLEDESSSoll = 0;
              }
              longpress = 0;
            }
            break;
          case 5:  //TV
            if (State == 1) {
            }
            if (State == 11) {
            }
            if (State == 10) {
            }
            if (State == 0) {
            }
            break;
          case 12:
            if (State == 1) {
              if (WZLAMPa == 1) {
                digitalWrite(Wohnzimmer1, LOW);
                WZLAMPa = 0;

              } else if (WZLAMPa == 0) {
                digitalWrite(Wohnzimmer1, HIGH);
                WZLAMPa = 1;
              }
            }
            if (State == 11) {}
            if (State == 10 && hold == 1) {}
            if (State == 0) {}


            break;
          case 16:
            break;
        }
        break;
      //______________________________________________________________________________________________________________________
      case 55:
        switch (Button) {
          case 0:
            break;
          case 4:
            break;
          case 5:
            break;
          case 12:
            break;
          case 16:
            break;
        }
        break;
      //______________________________________________________________________________________________________________________
      case 56:
        switch (Button) {
          case 0:
            if (State == 1) {
            }
            if (State == 11) {
              dimmingb2 = 1;
              longpress = 1;
              if (directionb2 == 1) {
                directionb2 = 0;
              } else {
                directionb2 = 1;
              }
            }
            if (State == 10) {
              dimmingb2 = 0;
            }
            if (State == 0) {
              if (LEDBAD2Soll == 0 && longpress == 0) {
                LEDBAD2Soll = 100;
              } else if (LEDBAD2Soll >= 13 && longpress == 0) {
                LEDBAD2Soll = 0;
              }
              longpress = 0;
            }
            break;
          case 4:
            if (State == 11) {

              IPAddress HOST_NAME_BAD(192, 168, 2, 213);  // hostname of web server:
              String PATH_NAME_BAD = "/relay/0";
              String queryString_BAD = String("?turn=toggle");
              EthernetClient client2;
              if (client2.connect(HOST_NAME_BAD, 80)) {
                // send HTTP header
                client2.println("GET " + PATH_NAME_BAD + queryString_BAD + " HTTP/1.1");
                client2.println("Host: " + String(HOST_NAME_BAD));
                client2.println("Connection: close");
                client2.println();  // end HTTP header
              }

              delay(5);
              client2.stop();
            }
            break;
          case 5:
            if (State == 11) {
              BADF1Soll = BADF1MaxSpeed;
              BADF1Count = AKTSek;
              BADF1WaitCount = AKTSek;
            }
            break;
          case 12:
            if (State == 1) {
            }
            if (State == 11) {
              dimmingb1 = 1;
              longpress = 1;
              if (directionb1 == 1) {
                directionb1 = 0;
              } else {
                directionb1 = 1;
              }
            }
            if (State == 10) {
              dimmingb1 = 0;
            }
            if (State == 0) {
              if (LEDBAD1Soll == 0 && longpress == 0) {
                LEDBAD1Soll = 100;
              } else if (LEDBAD1Soll >= 13 && longpress == 0) {
                LEDBAD1Soll = 0;
              }
              longpress = 0;
            }
            break;
          case 16:
            break;
        }
        break;
      //______________________________________________________________________________________________________________________
      case 57:
        switch (Button) {
          case 0:
            break;
          case 4:  // Büro Licht aus
            if (State == 1) {
              digitalWrite(Kinderzimmer, LOW);
            }
            break;
          case 5:  // Büro Licht an
            if (State == 1) {
              digitalWrite(Kinderzimmer, HIGH);
            }
            break;
          case 12:
            break;
          case 16:
            break;
        }
        break;
      //______________________________________________________________________________________________________________________
      case 58:
        switch (Button) {
          case 0:
            break;
          case 4:
            break;
          case 5:
            break;
          case 12:
            break;
          case 16:
            break;
        }
        break;
      //______________________________________________________________________________________________________________________
      case 59:
        switch (Button) {
          case 0:
            break;
          case 4:
            break;
          case 5:
            break;
          case 12:
            break;
          case 16:
            break;
        }
        break;
      //______________________________________________________________________________________________________________________
      case 60:
        switch (Button) {
          case 0:
            break;
          case 4:
            break;
          case 5:
            break;
          case 12:
            break;
          case 16:
            break;
        }
        break;
      //______________________________________________________________________________________________________________________
      case 61:
        switch (Button) {
          case 0:  // Büro Links auf
            break;
          case 4:  // Büro Links zu
            break;
          case 5:  // Büro Rechts zu
            break;
          case 12:  // Büro Rechts auf
            break;
          case 16:
            break;
        }
        break;
      //______________________________________________________________________________________________________________________
      case 62:
        switch (Button) {
          case 0:
            break;
          case 4:
            break;
          case 5:
            break;
          case 12:
            break;
          case 16:
            break;
        }
        break;
      //______________________________________________________________________________________________________________________
      case 63:
        switch (Button) {
          case 0:
            break;
          case 4:
            break;
          case 5:
            break;
          case 12:
            break;
          case 16:
            break;
        }
        break;
      //______________________________________________________________________________________________________________________
      case 64:
        switch (Button) {
          case 0:
            break;
          case 4:
            break;
          case 5:
            break;
          case 12:
            break;
          case 16:
            break;
        }
        break;
    }
  }

  //BAD/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (dimmingb1 == 1) {
    if (LEDBAD1Soll == LEDBAD1) {
      if (directionb1 == 0) {
        if (LEDBAD1Soll <= 12) {
          LEDBAD1Soll = 13;
        }
        if (LEDBAD1Soll >= 13 && LEDBAD1Soll <= 255) {
          LEDBAD1Soll++;
        }
        if (LEDBAD1Soll == 255)
          directionb1 = 1;
      }
      if (directionb1 == 1) {
        if (LEDBAD1Soll == 13) {
          directionb1 = 0;
        }
        if (LEDBAD1Soll >= 14 && LEDBAD1Soll <= 255) {
          LEDBAD1Soll--;
        }
      }
    }
  }
  if (dimmingb2 == 1) {
    if (LEDBAD2Soll == LEDBAD2) {
      if (directionb2 == 0) {
        if (LEDBAD2Soll <= 12) {
          LEDBAD2Soll = 13;
        }
        if (LEDBAD2Soll >= 13 && LEDBAD2Soll <= 255) {
          LEDBAD2Soll++;
        }
        if (LEDBAD2Soll == 255)
          directionb2 = 1;
      }
      if (directionb2 == 1) {
        if (LEDBAD2Soll == 13) {
          directionb2 = 0;
        }
        if (LEDBAD2Soll >= 14 && LEDBAD2Soll <= 255) {
          LEDBAD2Soll--;
        }
      }
    }
  }

  if (BADF1 > BADF1Soll) {
    BADF1--;
    delay(2);
  }
  if (BADF1 < BADF1Soll) {
    BADF1++;
    delay(2);
  }
  //Serial.print(LEDBAD1d);
  //Serial.print("  ");
  //Serial.println(LEDBAD1);

  if (LEDBAD1 > LEDBAD1Soll) {
    DIMMcount++;
    if (DIMMcount == DIMM) {
      DIMMcount = 0;
      LEDBAD1--;
    }
  }
  if (LEDBAD1 < LEDBAD1Soll) {
    DIMMcount++;
    if (DIMMcount == DIMM) {
      DIMMcount = 0;
      LEDBAD1++;
    }
  }
  if (LEDBAD2 > LEDBAD2Soll) {
    DIMMcount++;
    if (DIMMcount == DIMM) {
      DIMMcount = 0;
      LEDBAD2--;
    }
  }
  if (LEDBAD2 < LEDBAD2Soll) {
    DIMMcount++;
    if (DIMMcount == DIMM) {
      DIMMcount = 0;
      LEDBAD2++;
    }
  }


  if (BADF1Count + BADF1Time < AKTSek) {
    BADF1Count = 0;
    BADF1Soll = 0;
  }


  //Wohnzimmer/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /* if (State == 11) {
                dimmingess = 1;
                longpress = 1;
                if (directioness ==1){
                  directioness=0;
                } else {
                  directioness=1;
                }
              }
              if (State == 10) {
                dimmingess = 0;
              }
              if (State == 0) {
                if (WZLEDESSSoll == 0 && longpress == 0) {
                  WZLEDESSSoll = 60;
                }
                if (WZLEDESSSoll >= 49 && longpress == 0) {
                  WZLEDESSSoll = 0;
                }
                longpress = 0;
              }*/
  if (dimmingess == 1) {
    if (directioness == 0) {
      if (WZLEDESSSoll <= 49) {
        WZLEDESSSoll = 50;
      }
      if (WZLEDESSSoll >= 50 && WZLEDESSSoll <= 1022) {
        WZLEDESSSoll++;
      }
      if (WZLEDESSSoll == 1023)
        directioness = 1;
    }
    if (directioness == 1) {
      if (WZLEDESSSoll >= 51 && WZLEDESSSoll <= 1022) {
        WZLEDESSSoll--;
      }
      if (WZLEDESSSoll == 50)
        directioness = 0;
    }
  }
  if (WZLEDTVSoll < 49) {
    WZLEDTVSoll = 0;
    WZLEDTVa = 0;
  }

  if (WZLEDTV > WZLEDTVSoll) {
    WZLEDTV--;
    delay(2);
  }
  if (WZLEDTV < WZLEDTVSoll) {
    WZLEDTV++;
    delay(2);
  }

  if (WZLEDESSSoll < 49) {
    WZLEDESSSoll = 0;
    WZLEDESSa = 0;
  }

  if (WZLEDESS > WZLEDESSSoll) {
    WZLEDESS--;
    delay(2);
  }
  if (WZLEDESS < WZLEDESSSoll) {
    WZLEDESS++;
    delay(2);
  }


  //GästeWC///////////////////////////////////////////////////////////////////////////////////////////////////////////////////




  if (GWCSTATE > 10) {
    LEDGWCa = 1;
  } else {
    LEDGWCa = 0;
  }
  //Serial.println(LEDFLa);
  //Serial.println(LEDFLSoll);

  if (LEDGWCa == 1 && LEDGWCSoll < GWCLEDMAX) {
    if (LEDGWCSoll < 50) {
      LEDGWCSoll = 50;
    } else {
      LEDGWCSoll++;
    }
  }
  if (LEDGWCa == 0 && LEDGWCSoll > 0) {
    LEDGWCSoll--;
  }
  if (LEDGWC > LEDGWCSoll) {
    LEDGWC--;
    delay(1);
  }
  if (LEDGWC < LEDGWCSoll) {
    LEDGWC++;
    delay(1);
  }


  //Flur///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (FLSTATE > 10) {
    LEDFLa = 1;
  } else {
    LEDFLa = 0;
  }
  //Serial.println(LEDFLa);
  //Serial.println(LEDFLSoll);

  if (LEDFLa == 1 && LEDFLSoll < FLLEDMAX) {
    if (LEDFLSoll < 50) {
      LEDFLSoll = 50;
    } else {
      LEDFLSoll++;
    }
  }
  if (LEDFLa == 0 && LEDFLSoll > 0) {
    LEDFLSoll--;
  }
  if (LEDFL > LEDFLSoll) {
    LEDFL--;
    delay(1);
  }
  if (LEDFL < LEDFLSoll) {
    LEDFL++;
    delay(1);
  }
  //Schlafzimmer/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (TAST1STATE > 493 && TAST1STATE < 500) {
    SZTASTLO = 1;
  } else {
    SZTASTLO = 0;
  }

  if (TAST1STATE > 611 && TAST1STATE < 617) {
    SZTASTLU = 1;
  } else {
    SZTASTLU = 0;
  }

  if (TAST1STATE > 527 && TAST1STATE < 533) {
    SZTASTRO = 1;
  } else {
    SZTASTRO = 0;
  }

  if (TAST1STATE > 566 && TAST1STATE < 572) {
    SZTASTRU = 1;
  } else {
    SZTASTRU = 0;
  }

  if (SZTASTRO == 1) {
    SZM = 1;
  }

  if (SZTASTRU == 1) {
    SZM = 0;
  }

  if (SZM == 1) {
    digitalWrite(Schlafzimmer, HIGH);
  } else {
    digitalWrite(Schlafzimmer, LOW);
  }
  if (SZTASTLO == 1) {
    SZCOUNTaLED++;
    SZCOUNTbLED = 0;
  }

  if (SZCOUNTaLED > 20 && SZCOUNTaLED < 500 && SZTASTLO == 0 && LEDSZSoll < 1023) {
    LEDSZSoll = 1023;
  }
  if (SZTASTLU == 1) {
    SZCOUNTbLED++;
    SZCOUNTaLED = 0;
  }
  if (SZCOUNTaLED > 500 && SZTASTLO == 1 && LEDSZSoll < 1023) {
    LEDSZSoll++;
    delay(20);
  }

  if (SZCOUNTbLED > 20 && SZCOUNTbLED < 500 && SZTASTLU == 0 && LEDSZSoll > 0) {
    LEDSZSoll = 0;
    SZCOUNTbLED = 0;
  }
  if (SZCOUNTbLED > 500 && SZTASTLU == 1 && LEDSZSoll > 0) {
    LEDSZSoll--;
    delay(20);
  }
  //Ankleide////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (ANKLSTATE > 600) {
    TasterAK++;
  } else {
    TasterAK = 0;
  }

  if (TasterAK == 5 && LICHTAK == 0) {
    LICHTAK = 1;
    goto ANKL;
  }
  if (TasterAK == 5 && LICHTAK == 1) {
    LICHTAK = 0;
    goto ANKL;
  }
ANKL:
  if (LICHTAK > 0) {
    LEDAKSoll = 1023;
  } else {
    LEDAKSoll = 0;
  }

  if (LEDAKSoll > LEDAKIst) {
    if (LEDAKIst < 300) {
      LEDAKIst++;
    } else {
      LEDAKIst = LEDAKIst + 3;
    }

    delay(2);
  }
  if (LEDAKSoll < LEDAKIst) {
    if (LEDAKIst < 300) {
      LEDAKIst--;
    } else {
      LEDAKIst = LEDAKIst - 3;
    }
    delay(2);
  }

  if (ROLLOSZSTATE > 0) {
    ROLLORUNTIME++;
  }
  if (ROLLORUNTIME > 35000) {
    ROLLOSZSTATE = 0;
    ROLLORUNTIME = 0;
  }

  if (SZRSTATEAUF > 600 && ROLLOSZSTATE == 0) {
    ROLLOSZSTATE = 1;
    ROLLORUNTIME = 0;
    delay(250);
    goto next;
  }
  if (SZRSTATEZU > 600 && ROLLOSZSTATE == 0) {
    ROLLOSZSTATE = 2;
    ROLLORUNTIME = 0;
    delay(250);
    goto next;
  }
  if (SZRSTATEAUF > 600 && ROLLOSZSTATE == 1) {
    delay(200);
    ROLLOSZSTATE = 0;
    goto next;
  }
  if (SZRSTATEAUF > 600 && ROLLOSZSTATE == 2) {
    delay(200);
    ROLLOSZSTATE = 0;
    goto next;
  }

  if (SZRSTATEZU > 600 && ROLLOSZSTATE == 1) {
    delay(200);
    ROLLOSZSTATE = 0;
    goto next;
  }
  if (SZRSTATEZU > 600 && ROLLOSZSTATE == 2) {
    delay(200);
    ROLLOSZSTATE = 0;
    goto next;
  }
next:
  if (ROLLOSZSTATE == 1) {
    digitalWrite(AUF, HIGH);
    digitalWrite(ZU, LOW);
  }
  if (ROLLOSZSTATE == 2) {
    digitalWrite(ZU, HIGH);
    digitalWrite(AUF, LOW);
  }
  if (ROLLOSZSTATE == 0) {
    digitalWrite(AUF, LOW);
    digitalWrite(ZU, LOW);
  }





  if (LEDSZSoll > LEDSZIst) {
    LEDSZIst = LEDSZIst + 1;
    delay(10);
  }
  if (LEDSZSoll < LEDSZIst) {
    LEDSZIst = LEDSZIst - 1;
    delay(10);
  }

  OCR3A = LEDSZIst;
  OCR4A = LEDAKIst;
  OCR4B = WZLEDTV;
  OCR4C = LEDFL;
  OCR2B = LEDBAD1;   //D7
  OCR2A = LEDBAD2;   //D8
  OCR3C = WZLEDESS;  //D1
  OCR1B = BADF1;     //D10
  OCR0A = LEDGWC;    //D13
  ///__________________________________________________________________



  if (TuerSTATE > 100 && EckeSTATE < 100) {
    LEDPROG = 1;
  }
  if (EckeSTATE > 100 && TuerSTATE < 100) {
    LEDPROG = 2;
  }
  if (EckeSTATE < 100 && TuerSTATE < 100) {
    LEDPROG = 0;
  }

  if (LEDPROG == 1 && LEDSTATE == 0) {
    INTENS = 0;
    LedSend seq1[] = {
      { LED1, 2000 }, { LED1, 2000 }, { LED2, 1800 }, { LED2, 1800 },
      { LED7, 1600 }, { LED7, 1600 }, { LED3, 1400 }, { LED3, 1400 },
      { LED8, 1200 }, { LED8, 1200 }, { LED4, 1000 }, { LED4, 1000 },
      { LED9, 800 },  { LED9, 800 },  { LED5, 600 },  { LED5, 600 },
      { LED10, 400 }, { LED10, 400 }, { LED6, 200 },  { LED6, 200 }
    };
    sendLEDSequence(seq1, 20, SPEED, INTENS);
    LEDSTATE = 1;
  }
  if (LEDPROG == 2 && LEDSTATE == 0) {
    INTENS = 0;
    LedSend seq2[] = {
      { LED6, 2000 },  { LED6, 2000 },  { LED5, 1800 }, { LED5, 1800 },
      { LED4, 1600 },  { LED4, 1600 },  { LED3, 1400 }, { LED3, 1400 },
      { LED2, 1200 },  { LED2, 1200 },  { LED1, 1000 }, { LED1, 1000 },
      { LED7, 800 },   { LED7, 800 },   { LED8, 600 },  { LED8, 600 },
      { LED9, 400 },   { LED9, 400 },   { LED10, 200 }, { LED10, 200 }
    };
    sendLEDSequence(seq2, 20, SPEED, INTENS);
    LEDSTATE = 1;
  }

  if (LEDPROG == 0 && LEDSTATE == 1) {
    INTENS = 255;
    // Reihenfolge/Offsets 1:1 aus dem Original uebernommen. Der erste LED6-Offset (200 statt 2000)
    // weicht vom sonstigen Muster ab - vermutlich ein Tippfehler im Original, hier bewusst nicht "korrigiert".
    LedSend seqOff[] = {
      { LED6, 200 },   { LED6, 2000 },  { LED5, 1800 }, { LED5, 1800 },
      { LED4, 1600 },  { LED4, 1600 },  { LED3, 1400 }, { LED3, 1400 },
      { LED2, 1200 },  { LED2, 1200 },  { LED1, 1000 }, { LED1, 1000 },
      { LED7, 800 },   { LED7, 800 },   { LED8, 600 },  { LED8, 600 },
      { LED9, 400 },   { LED9, 400 },   { LED10, 200 }, { LED10, 200 }
    };
    sendLEDSequence(seqOff, 20, SPEED, INTENS);
    LEDSTATE = 0;
  }

  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
          //Serial.print(c);
        }

        //if HTTP request has ended
        if (c == '\n') {
          Serial.println(readString);  //print to serial monitor for debuging

          // Leichtgewichtiger Status-Endpunkt fuers Polling der Web-UI (Lampen AN/AUS + Dimmwerte),
          // damit nicht bei jedem Poll die komplette Seite neu aufgebaut werden muss.
          bool isStateRequest = readString.indexOf("GET /state") >= 0;

          client.println(F("HTTP/1.1 200 OK"));  //send new page
          if (isStateRequest) {
            client.println(F("Content-Type: text/plain"));
            client.println();
            client.print(F("sz="));
            client.print(SZM);
            client.print(F("&ak="));
            client.print(LICHTAK);
            client.print(F("&bad="));
            client.print(BadA);
            client.print(F("&bu="));
            client.print(digitalRead(Kinderzimmer) == LOW ? 1 : 0);
            client.print(F("&ku="));
            client.print(KLAMPa);
            client.print(F("&kl="));
            client.print(KLEDa);
            client.print(F("&we="));
            client.print(WZLAMPa);
            client.print(F("&wt="));
            client.print(TVLAMPa);
            client.print(F("&pump="));
            client.print(BADF1Soll > 0 ? 1 : 0);
            client.print(F("&szl="));
            client.print(LEDSZSoll);
            client.print(F("&wel="));
            client.print(WZLEDTVSoll);
            client.print(F("&wtl="));
            client.print(WZLEDESSSoll);
            client.print(F("&fl="));
            client.println(LEDFLSoll);
          } else {
          client.println(F("Content-Type: text/html"));
          client.println();
          client.println(F("<HTML>"));
          client.println(F("<HEAD>"));
          client.println(F("<meta charset='UTF-8'>"));
          client.println(F("<meta name='viewport' content='width=device-width, initial-scale=1'>"));
          client.println(F("<meta name='apple-mobile-web-app-capable' content='yes' />"));
          client.println(F("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />"));
          client.println(F("<TITLE>Lichtschalter</TITLE>"));
          client.println(F("<style>"));
          client.println(F(":root{--bg:#0f172a;--card:#1e293b;--text:#e6edf3;--muted:#94a3b8;--accent:#38bdf8;--on:#22c55e;--off:#475569;--stop:#f59e0b}"));
          client.println(F("*{box-sizing:border-box}"));
          client.println(F("body{margin:0;padding:16px 12px 40px;background:var(--bg);color:var(--text);font-family:-apple-system,Segoe UI,Roboto,Helvetica,Arial,sans-serif}"));
          client.println(F("h1{text-align:center;font-size:1.4rem;margin:4px 0 24px}"));
          client.println(F("section{max-width:960px;margin:0 auto 28px}"));
          client.println(F("section h2{font-size:.8rem;color:var(--muted);text-transform:uppercase;letter-spacing:.08em;margin:0 0 10px 4px}"));
          client.println(F(".grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(210px,1fr));gap:12px}"));
          client.println(F(".card{background:var(--card);border-radius:14px;padding:14px 16px;box-shadow:0 2px 6px rgba(0,0,0,.35)}"));
          client.println(F(".card h3{margin:0 0 12px;font-size:.95rem;font-weight:600}"));
          client.println(F(".btnrow,.btn3{display:flex;gap:8px}"));
          client.println(F(".btn{flex:1;text-align:center;padding:11px 0;border-radius:999px;text-decoration:none;font-weight:700;font-size:.8rem;color:#fff;letter-spacing:.02em;transition:opacity .2s}"));
          client.println(F(".btn.on{background:var(--on)}"));
          client.println(F(".btn.off{background:var(--off)}"));
          client.println(F(".btn.dim{opacity:.35}"));
          client.println(F(".btn.stop{background:var(--stop);color:#1a1a1a}"));
          client.println(F(".val{margin:0 0 6px;font-size:.8rem;color:var(--muted)}"));
          client.println(F(".val span{color:var(--text);font-weight:700}"));
          client.println(F("input[type=range]{width:100%;accent-color:var(--accent);height:26px}"));
          client.println(F("</style>"));
          client.println(F("</HEAD>"));
          client.println(F("<BODY>"));
          client.println(F("<H1>Lichtschalter</H1>"));

          client.println(F("<section><h2>Licht</h2><div class='grid'>"));

          client.print(F("<div class='card'><h3>Schlafzimmer</h3><div class='btnrow'><a class='btn on' data-k='sz' data-v='1' href='/?button1on'>AN</a><a class='btn off' data-k='sz' data-v='0' href='/?button1off'>AUS</a></div>"));
          client.print(F("<p class='val' style='margin-top:12px'>LED: <span id='sliderSZval'>"));
          client.print(LEDSZSoll);
          client.print(F("</span></p><input type='range' min='0' max='1023' class='slider' id='sliderSZ' value='"));
          client.print(LEDSZSoll);
          client.println(F("'></div>"));

          printToggleCard(client, F("Ankleide"), F("ak"), F("/?button2on"), F("AN"), F("/?button2off"), F("AUS"));
          printToggleCard(client, F("Badspiegel"), F("bad"), F("/?button3on"), F("AN"), F("/?button3off"), F("AUS"));
          printToggleCard(client, F("Buero"), F("bu"), F("/?button4on"), F("AN"), F("/?button4off"), F("AUS"));
          printToggleCard(client, F("Kueche"), F("ku"), F("/?button5on"), F("AN"), F("/?button5off"), F("AUS"));
          printToggleCard(client, F("KuecheLED"), F("kl"), F("/?button5LEDon"), F("AN"), F("/?button5LEDoff"), F("AUS"));
          printToggleCard(client, F("Wohnzimmer Esstisch"), F("we"), F("/?button6on"), F("AN"), F("/?button6off"), F("AUS"));
          printSliderCard(client, F("Wohnzimmer LED Esstisch"), F("sliderWZ"), F("sliderWZval"), WZLEDTVSoll);
          printToggleCard(client, F("Wohnzimmer TV"), F("wt"), F("/?button7on"), F("AN"), F("/?button7off"), F("AUS"));
          printSliderCard(client, F("Wohnzimmer LED TV"), F("sliderWZESS"), F("sliderWZESSval"), WZLEDESSSoll);
          printSliderCard(client, F("Flur"), F("sliderFL"), F("sliderFLval"), LEDFLSoll);

          client.println(F("</div></section>"));

          client.println(F("<section><h2>Rollos</h2><div class='grid'>"));
          printRolloCard(client, F("Rollo Schlafzimmer"), F("/?buttonhoch"), F("/?buttonstop"), F("/?buttonrunter"));
          printRolloCard(client, F("Rollo Bad rechts"), F("/?brbuttonhoch"), F("/?brbuttonstop"), F("/?brbuttonrunter"));
          printRolloCard(client, F("Rollo Bad links"), F("/?blbuttonhoch"), F("/?blbuttonstop"), F("/?blbuttonrunter"));
          printRolloCard(client, F("Rollo Buero rechts"), F("/?orbuttonhoch"), F("/?orbuttonstop"), F("/?orbuttonrunter"));
          printRolloCard(client, F("Rollo Buero links"), F("/?olbuttonhoch"), F("/?olbuttonstop"), F("/?olbuttonrunter"));
          printRolloCard(client, F("Rollo Kueche rechts"), F("/?krbuttonhoch"), F("/?krbuttonstop"), F("/?krbuttonrunter"));
          printRolloCard(client, F("Rollo Kueche links"), F("/?klbuttonhoch"), F("/?klbuttonstop"), F("/?klbuttonrunter"));
          printRolloCard(client, F("Rollo WZR"), F("/?WZRbuttonhoch"), F("/?WZRbuttonstop"), F("/?WZRbuttonrunter"));
          printRolloCard(client, F("Rollo WZL"), F("/?WZLbuttonhoch"), F("/?WZLbuttonstop"), F("/?WZLbuttonrunter"));
          client.println(F("</div></section>"));

          client.println(F("<section><h2>Sonstiges</h2><div class='grid'>"));
          printLinkCard3(client, F("LED Aussen"), F("/?LEDAS1"), F("Sequenz 1"), F("/?LEDAS2"), F("Sequenz 2"), F("/?LEDAAus"), F("AUS"));
          printToggleCard(client, F("Umwaelzpumpe"), F("pump"), F("/?Pumpean"), F("Start"), F("/?Pumpestop"), F("STOP"));
          client.println(F("</div></section>"));

          client.println(F("<script>"));
          client.println(F("function bindSlider(s,p,q){var sl=document.getElementById(s),sp=document.getElementById(p);"));
          client.println(F("sl.oninput=function(){sp.textContent=this.value;};"));
          client.println(F("sl.onchange=function(){fetch('/?'+q+'='+this.value+'&');};}"));
          client.println(F("bindSlider('sliderSZ','sliderSZval','valueSZ');"));
          client.println(F("bindSlider('sliderWZ','sliderWZval','valueWZ');"));
          client.println(F("bindSlider('sliderWZESS','sliderWZESSval','valueWZESS');"));
          client.println(F("bindSlider('sliderFL','sliderFLval','valueFL');"));
          client.println(F("var sliderKeys={sliderSZ:'szl',sliderWZ:'wel',sliderWZESS:'wtl',sliderFL:'fl'};"));
          client.println(F("function pollState(){fetch('/state').then(function(r){return r.text();}).then(function(t){"));
          client.println(F("var s={};t.trim().split('&').forEach(function(p){var kv=p.split('=');s[kv[0]]=kv[1];});"));
          client.println(F("document.querySelectorAll('[data-k]').forEach(function(el){var k=el.getAttribute('data-k');"));
          client.println(F("if(s[k]===undefined)return;el.classList.toggle('dim',el.getAttribute('data-v')!==s[k]);});"));
          client.println(F("Object.keys(sliderKeys).forEach(function(id){var key=sliderKeys[id];if(s[key]===undefined)return;"));
          client.println(F("var sl=document.getElementById(id),sp=document.getElementById(id+'val');"));
          client.println(F("if(sl&&document.activeElement!==sl)sl.value=s[key];if(sp)sp.textContent=s[key];});"));
          client.println(F("}).catch(function(){});}"));
          client.println(F("pollState();setInterval(pollState,2000);"));
          client.println(F("</script>"));

          client.println(F("</BODY>"));
          client.println(F("</HTML>"));
          }

          delay(1);
          //stopping client
          client.stop();
          //controls the Arduino if you press the buttons
          //________________________________________________________
          /*if (BADTASTLORO == 30  && BADF1WaitCount == 0) {
              BADF1Soll = BADF1MaxSpeed;
              BADF1Count = AKTSek;
              BADF1WaitCount = AKTSek;
            }
            if (BADF1Count + BADF1Time < AKTSek) {
              BADF1Count = 0;
              BADF1Soll = 0;
            }*/
          if (readString.indexOf("?Pumpean") > 0) {
            BADF1Soll = BADF1MaxSpeed;
            BADF1Count = AKTSek;
            BADF1WaitCount = AKTSek;
          }
          if (readString.indexOf("?Pumpestop") > 0) {
            BADF1Soll = 0;
            BADF1Count = 0;
            BADF1WaitCount = 0;
          }
          //________________________________________________________




          if (readString.indexOf("GET /?valueWZESS=") >= 0) {
            pos1 = readString.indexOf('=');
            pos2 = readString.indexOf('&');
            valueStringWZESS = readString.substring(pos1 + 1, pos2);
            WZLEDESSSoll = valueStringWZESS.toInt();
          }
          if (readString.indexOf("GET /?valueWZ=") >= 0) {
            pos1 = readString.indexOf('=');
            pos2 = readString.indexOf('&');
            valueStringWZ = readString.substring(pos1 + 1, pos2);
            WZLEDTVSoll = valueStringWZ.toInt();
          }
          if (readString.indexOf("GET /?valueFL=") >= 0) {
            pos1 = readString.indexOf('=');
            pos2 = readString.indexOf('&');
            valueStringFL = readString.substring(pos1 + 1, pos2);
            LEDFLSoll = valueStringFL.toInt();
            Serial.println(valueStringFL);
          }
          if (readString.indexOf("GET /?valueSZ=") >= 0) {
            pos1 = readString.indexOf('=');
            pos2 = readString.indexOf('&');
            valueStringSZ = readString.substring(pos1 + 1, pos2);
            //Serial.println(valueStringSZ);
            //Serial.println(valueStringWZ);
            //Serial.println(valueStringFL);
            LEDSZSoll = (valueStringSZ.toInt());
          }

          /*    ______________________________________________
              /*

                        client.println(F("<H3>Rollo Kueche rechts</H3>"));
                        client.println(F("<a href=\"/?krbuttonhoch\"\">AUF</a>"));
                        client.println(F("<a href=\"/?krbuttonstop\"\">STOP</a>"));
                        client.println(F("<a href=\"/?krbuttonrunter\"\">ZU</a><br />"));
                        client.println(F("<br />"));
                        client.println(F("<H3>Rollo Kueche linksr</H3>"));
                        client.println(F("<a href=\"/?klbuttonhoch\"\">AUF</a>"));
                        client.println(F("<a href=\"/?klbuttonstop\"\">STOP</a>"));
                        client.println(F("<a href=\"/?klbuttonrunter\"\">ZU</a><br />"));
                        client.println(F("<br />"));

          */
          if (readString.indexOf("?brbuttonhoch") > 0) {
            sendRelayCommand("RELAISN02n");
          }
          if (readString.indexOf("?brbuttonrunter") > 0) {
            sendRelayCommand("RELAISN01n");
          }
          if (readString.indexOf("?brbuttonstop") > 0) {
            sendRelayCommand("RELAISN00n");
          }
          if (readString.indexOf("?blbuttonhoch") > 0) {
            sendRelayCommand("RELAISN11n");
          }
          if (readString.indexOf("?blbuttonrunter") > 0) {
            sendRelayCommand("RELAISN12n");
          }
          if (readString.indexOf("?blbuttonstop") > 0) {
            sendRelayCommand("RELAISN10n");
          }

          if (readString.indexOf("?orbuttonhoch") > 0) {
            sendRelayCommand("RELAISN22n");
          }
          if (readString.indexOf("?orbuttonrunter") > 0) {
            sendRelayCommand("RELAISN21n");
          }
          if (readString.indexOf("?orbuttonstop") > 0) {
            sendRelayCommand("RELAISN20n");
          }
          if (readString.indexOf("?olbuttonhoch") > 0) {
            sendRelayCommand("RELAISN31n");
          }
          if (readString.indexOf("?olbuttonrunter") > 0) {
            sendRelayCommand("RELAISN32n");
          }
          if (readString.indexOf("?olbuttonstop") > 0) {
            sendRelayCommand("RELAISN30n");
          }
          if (readString.indexOf("?klbuttonhoch") > 0) {
            sendRelayCommand("RELAISN42n");
          }
          if (readString.indexOf("?klbuttonrunter") > 0) {
            sendRelayCommand("RELAISN41n");
          }
          if (readString.indexOf("?klbuttonstop") > 0) {
            sendRelayCommand("RELAISN40n");
          }

          if (readString.indexOf("?krbuttonhoch") > 0) {
            digitalWrite(23, LOW);
            delay(20);
            digitalWrite(22, HIGH);
            krdrive = 1;
          }
          if (readString.indexOf("?krbuttonrunter") > 0) {
            digitalWrite(22, LOW);
            delay(20);
            digitalWrite(23, HIGH);
            krdrive = 1;
          }
          if (readString.indexOf("?krbuttonstop") > 0) {
            digitalWrite(22, LOW);
            digitalWrite(23, LOW);
            krdrive = 0;
          }
          // ___________________________________________
          if (readString.indexOf("?WZLbuttonhoch") > 0) {
            digitalWrite(WZLZU, LOW);
            delay(20);
            digitalWrite(WZLAUF, HIGH);
            WZLdrive = 1;
          }
          if (readString.indexOf("?WZLbuttonrunter") > 0) {
            digitalWrite(WZLAUF, LOW);
            delay(20);
            digitalWrite(WZLZU, HIGH);
            WZLdrive = 1;
          }
          if (readString.indexOf("?WZLbuttonstop") > 0) {
            digitalWrite(WZLAUF, LOW);
            digitalWrite(WZLZU, LOW);
            WZLdrive = 0;
          }
          if (readString.indexOf("?WZRbuttonhoch") > 0) {
            digitalWrite(WZRZU, LOW);
            delay(20);
            digitalWrite(WZRAUF, HIGH);
            WZRdrive = 1;
          }
          if (readString.indexOf("?WZRbuttonrunter") > 0) {
            digitalWrite(WZRAUF, LOW);
            delay(20);
            digitalWrite(WZRZU, HIGH);
            WZRdrive = 1;
          }
          if (readString.indexOf("?WZRbuttonstop") > 0) {
            digitalWrite(WZRAUF, LOW);
            digitalWrite(WZRZU, LOW);
            WZRdrive = 0;
          }

          if (readString.indexOf("?buttonhoch") > 0) {
            ROLLOSZSTATE = 1;
          }
          if (readString.indexOf("?buttonrunter") > 0) {
            ROLLOSZSTATE = 2;
          }
          if (readString.indexOf("?buttonstop") > 0) {
            ROLLOSZSTATE = 0;
          }


          if (readString.indexOf("?button8on") > 0) {

            LEDSZSoll = 255;
          }

          if (readString.indexOf("?button875") > 0) {

            LEDSZSoll = 180;
          }

          if (readString.indexOf("?button850") > 0) {

            LEDSZSoll = 127;
          }

          if (readString.indexOf("?button825") > 0) {

            LEDSZSoll = 60;
          }

          if (readString.indexOf("?button8off") > 0) {
            LEDSZSoll = 0;
          }

          if (readString.indexOf("?button1on") > 0) {
            SZM = 1;
          }
          if (readString.indexOf("?button1off") > 0) {
            SZM = 0;
          }
          if (readString.indexOf("?button2on") > 0) {
            LICHTAK = 1;
          }
          if (readString.indexOf("?button2off") > 0) {
            LICHTAK = 0;
          }
          //BADE
          if (readString.indexOf("?button3on") > 0) {
            // digitalWrite(Bad, HIGH);
            BadA = 1;
            int HTTP_PORT = 80;
            String HTTP_METHOD = "GET";
            IPAddress HOST_NAME(192, 168, 2, 213);  // hostname of web server:
            String PATH_NAME = "/relay/0";
            String queryString = String("?turn=on");
            EthernetClient client;
            if (client.connect(HOST_NAME, 80)) {
              Serial.print("connected to ");
              Serial.println(client.remoteIP());
              // send HTTP header
              client.println("GET " + PATH_NAME + queryString + " HTTP/1.1");
              client.println("Host: " + String(HOST_NAME));
              client.println("Connection: close");
              client.println();  // end HTTP header
            } else {
              // if you didn't get a connection to the server:
              Serial.println("connection failed");
            }
            while (client.available()) {
              // read an incoming byte from the server and print them to serial monitor:
              char c = client.read();
              Serial.print(c);
            }

            if (!client.connected()) {
              // if the server's disconnected, stop the client:
              Serial.println("disconnected");
              client.stop();
            }
          }
          if (readString.indexOf("?button3off") > 0) {
            //digitalWrite(Bad, LOW);
            BadA = 0;
            int HTTP_PORT = 80;
            String HTTP_METHOD = "GET";
            IPAddress HOST_NAME(192, 168, 2, 213);  // hostname of web server:
            String PATH_NAME = "/relay/0";
            String queryString = String("?turn=off");
            EthernetClient client;
            if (client.connect(HOST_NAME, 80)) {
              Serial.print("connected to ");
              Serial.println(client.remoteIP());
              // send HTTP header
              client.println("GET " + PATH_NAME + queryString + " HTTP/1.1");
              client.println("Host: " + String(HOST_NAME));
              client.println("Connection: close");
              client.println();  // end HTTP header
            } else {
              // if you didn't get a connection to the server:
              Serial.println("connection failed");
            }
            while (client.available()) {
              // read an incoming byte from the server and print them to serial monitor:
              char c = client.read();
              Serial.print(c);
            }

            if (!client.connected()) {
              // if the server's disconnected, stop the client:
              Serial.println("disconnected");
              client.stop();
            }
          }

          if (readString.indexOf("?button4on") > 0) {
            digitalWrite(Kinderzimmer, LOW);
          }
          if (readString.indexOf("?button4off") > 0) {
            digitalWrite(Kinderzimmer, HIGH);
          }
          if (readString.indexOf("?button5on") > 0) {
            digitalWrite(Kueche, HIGH);
            KLAMPa = 1;
          }
          if (readString.indexOf("?button5off") > 0) {
            digitalWrite(Kueche, LOW);
            KLAMPa = 0;
          }
          if (readString.indexOf("?button5LEDon") > 0) {
            digitalWrite(KuecheLED, LOW);
            KLEDa = 1;
          }
          if (readString.indexOf("?button5LEDoff") > 0) {
            digitalWrite(KuecheLED, HIGH);
            KLEDa = 0;
          }
          if (readString.indexOf("?button6on") > 0) {
            digitalWrite(Wohnzimmer1, HIGH);
            WZLAMPa = 1;
          }
          if (readString.indexOf("?button6off") > 0) {
            digitalWrite(Wohnzimmer1, LOW);
            WZLAMPa = 0;
          }
          if (readString.indexOf("?button7on") > 0) {
            digitalWrite(Wohnzimmer2, HIGH);
            TVLAMPa = 1;
          }
          if (readString.indexOf("?button7off") > 0) {
            digitalWrite(Wohnzimmer2, LOW);
            TVLAMPa = 0;
          }


          //clearing string for next read
          readString = "";
        }
      }
    }
    if (!client.connected()) {
      client.stop();
    }
  }
}
