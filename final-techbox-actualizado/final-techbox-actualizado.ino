#include <SoftwareSerial.h> //Para el ESP32 se necesita la libreria EspSoftwareSerial by Dirk Kaar, Peter Lerup
#include <WiFi.h>
#include <FirebaseESP32.h>

//Pines del Scaner (3.3v)
SoftwareSerial mySerial(16, 17); // TX (amarillo), RX (verde) //Necesario para el escaner QR

//Pines de los relevadores (5v)
const byte relayExtension = 25; //IN1
const byte relayEthernet = 26; //IN2
const byte relayAdaptador = 27; //IN3
const byte relayHDMI = 33; //IN4

//Pines sensores infrarrojo (5v)
const byte irExtension = 15; //no funciona el 12
const byte irEthernet = 35;
const byte irAdaptador = 34;
const byte irHDMI = 14;
const byte irEngine = 32;

//Pines LEDS (3.3v)
const byte ledVerde = 12;
const byte ledRojo = 13;

//Conexión WIFI
#define WIFI_SSID "IZZI-AB02"
#define WIFI_PASSWORD "3C046117AB02"

/*#define WIFI_SSID "Pixel"
#define WIFI_PASSWORD "asdfg123"*/

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
  pinMode(relayExtension, OUTPUT);
  pinMode(relayEthernet, OUTPUT);
  pinMode(relayAdaptador, OUTPUT);
  pinMode(relayHDMI, OUTPUT);
  //Fin setup relés

  //Setup IRojos
  pinMode(irExtension, INPUT);
  pinMode(irEthernet, INPUT);
  pinMode(irAdaptador, INPUT);
  pinMode(irHDMI, INPUT);
  pinMode(irEngine, INPUT);
  //fin setup IR

  //Setup leds
  pinMode(ledVerde, OUTPUT);
  pinMode(ledRojo,OUTPUT);

}//fin setup

//variables para el json
String qrCode = "";  // Inicializa una cadena vacía para almacenar el código QR, debe tener formato como este: Tipo:Prest,Soli:-Ns_Yl3OPtq2Nurm4BjO,Adpr:1,HDMI:1,Eth:1,Ext:1

//Variables para saber la cantidad de articulos que se prestarán o se devolverán
int amountAdaptador = 0;
int amountHDMI = 0;
int amountEthernet = 0;
int amountExtension = 0;
//int i = 0; //esta es para los while para que se activen los motores n cantidad de veces para sacar las cosas.

//Variable para leer el IR
int irValue = 0;

//Variable para guardar el ID del pedido
String userId = "";
String orderId = "";

void loop() {
    if (mySerial.available() > 0) { //para saber si se esta escaneando algo
      //blinkLed(ledVerde);
      //blinkLed(ledRojo);
      //digitalWrite(12, LOW);
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
      digitalWrite(relayExtension, HIGH);
      digitalWrite(relayEthernet, HIGH);
      digitalWrite(relayAdaptador, HIGH);
      digitalWrite(relayHDMI, HIGH);
      //digitalWrite(12, HIGH); // Apaga el LED verde
      //digitalWrite(13, HIGH); // Enciende el LED rojo
    }//fin lectura qr

}//fin loop

