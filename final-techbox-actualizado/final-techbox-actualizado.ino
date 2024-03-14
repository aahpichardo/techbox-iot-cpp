#include <SoftwareSerial.h> //Para el ESP32 se necesita la libreria EspSoftwareSerial by Dirk Kaar, Peter Lerup
#include <WiFi.h>
#include <FirebaseESP32.h>

//Pines del Scaner (3.3v)
SoftwareSerial mySerial(16, 17); // TX (amarillo), RX (verde) //Necesario para el escaner QR

//Pines de los relevadores (5v)
const byte relayOne = 25;
const byte relayTwo = 26;
const byte relayThree = 27;
const byte relayFour = 33;

//Pines sensores infrarrojo (5v)
const byte irOne = 32;
const byte irTwo = 35;
const byte irThree = 34;
const byte irFour = 14;
const byte irFive = 12;

//Conexión WIFI
/*#define WIFI_SSID "IZZI-AB02"
#define WIFI_PASSWORD "3C046117AB02"*/

#define WIFI_SSID "Pixel"
#define WIFI_PASSWORD "asdfg123"

//Configuración de Firebase
/* Definir credenciales de Firebase BD DE PRUEBA */
/*#define API_KEY "AIzaSyCTczQ8QXbdH0o0oRzz9ZMCLDvR1jOlGS0"
#define DATABASE_URL "pruebas-f0910-default-rtdb.firebaseio.com"
#define USER_EMAIL "techhboxinc@gmail.com"
#define USER_PASSWORD "prueba123"*/

/* Definir credenciales de Firebase BD */
#define API_KEY "AIzaSyAXZdVllDQFU8N-tn2-5aPz1pc7npjZGPY"
#define DATABASE_URL "https://techbox-be7c3-default-rtdb.firebaseio.com/"
#define USER_EMAIL "techhboxinc@gmail.com"
#define USER_PASSWORD "prueba123"

/* Definir objeto de datos de Firebase */
FirebaseData fbdo;

/* Configurar autenticación y configuración de Firebase */
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

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

  //Setup IRojos
  pinMode(irOne, INPUT);
  pinMode(irTwo, INPUT);
  pinMode(irThree, INPUT);
  pinMode(irFour, INPUT);
  pinMode(irFive, INPUT);
  //fin setup IR

}//fin setup

//variables para el json
String qrCode = "";  // Inicializa una cadena vacía para almacenar el código QR, debe tener formato como este: Tipo:Prest,Soli:-Ns_Yl3OPtq2Nurm4BjO,Adpr:1,HDMI:1,Eth:1,Ext:1

//Variables para saber la cantidad de articulos que se prestarán o se devolverán
int amountAdaptador = 0;
int amountHDMI = 0;
int amountEthernet = 0;
int amountExtension = 0;
int i = 0; //esta es para los while para que se activen los motores n cantidad de veces para sacar las cosas.

//Variable para leer el IR
int irValue = 0;

