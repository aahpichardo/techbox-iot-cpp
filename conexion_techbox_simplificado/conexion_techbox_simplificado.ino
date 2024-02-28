#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

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
  Serial.begin(115200);

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

void loop() {
  // Firebase.ready() should be called repeatedly to handle authentication tasks.
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    // For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create_Edit_Parse.ino
    FirebaseJson json;

    // Agrega los datos al objeto JSON
    json.add("amount", "23");
    json.add("category", "prueba1");
    json.add("name", "nuevo");

    // Genera un identificador único para el nuevo elemento
    String path = "/loans/" + Firebase.pushString(fbdo, "/loans", "");

    // Si el push fue exitoso, el identificador único estará en fbdo.pushName()
    if (fbdo.pushName())
    {
      path = "/loans/" + fbdo.pushName();
      // Envía el objeto JSON a Firebase
      if (Firebase.setJSON(fbdo, path, json))
      {
        Serial.println("JSON enviado exitosamente");
      }
      else
      {
        Serial.println("Error al enviar JSON: " + fbdo.errorReason());
      }
    }
    else
    {
      Serial.println("Error al generar identificador único: " + fbdo.errorReason());
    }

    count++;
  }

}
