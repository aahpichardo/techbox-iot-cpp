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

//variables para el json
String qrCode = "";  // Inicializa una cadena vacía para almacenar el código QR, debe tener formato como este: Tipo:Prestamo,Matricula:1121120162,Articulo:Extension, Cantidad:1,Fecha:02-03-24
String qrMatricula = "";
String qrArticulo = "";
int qrCantidad = 0;
String qrDate = "";
//fin variables json

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

      /*//PRESTAMO
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
      }//end if saber si es prestamo o devolucion*/
      
      processQRCode(qrCode);

      qrCode = "";
    }else {
      // Si no hay datos disponibles, asegúrate de que el relé esté activado (osease, desactivado)
      digitalWrite(relayOne, HIGH);
      digitalWrite(relayTwo, HIGH);
      digitalWrite(relayThree, HIGH);
      digitalWrite(relayFour, HIGH);
    }//fin lectura qr

}//fin loop

void processQRCode(String qrCode){
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
        sendDataToFirebase(qrCode);
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
      }
}

//Funcion para enviar los datos a firebase
void sendDataToFirebase(String qrCode){
    // Firebase.ready() should be called repeatedly to handle authentication tasks.
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    // For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create_Edit_Parse.ino
    FirebaseJson json;

    //extraccion del qr a la matricula
    qrMatricula = extractValue(qrCode, "Matricula");
    qrArticulo = extractValue(qrCode, "Articulo");
    qrCantidad = extractValue(qrCode, "Cantidad").toInt();
    qrDate = extractValue(qrCode, "Fecha");

    // Agrega los datos al objeto JSON
    json.add("userRegistration", qrMatricula);
    json.add("amount", qrCantidad);
    json.add("item", qrArticulo);
    json.add("date", qrDate);

    //json.setJsonData(qrCode.c_str());

    // Genera un identificador único para el nuevo elemento y envía el objeto JSON a Firebase
    String path = "/loans/active";
    if (Firebase.pushJSON(fbdo, path, json))
    {
    Serial.println("JSON enviado exitosamente");
    // El ID único generado por Firebase se puede obtener con fbdo.pushName()
    Serial.println("ID único generado: " + fbdo.pushName());
    }else
    {
      Serial.println("Error al enviar JSON: " + fbdo.errorReason());
    }
  }else
  {
    Serial.println("Firebase no está listo");
  }//fin firebase ready
}//fin funcion sendDataToFirebase

//función utilizada en la otra funcion sendDataToFirebase para poder extrar campos del qr
String extractValue(String data, String field){
  int start = data.indexOf(field + ":") + field.length() + 1;
  int end = data.indexOf(",", start);
  if (end == -1) { // If there is no comma, then this is the last field.
      end = data.length();
  }

  if (start != -1 && end != -1) {
      String extracted = data.substring(start, end);
      extracted.trim(); // Remove leading and trailing whitespace
      return extracted;
  } else {
      return "";
  }
}//fin funcion extractValue





