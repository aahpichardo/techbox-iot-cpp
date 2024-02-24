#include <SoftwareSerial.h> //Para el ESP32 se necesita la libreria EspSoftwareSerial by Dirk Kaar, Peter Lerup
SoftwareSerial mySerial(16, 17); // TX (amarillo), RX (verde)

const byte relayOne = 25;

void setup() {
  //Inicia setup del scanner QR
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("Inicializando GM60...");
  //Fin setup QR

  //Inicia setup de relés
  pinMode(relayOne, OUTPUT);

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
    if (!qrCode.isEmpty()) {
      // Desactiva el relé
      digitalWrite(relayOne, LOW); //LOW es para que se active
      qrReadRecently = true;  // Establece la variable de estado en true
      // Espera 3 segundos
      delay(3000);
      // Activa el relé
      digitalWrite(relayOne, HIGH); //HIGH es para que se apague

      // Limpia qrCode para la próxima lectura
      qrCode = "";
    }
  } else {
    // Si no hay datos disponibles, asegúrate de que el relé esté activado
    digitalWrite(relayOne, HIGH);
  }

  // Tu código principal puede continuar aquí...
}





