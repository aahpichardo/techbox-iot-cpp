#include <SoftwareSerial.h> //Para el ESP32 se necesita la libreria EspSoftwareSerial by Dirk Kaar, Peter Lerup
#include <WiFi.h>
#include <FirebaseESP32.h>

SoftwareSerial mySerial(16, 17); // TX (amarillo), RX (verde) //Necesario para el escaner QR

//Pines de los relevadores
const byte relayOne = 25;
const byte relayTwo = 26;
const byte relayThree = 27;
const byte relayFour = 33;

//Conexión WIFI
#define WIFI_SSID "IZZI-AB02"
#define WIFI_PASSWORD "3C046117AB02"

//Configuración de Firebase
/* Definir credenciales de Firebase */
#define API_KEY "AIzaSyCTczQ8QXbdH0o0oRzz9ZMCLDvR1jOlGS0"
#define DATABASE_URL "pruebas-f0910-default-rtdb.firebaseio.com"
#define USER_EMAIL "techhboxinc@gmail.com"
#define USER_PASSWORD "prueba123"

/* Definir objeto de datos de Firebase */
FirebaseData fbdo;

/* Configurar autenticación y configuración de Firebase */
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

//Fin configuración de firebase

void setup() {
  //Inicia setup del scanner QR
  Serial.begin(115200);
  mySerial.begin(9600);
  Serial.println("Inicializando GM60...");
  //Fin setup QR

  //Conexión WIFI
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //FIN conexion WIFI

  //Configuración Firebase
  /* Configurar credenciales de Firebase */
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  

  /* Iniciar conexión con Firebase */
  Firebase.begin(&config, &auth);
  //Fin configuración firebase

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
    if (mySerial.available() > 0) { //para saber si se esta escaneando algo
      Serial.println("Puedes escanear");
      // Lee los datos hasta que se reciba un carácter de nueva línea
      while (mySerial.available()) { //para agregar lo que se escanea a una variable, ver si se puede optimizar
        char input = mySerial.read();
        Serial.print(input);
        qrCode += input;  // Concatena los caracteres para formar el código QR
        delay(5);
      }

      Serial.println();

      //PRESTAMO
      if(!qrCode.isEmpty() && qrCode.indexOf("Prestamo") != -1){
        Serial.println("Se esta realizando un prestamo");
        if(qrCode.indexOf("Extension") != -1){
          Serial.println("Encendiendo relay uno");
          digitalWrite(relayOne, LOW);
          delay(3000);
          digitalWrite(relayOne, HIGH);
        }else if(qrCode.indexOf("Ethernet") != -1){
          Serial.println("Encendiendo relay dos");
          digitalWrite(relayTwo, LOW);
          delay(3000);
          digitalWrite(relayTwo, HIGH);
        }else if(qrCode.indexOf("Adaptador") != -1 ){
          Serial.println("Encendiendo relay tres");
          digitalWrite(relayThree, LOW);
          delay(3000);
          digitalWrite(relayThree, HIGH);
        }else if(qrCode.indexOf("HDMI") != -1){
          Serial.println("Encendiendo relay cuatro");
          digitalWrite(relayFour, LOW);
          delay(3000);
          digitalWrite(relayFour, HIGH);
        }
          //DEVOLUCION
      }else if(!qrCode.isEmpty() && qrCode.indexOf("Devolucion") != -1){
        Serial.println("Se está realizando una devolución");
        if(qrCode.indexOf("Extension") != -1){
          Serial.println("Devolver Extension");
        }else if(qrCode.indexOf("Ethernet") != -1){
          Serial.println("Devolver Ethernet");
        }else if(qrCode.indexOf("Adaptador") != -1){
          Serial.println("Devolver adaptador");
        }else if(qrCode.indexOf("HDMI") != -1){
          Serial.println("Devolver HDMI");
        }

      }else{
        Serial.println("QR invalido");
      }//end if saber si es prestamo o devolucion
              
      qrCode = "";
    }else {
      // Si no hay datos disponibles, asegúrate de que el relé esté activado (osease, desactivado)
      digitalWrite(relayOne, HIGH);
      digitalWrite(relayTwo, HIGH);
      digitalWrite(relayThree, HIGH);
      digitalWrite(relayFour, HIGH);
    }//fin lectura qr

}//fin loop








