/*La configuración del sensor DHT, el cliente MQTT y la conexión WiFi 
permanecen sin cambios.
El pin 2 se configura como salida digital, y este pin se utilizará 
para controlar el ventilador.
En el bucle principal, se mide la temperatura y la humedad al 
igual que antes.
El control del ventilador se realiza utilizando el pin 12.
Si la temperatura es mayor a 29 grados Celsius, el pin 12 se 
configura en estado alto (5V),
lo que encenderá el ventilador. Si la temperatura es menor 
o igual a 29 grados, 
el pin 12 se configura en estado bajo (0V), 
apagando el ventilador.*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Definición del pin y el tipo de sensor DHT
#define DHT_PIN 14
#define DHT_TIPO DHT11

// Creación de un objeto DHT para el sensor de temperatura y humedad
DHT dht(DHT_PIN, DHT_TIPO);

// Creación de un objeto WiFiClient para la conexión WiFi
WiFiClient espClient;

// Creación de un objeto PubSubClient para la comunicación MQTT
PubSubClient client(espClient);

// Creación de un arreglo para almacenar el mensaje a publicar en MQTT
char msg[16];

// Función de configuración que se ejecuta una vez al inicio
void setup() {
  // Inicializa el sensor DHT
  dht.begin();
  
  // Configura la conexión WiFi y el cliente MQTT
  setup_wifi();
  client.setServer("broker.hivemq.com", 1883);

  // Configura la función de callback para manejar mensajes MQTT entrantes
  client.setCallback(callback);
}

// Función de bucle principal que se ejecuta repetidamente
void loop() {
  // Si no está conectado al servidor MQTT, intenta reconectar
  if (!client.connected()) {
    reconnect();
  }

  // Mantén la conexión MQTT activa
  client.loop();

  // Espera 2 segundos antes de leer los valores del sensor DHT
  delay(2000);
  int temperatura = dht.readTemperature();
  int humedad = dht.readHumidity();

  // Formatea la temperatura y la humedad como una cadena y almacénala en 'msg'
  snprintf(msg, 16, "%d,%d", temperatura, humedad);

  // Publica la cadena 'msg' en el canal MQTT "canal"
  client.publish("canal", msg);

  // Espera 1 segundo antes de repetir el ciclo
  delay(1000);

  // Control del ventilador basado en la temperatura
  if (temperatura > 29) {
    // Enciende el ventilador al suministrar 5V
    digitalWrite(12, HIGH);
  } else {
    // Apaga el ventilador
    digitalWrite(12, LOW);
  }
}

// Función para reconectar al servidor MQTT en caso de desconexión
void reconnect() {
  if (client.connect("ESP32")) {
    Serial.println(".......Conexión exitosa");
  }
}

// Función para configurar la conexión WiFi
void setup_wifi() {
  Serial.begin(115200);
  WiFi.begin("SSID DE TU RED", "CONTRASEÑA");

  // Espera hasta que el ESP32 se conecte a la red WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
}
