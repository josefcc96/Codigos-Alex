#include <EEPROM.h>
#include <SPI.h>
//#include <printf.h>
#include <SoftwareSerial.h>
#define tx1 3
#define rx1 4
#define rele 6

SoftwareSerial mySerial(rx1, tx1); // RX, TX

unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
unsigned long preMill = 0;
uint32_t millisTimer = 0;

const int nadmins = 2;
const int numeros = 2;
String num[numeros] = {"3213114520","3213114520"};
char  * personas[numeros] = {"Alex Cachon", "Alex Cachon2"};
char  *direcciones[numeros] = {"--Direccion Alex Cachon--", " --Direccion Alex Cachon2--"};
char  *admins[nadmins] = {"3213114520", "3213114520"};

String o;


void celular(int x) {
 Serial.println("Enviando Mensaje");
 mySerial.print("AT+CMGF = 1\r");  //Configura el modo texto para enviar o recibir mensajes
 delay(200);
 for (int i = 0; i < nadmins; i++) {
   if (admins[i] != "") {
     char buff[100];
     memset(buff, '\0', 100); // Inicializa el string
     Serial.print("Mensaje al: ");
     Serial.println(admins[i]);
     sprintf(buff, "AT+CMGS=\"+57%s\"\n", admins[i]);
     mySerial.print(buff);
     delay(200);
     memset(buff, '\0', 100); // Inicializa el string
     sprintf(buff, "Se esta presentando un robo en el local de %s", personas[x]);
     mySerial.println(buff);  // Texto del SMS
     memset(buff, '\0', 100); // Inicializa el string
     sprintf(buff, "Direccion %s", direcciones[x]);
     mySerial.println(buff);  // Texto del SMS
     delay(200);
     mySerial.println((char)26); //Comando de finalización ^Z
     delay(200);
     mySerial.println();
     delay(5000);  // Esperamos un tiempo para que envíe el SMS
     Serial.println("SMS enviado");
     Serial.print("Llamando al:");
     Serial.println(admins[i]);
     sprintf(buff,"ATD%s;\r", admins[i]);
     Serial.println(buff);
     mySerial.print(buff);
     while (true) {
       unsigned long currentMillis = millis();
       if (currentMillis - previousMillis >= 40000) {
         previousMillis = currentMillis;
         break;
       }
       if (currentMillis - preMill >= 1000) {
         digitalWrite(10, ~(digitalRead(10)));
         preMill = currentMillis;
       }
       while (mySerial.available()) {
         Serial.write(mySerial.read());
       }
     }
     Serial.println("Colgando");
     mySerial.println("ATH\r");
     delay(500);

     while (mySerial.available()) {
       Serial.write(mySerial.read());
     }
   }
 }
}


int enviarAT(String ATcommand, char* resp_correcta, unsigned int tiempo)
{
  int x = 0;
  bool correcto = 0;
  char respuesta[100];
  unsigned long anterior;
  memset(respuesta, '\0', 100); // Inicializa el string
  delay(100);
  while ( mySerial.available() > 0) mySerial.read(); // Limpia el buffer de entrada
  mySerial.println(ATcommand); // Envia el comando AT
  x = 0;
  anterior = millis();
  // Espera una respuesta
  do {
    // si hay datos el buffer de entrada del UART lee y comprueba la respuesta
    if (mySerial.available() != 0)
    {
      respuesta[x] = mySerial.read();
      x++;
      // Comprueba si la respuesta es correcta
      if (strstr(respuesta, resp_correcta) != 0)
      {
        correcto = 1;
      }
    }
  }
  // Espera hasta tener una respuesta
  while ((correcto == 0) && ((millis() - anterior) < tiempo));
  Serial.println(respuesta);
  return correcto;
}

void setup() {

  Serial.begin(57600);
  mySerial.begin(57600);
  mySerial.println("AT");
  mySerial.println("AT&F");
  mySerial.println("AT+IPR = 57600");

  pinMode(rele, OUTPUT);
  pinMode(9, OUTPUT);

  pinMode(10, OUTPUT);

  pinMode(11, OUTPUT);

  delay(2000);
  enviarAT("AT", "OK", 100);

  enviarAT("AT", "OK", 100);
  enviarAT("AT&F\r", "OK", 100);
  enviarAT("AT+IPR=57600\r", "OK", 100);
  enviarAT("AT+CLIP=1\r", "OK", 100);
  enviarAT("AT+CMGF=1\r", "OK", 200);
  enviarAT("AT+CNMI=2,2,0,0,0\r", "OK", 200);

}
//RING
//
//+CLIP: "3003973027",129,"",0,"",0
//
//NO CARRIER
//at
//OK
//at
//OK
//AT+CMGL="ALL"
//+CMGL: 1,"REC UNREAD","3003973027","","20/04/11,17:09:56-20"
// Ok

int nume = 0;
void loop() {
  while (mySerial.available()) {
    char f = mySerial.read();
    Serial.write(f);
    o += f;
    int in = o.indexOf("+CLIP: \"");
    if (in > 0 && !nume) {
      o = "";
      for (int i = 0; i < 10; i++) {
        char f = mySerial.read();
        Serial.write(f);
        o += f;
      }
      mySerial.readString();
      Serial.print("\n LLAMAN DESDE: ");
      Serial.println(o);
      for (int i = 0; i < numeros; i++) {
        if (num[i] == o ) {
          Serial.print("Esta Registrado es: ");
          Serial.print(personas[i]);
          Serial.print(" Direccion: ");
          Serial.println(direcciones[i]);
          Serial.println("Colgando");
          mySerial.println("ATH\r");
          mySerial.flush();
          celular(i);
        }
      }
      nume = 1;

    }
  }
  while (Serial.available()) {
    char f = Serial.read();
    mySerial.write(f);
    o += f;
  }

  // for (int i = 0; i < numeros; i++) {
  //    if (num[i] == ) {

}


//void botones() {
//  previousMillis = millis();
//  previousMillis2 = millis();
//  while (1) {
//    unsigned long currentMillis = millis();
//    if (currentMillis - previousMillis >= 20000 && digitalRead(b2) == LOW) {
//      celular();
//      break;
//    }
//    if (currentMillis - previousMillis2 >= 1000 && digitalRead(b2) == LOW) {
//      Serial.print(".");
//      previousMillis2 = millis();
//
//    }
//  }
//}
//
//
//
//
//
