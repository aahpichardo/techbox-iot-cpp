#include <SoftwareSerial.h> //Para el ESP32 se necesita la libreria EspSoftwareSerial by Dirk Kaar, Peter Lerup
#include <WiFi.h>
SoftwareSerial mySerial(16, 17); // TX (amarillo), RX (verde) //Necesario para el escaner QR

//Pines de los relevadores
const byte relayOne = 25;
const byte relayTwo = 26;
const byte relayThree = 27;
const byte relayFour = 33;

//Conexión WIFI
#define WIFI_SSID "IZZI-AB02"
#define WIFI_PASSWORD "3C046117AB02"

void setup() {
  //Conexión WIFI
  

  //Inicia setup del scanner QR
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("Inicializando GM60...");
  //Fin setup QR

  //Inicia setup de relés
  pinMode(relayOne, OUTPUT);
  pinMode(relayTwo, OUTPUT);
  pinMode(relayThree, OUTPUT);
  pinMode(relayFour, OUTPUT);
  //Fin setup relés

}

// Variable de estado para rastrear si se ha leído un código QR recientemente
bool qrReadRecently = false;
String qrCode = "";  // Inicializa una cadena vacía para almacenar el código QR

void loop() {
  // Espera hasta que haya datos disponibles
  if (mySerial.available() > 0) {
    // Lee los datos hasta que se reciba un carácter de nueva línea
    while (mySerial.available()) {
      char input = mySerial.read();
      Serial.print(input);
      qrCode += input;  // Concatena los caracteres para formar el código QR
      delay(5);
    }
    
    Serial.println();
    
    // Verifica si el contenido del código QR no está vacío antes de activar el relé
      if(!qrCode.isEmpty()){
        if(qrCode == "Extension"){
          digitalWrite(relayOne, LOW);
          delay(3000);
          digitalWrite(relayOne, HIGH);
      }else if( qrCode == "Ethernet"){
        digitalWrite(relayTwo, LOW);
        delay(3000);
        digitalWrite(relayTwo, HIGH);
        Serial.println("Encendiendo relay dos");
      }else if(qrCode == "Adaptador"){
        digitalWrite(relayThree, LOW);
        delay(3000);
        digitalWrite(relayThree, HIGH);
        Serial.println("Encendiendo relay tres");
      }else if(qrCode == "HDMI"){
        digitalWrite(relayFour, LOW);
        delay(3000);
        digitalWrite(relayFour, HIGH);
        Serial.println("Encendiendo relay cuatro");
      }
      // Limpia qrCode para la próxima lectura
      qrCode = "";
    }
  } else {
    // Si no hay datos disponibles, asegúrate de que el relé esté activado
    digitalWrite(relayOne, HIGH);
    digitalWrite(relayTwo, HIGH);
    digitalWrite(relayThree, HIGH);
    digitalWrite(relayFour, HIGH);
  }

  // Tu código principal puede continuar aquí...
}//fin loop








