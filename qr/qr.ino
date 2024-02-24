#include <SoftwareSerial.h> //Para el ESP32 se necesita la libreria EspSoftwareSerial by Dirk Kaar, Peter Lerup
SoftwareSerial mySerial(16, 17); // TX (amarillo), RX (verde)

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  Serial.println("Inicializando GM60...");
}

void loop() {
  if (mySerial.available()) {
    while (mySerial.available()) {
      char input = mySerial.read();
      Serial.print(input);
      delay(5);
    }
    Serial.println();
  }
}



