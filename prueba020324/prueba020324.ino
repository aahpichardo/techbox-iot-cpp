#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <SoftwareSerial.h> //Para el ESP32 se necesita la libreria EspSoftwareSerial by Dirk Kaar, Peter Lerup

SoftwareSerial mySerial(16, 17); // TX (amarillo), RX (verde) //Necesario para el escaner QR

/* Definir credenciales de WiFi */
#define WIFI_SSID "IZZI-AB02"
#define WIFI_PASSWORD "3C046117AB02"

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

void setup()
{
  //Inicia setup del scanner QR
  Serial.begin(115200);
  mySerial.begin(9600);
  Serial.println("Inicializando GM60...");
  //Fin setup QR

  // Conexión Wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
  }
  Serial.println("Conectado a WiFi con IP: " + WiFi.localIP().toString());
  // Fin conexión wifi

  //Configuración Firebase
  /* Configurar credenciales de Firebase */
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  

  /* Iniciar conexión con Firebase */
  Firebase.begin(&config, &auth);
  //Fin configuración firebase
}

String qrCode = "";  // Inicializa una cadena vacía para almacenar el código QR, debe tener formato como este: Tipo:Prestamo,Matricula:1121120162,Articulo:Extension, Cantidad:1,Fecha:02-03-24
String qrMatricula = "";
String qrArticulo = "";
int qrCantidad = 0;
String qrDate = "";

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

    if(!qrCode.isEmpty() && qrCode.indexOf("Prestamo") != -1){
      Serial.println("Se esta realizando un prestamo");
      sendDataToFirebase(qrCode);
    }else{
      Serial.println("QR inválido");
    }//fin prestamo
  }//fin lectura qr
}//fin loop

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
    String pathActive = "/loans/active";
    String pathHistory = "/loans/history";

    // Enviar a /loans/active
    if (Firebase.pushJSON(fbdo, pathActive, json)) {
      Serial.println("JSON enviado exitosamente a /loans/active");
      Serial.println("ID único generado: " + fbdo.pushName());
    } else {
      Serial.println("Error al enviar JSON a /loans/active: " + fbdo.errorReason());
    }

    // Enviar a /loans/history
    if (Firebase.pushJSON(fbdo, pathHistory, json)) {
      Serial.println("JSON enviado exitosamente a /loans/history");
      Serial.println("ID único generado: " + fbdo.pushName());
    } else {
      Serial.println("Error al enviar JSON a /loans/history: " + fbdo.errorReason());
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
}
