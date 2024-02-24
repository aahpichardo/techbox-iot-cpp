#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "IZZI-AB02"
#define WIFI_PASSWORD "3C046117AB02"

// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyCTczQ8QXbdH0o0oRzz9ZMCLDvR1jOlGS0"

/* 3. Define the RTDB URL */
#define DATABASE_URL "pruebas-f0910-default-rtdb.firebaseio.com" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "techhboxinc@gmail.com"
#define USER_PASSWORD "prueba123"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

void setup()
{

  Serial.begin(9600);

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

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
  Firebase.reconnectNetwork(true);

  // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
  // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
  fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);


  Firebase.begin(&config, &auth);

  Firebase.setDoubleDigits(5);
}

void loop()
{
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