//Variable para guardar el ID del pedido
String userId = "";
String orderId = "";

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
  if(!qrCode.isEmpty() && qrCode.indexOf("Prest") != -1){
    Serial.println("Se esta realizando un prestamo");
    amountExtension = extractValue(qrCode,"Ext").toInt();
    amountEthernet = extractValue(qrCode, "Eth").toInt();
    amountAdaptador = extractValue(qrCode, "Adpr").toInt();
    amountHDMI = extractValue(qrCode, "HDMI").toInt();

    if (amountExtension != 0){
      while(i < amountExtension){
        Serial.println("Veces que se ha encendido el relay uno: ");
        Serial.println(i);
        digitalWrite(relayOne, LOW);
        delay(3000);
        digitalWrite(relayOne, HIGH);
        i++;
      }
      Serial.println("Todos los items fueron entregados con éxito");
      amountExtension = 0;
      i=0;
    }

    if(amountEthernet != 0){
      while(i < amountEthernet){
        Serial.println("Veces que se ha encendido el relay dos: ");
        Serial.println(i);
        digitalWrite(relayTwo, LOW);
        delay(2000);
        digitalWrite(relayTwo, HIGH);
        i++;
      }
      amountEthernet = 0;
      i=0;
    }

    if(amountAdaptador != 0){
      while(i < amountEthernet){
        Serial.println("Veces que se ha encendido el relay tres: ");
        Serial.println(i);
        digitalWrite(relayThree, LOW);
        delay(2000);
        digitalWrite(relayThree, HIGH);
        i++;
      }
      amountAdaptador = 0;
      i=0;
    }

    if(amountHDMI != 0){
      while(i < amountEthernet){
        Serial.println("Veces que se ha encendido el relay cuatro: ");
        Serial.println(i);
        digitalWrite(relayFour, LOW);
        delay(2000);
        digitalWrite(relayFour, HIGH);
        i++;
      }
      amountHDMI = 0;
      i=0;
    }
      //DEVOLUCION
  }else if(!qrCode.isEmpty() && qrCode.indexOf("Devo") != -1){
    Serial.println("Se está realizando una devolución");
    amountExtension = extractValue(qrCode, "Ext").toInt();
    amountEthernet = extractValue(qrCode, "Eth").toInt();
    amountAdaptador = extractValue(qrCode, "Adpr").toInt();
    amountHDMI = extractValue(qrCode, "HDMI").toInt();

    if(amountExtension != 0){
      while(i < amountExtension){
        Serial.println("Coloca el item, se enciende led para indicar");
        irValue = digitalRead(irOne);
        delay(3000);
        if(irValue == LOW){
          Serial.println("Se devolvió el item");
          Serial.println(i);
          digitalWrite(relayOne, LOW);
          delay(3000);
          digitalWrite(relayOne, HIGH);
          i++;
        }else{
          Serial.println("No se devolvió nada, intentalo de nuevo");
        }
      }
      i = 0;
      amountExtension = 0;
      Serial.println("Se devolvió todo: " + String("i: ") + String(i) + " amountExtension: " + String(amountExtension));
    }

    if(amountEthernet != 0){
      while(i < amountEthernet){
        Serial.println("Coloca el item, se enciende led para indicar");
        irValue = digitalRead(irOne);
        delay(3000);
        if(irValue == LOW){
          Serial.println("Se devolvió el item");
          Serial.println(i);
          digitalWrite(relayTwo, LOW);
          delay(2000);
          digitalWrite(relayTwo, HIGH);
          i++;
        }else{
          Serial.println("No se devolvió nada, intentalo de nuevo");
        }
      }
      i = 0;
      amountEthernet = 0;
      Serial.println("Se devolvió todo: " + String("i: ") + String(i) + " amountEthernet: " + String(amountEthernet));
    }
  //fin devolucion
  }else{
    Serial.println("QR invalido");
  }//fin if general
}//fin process qr




/*void sendDataToFirebase(String qrCode){
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
}//fin funcion sendDataToFirebase*/

//sendDataToFirebase(orderId, userId, "Recogido");
//sendDataToFirebase(orderId, userId, "Devuelto")
void sendDataToFirebase(String orderId, String userId, String customStatus){
  // Comprueba si Firebase está listo
  if (Firebase.ready())
  {
    // Define la ruta a la orden
    String pathOrder = "/loans/orders/" + userId + "/" + orderId;

    // Crea un objeto JSON para actualizar el estado
    FirebaseJson json;
    json.add("status", customStatus); // Agrega el estado personalizado al objeto JSON

    // Actualiza el estado en Firebase
    if (Firebase.updateNode(fbdo, pathOrder, json)) {
      Serial.println("Estado de la orden actualizado exitosamente");
    } else {
      Serial.println("Error al actualizar el estado de la orden: " + fbdo.errorReason());
    }
  }else
  {
    Serial.println("Firebase no está listo");
  }
}


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





