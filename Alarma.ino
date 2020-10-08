#include <EEPROM.h>
#include <SPI.h>
//#include <printf.h>
#include <SoftwareSerial.h>
#define tx1 3
#define rx1 4
#define rele 6
#include <avr/pgmspace.h>
#include "configs.h"
#include "numeros.h"
#include "personas.h"
#include "direcciones.h"
#include "admins.h"

SoftwareSerial mySerial(rx1, tx1); // RX, TX

unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
unsigned long preMill = 0;
uint32_t millisTimer = 0;




String o;
int nume = 0;
bool alarma = false;

char buffer[100];

void celular(int x) {
  Serial.println("Enviando Mensaje");
  mySerial.print("AT+CMGF = 1\r");  //Configura el modo texto para enviar o recibir mensajes
  Serial.flush();
  delay(200);
  for (int i = 0; i < nadmins; i++) {
    memset(buffer, '\0', 100);
    strcpy_P(buffer, (char *)pgm_read_word(&(admins[i])));
    if (buffer != "") {
      char buff[100];
      Serial.flush();
      memset(buff, '\0', 100); // Inicializa el string
      //  Serial.print("Mensaje al: ");
      //  Serial.println(admins[i]);
      sprintf(buff, "AT+CMGS=\"+57%s\"\n", buffer);
      mySerial.print(buff);
      delay(200);
      Serial.println(mySerial.readString());
      memset(buff, '\0', 100); // Inicializa el string
      memset(buffer, '\0', 100);
      strcpy_P(buffer, (char *)pgm_read_word(&(personas[x])));
      sprintf(buff, "Se esta presentando un robo en el local de %s", buffer);
      mySerial.println(buff);  // Texto del SMS
      delay(100);
      Serial.print(mySerial.readString());
      memset(buff, '\0', 100); // Inicializa el string
      memset(buffer, '\0', 100);
      strcpy_P(buffer, (char *)pgm_read_word(&(direcciones[x])));
      sprintf(buff, "Direccion %s", buffer);
      mySerial.println(buff);  // Texto del SMS
      delay(100);
      Serial.print(mySerial.readString());
      delay(200);
      mySerial.println((char)26); //Comando de finalización ^Z
      mySerial.println();
      delay(100);
      Serial.print(mySerial.readString());
      delay(200);
      int r = 0;
      String t;
      while (r <= 0) {
        if (mySerial.available()) {
          char u = mySerial.read();
          Serial.write(u);
          t += u;
          r = t.indexOf("+CMGS:");
        }
      };
      Serial.print(mySerial.readString());
      Serial.println("SMS enviado");
      Serial.print(mySerial.readString());
      Serial.print("Llamando al:");
      memset(buffer, '\0', 100);
      strcpy_P(buffer, (char *)pgm_read_word(&(admins[i])));
      Serial.println(buffer);
      memset(buff, '\0', 100); // Inicializa el string
      sprintf(buff, "ATD%s;\r", buffer);
      mySerial.println(buff);
      delay(100);
      Serial.print(mySerial.readString());
      delay(50);
      unsigned long currentMillis = millis();
      previousMillis = millis();
      int p = 1;
      while (currentMillis - previousMillis <= 40000) {
        currentMillis = millis();
        if (currentMillis - preMill >= 1000) {
          digitalWrite(10, p);
          p = !p;
          preMill = currentMillis;
        }
        while (mySerial.available()) {
          Serial.write(mySerial.read());
        }
      }
      digitalWrite(10, LOW);
      Serial.println("Colgando");
      mySerial.println("ATH\r");
      Serial.println(mySerial.readString());
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
  //  EEPROM.update(0,);
  Serial.begin(57600);
  mySerial.begin(57600);

  pinMode(rele, OUTPUT);
  pinMode(9, OUTPUT);

  pinMode(10, OUTPUT);

  pinMode(11, OUTPUT);

  delay(2000);
  enviarAT("AT", "OK", 100);
  enviarAT("AT&F\r", "OK", 100);
  enviarAT("AT+IPR=57600\r", "OK", 100);
  enviarAT("AT+CLIP=1\r", "OK", 100);
  enviarAT("AT+CMGF=1\r", "OK", 200);
  enviarAT("AT+CNMI=2,2,0,0,0\r", "OK", 200);

}

/* 
RING

+CLIP: "3003973027",129,"",0,"",0

NO CARRIER
at
OK
at
OK
AT+CMGL="ALL"
+CMGL: 1,"REC UNREAD","3003973027","","20/04/11,17:09:56-20"
Ok
*/
void loop() {
  o="";
  while (mySerial.available()) {
    char f = mySerial.read();
    Serial.write(f);
    o+=f;
    int in = o.indexOf("+CLIP: \"");
    if (in>0 ) {

      o="";
      for (int i = 0; i < 10; i++) {
        char f = mySerial.read();
        Serial.write(f);
        o+=f;
      }
      mySerial.readString();
      Serial.print("\n LLAMAN DESDE: ");
      Serial.println(o);
      for (int i = 0; i < numeros; i++) {
        memset(buffer, '\0', 100);
        strcpy_P(buffer, (char *)pgm_read_word(&(num[i])));
        Serial.println(buffer);
        if (o==buffer) {
          Serial.print("Esta Registrado es: ");
          memset(buffer, '\0', 100);
          strcpy_P(buffer, (char *)pgm_read_word(&(personas[i])));
          Serial.print(buffer);
          Serial.print(" Direccion: ");
          memset(buffer, '\0', 100);
          strcpy_P(buffer, (char *)pgm_read_word(&(direcciones[i])));
          Serial.println(buffer);
          Serial.println("Colgando");
          mySerial.println("ATH\r");
          delay(200);
          Serial.flush();
          previousMillis2 = millis();
          digitalWrite(rele, HIGH);
          digitalWrite(9, HIGH);
          celular(i);
          delay(500);
          alarma = true;
        }
      }
    }
  }
  o="";
  while (Serial.available()) {
    char f = Serial.read();
    mySerial.write(f);
    o+=f;
  }
  unsigned long currentMillis2 = millis();
  if (alarma == true) {
    if ((currentMillis2 - previousMillis2) >= aTiempo){
      digitalWrite(9, LOW);
      digitalWrite(rele, LOW);
      alarma = false;
    }
    else {
      Serial.print("ALARMA SONANDO:");
      Serial.println(currentMillis2 - previousMillis2);
    }
  }
  // for (int i = 0; i < numeros; i++) {
  //    if (num[i] == ) {

}