void processQRCode(String qrCode){
  //PRESTAMO
  if(!qrCode.isEmpty() && qrCode.indexOf("Pre") != -1){
    blinkLed(ledVerde);
    Serial.println("Se esta realizando un prestamo");
    amountExtension = extractValue(qrCode,"Ex").toInt();
    amountEthernet = extractValue(qrCode, "Et").toInt();
    amountAdaptador = extractValue(qrCode, "Ad").toInt();
    amountHDMI = extractValue(qrCode, "HD").toInt();
    userId = extractValue(qrCode, "Us");
    orderId = extractValue(qrCode, "So");

  /*if (amountExtension != 0 && i == 0){
    while(i < amountExtension){
        irValue = digitalRead(irEngine);
        delay(500);
        while(irValue != LOW && i != amountExtension){
            Serial.println("Veces que se ha encendido el relay uno: ");
            Serial.println(i);
            digitalWrite(relayExtension, LOW);
            delay(3000);
            i++;
            irValue = digitalRead(irEngine); // use = instead of ==
        }
        digitalWrite(relayExtension, HIGH);
    }
    Serial.println("Todos los items fueron entregados con éxito");
    amountExtension = 0;
    i=0;
  }*/

    moveItems(amountExtension, relayExtension, userId, orderId, "extension");
    amountExtension = 0;
    moveItems(amountEthernet, relayEthernet, userId, orderId, "ethernet");
    amountEthernet=0;
    moveItems(amountAdaptador, relayAdaptador, userId, orderId, "adaptador");
    amountAdaptador=0;
    moveItems(amountHDMI, relayHDMI, userId, orderId, "hdmi");
    amountHDMI=0;
    qrCode="";

    blinkLed(ledVerde);
      //DEVOLUCION
  }else if(!qrCode.isEmpty() && qrCode.indexOf("Dev") != -1){
    blinkLed(ledVerde);
    Serial.println("Se está realizando una devolución");
    amountExtension = extractValue(qrCode, "Ex").toInt();
    amountEthernet = extractValue(qrCode, "Et").toInt();
    amountAdaptador = extractValue(qrCode, "Ad").toInt();
    amountHDMI = extractValue(qrCode, "HD").toInt();
    userId = extractValue(qrCode, "Us");
    orderId = extractValue(qrCode, "So");

    /*if(amountExtension != 0){
      while(i < amountExtension){
        Serial.println("Coloca el item, se enciende led para indicar");
        irValue = digitalRead(irEngine);
        delay(3000);
        if(irValue == LOW){
          Serial.println("Se devolvió el item");
          Serial.println(i);
          digitalWrite(relayExtension, LOW);
          delay(3000);
          digitalWrite(relayExtension, HIGH);
          i++;
        }else{
          Serial.println("No se devolvió nada, intentalo de nuevo");
        }
      }
      i = 0;
      amountExtension = 0;
      Serial.println("Se devolvió todo: " + String("i: ") + String(i) + " amountExtension: " + String(amountExtension));
    }*/
    returnItems(amountExtension, relayExtension, irExtension, userId, orderId, "extension");
    amountExtension = 0;
    returnItems(amountEthernet, relayEthernet, irEthernet, userId, orderId, "ethernet");
    amountEthernet = 0;
    returnItems(amountAdaptador, relayAdaptador,irAdaptador, userId, orderId, "adaptador");
    amountAdaptador = 0;
    returnItems(amountHDMI, relayHDMI, irHDMI, userId, orderId, "hdmi");
    amountHDMI = 0;
    qrCode="";

    blinkLed(ledVerde);

  //fin devolucion
  }else{
    Serial.println("QR invalido");
    blinkLed(ledRojo);
  }//fin if general
}//fin process qr

//PARA PRESTAMO
void moveItems(int amount, byte relay, String userId, String orderId, String materialId) {
    int i = 0;
    if (amount != 0 && i == 0){
        while(i < amount){
            irValue = digitalRead(irEngine);
            delay(500);
            while(irValue != LOW && i != amount){
                Serial.println("Veces que se ha encendido el relay: ");
                Serial.println(i);
                digitalWrite(relay, LOW);
                delay(3000);
                i++;
                irValue = digitalRead(irEngine);
            }
            digitalWrite(relay, HIGH);
        }
        Serial.println("Todos los items fueron entregados con éxito");
        sendDataToFirebase(orderId, userId, "En uso");
        updateMaterialCount(materialId, amount, "subtract");
        amount = 0;
        i=0;
    }
}//fin moveitems

//PARA DEVOLUCION
void returnItems(int amount, byte relay, byte irSensor, String userId, String orderId, String materialId) {
    int i = 0;
    if(amount != 0){
        while(i < amount){
            Serial.println("Coloca el item, se enciende led para indicar");
            int irValue = digitalRead(irSensor);
            delay(3000);
            if(irValue == LOW){
                Serial.println("Se devolvió el item");
                Serial.println(i);
                //digitalWrite(relay, LOW);
                //delay(3000);
                //digitalWrite(relay, HIGH);
                i++;
            }else{
                Serial.println("No se devolvió nada, intentalo de nuevo");
            }
        }
        sendDataToFirebase(orderId, userId, "Devuelto");
        updateMaterialCount(materialId, amount, "add");
        i = 0;
        amount = 0;
        Serial.println("Se devolvió todo: " + String("i: ") + String(i) + " amount: " + String(amount));
    }
}//Fin return items

void blinkLed(byte led){
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
  delay(500);
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
  delay(500);
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
}

//sendDataToFirebase(orderId, userId, "Recogido");
//sendDataToFirebase(orderId, userId, "Devuelto")
void sendDataToFirebase(String orderId, String userId, String customStatus){
  // Comprueba si Firebase está listo
  if (Firebase.ready())
  {
    // Define la ruta a la orden
String pathOrder = "/loans/" + userId + "/orders/" + orderId;

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

void updateMaterialCount(String materialId, int amount, String operation){
  // Comprueba si Firebase está listo
  if (Firebase.ready())
  {
    // Define la ruta al material
    String pathMaterial = "/material/" + materialId + "/available";

    // Obtiene el valor actual del material
    if (Firebase.getInt(fbdo, pathMaterial)) {
      int currentAmount = fbdo.intData();

      // Actualiza el valor del material
      if (operation == "add")
      {
        currentAmount += amount;
      }
      else if (operation == "subtract")
      {
        currentAmount -= amount;
      }

      // Actualiza el valor en Firebase
      if (Firebase.set(fbdo, pathMaterial, currentAmount)) {
        Serial.println("Valor del material actualizado exitosamente");
      } else {
        Serial.println("Error al actualizar el valor del material: " + fbdo.errorReason());
      }
    } else {
      Serial.println("Error al obtener el valor del material: " + fbdo.errorReason());
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





