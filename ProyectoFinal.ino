#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define WIFI_SSID "tu_nombre_de_red"
#define WIFI_PASS "tu_contraseña"
#define MQTT_SERVER "direccion_ip_o_dominio_de_tu_broker"
#define MQTT_PORT 1883
#define FAN_CHANNEL 0 // Puedes utilizar un valor de 0 a 15 para el canal PWM
#define DHTPIN 13        // Pin donde está conectado el sensor DHT11
#define DHTTYPE DHT11    // Tipo de sensor DHT (puede ser DHT11, DHT21 o DHT22)

#define FAN_PIN 12       // Pin donde está conectado el ventilador (D4 en el ESP32-CAM)
#define FAN_SPEED_LOW 7  // Velocidad baja del ventilador
#define FAN_SPEED_HIGH 15 // Velocidad alta del ventilador
#define TEMP_THRESHOLD 28.0 // Umbral de temperatura para activar el ventilador

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi!");

  client.setServer(MQTT_SERVER, MQTT_PORT);
  // Inicializa el pin del ventilador como salida
  pinMode(FAN_PIN, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (!isnan(temperature) && !isnan(humidity)) {
    char payload[100];
    snprintf(payload, sizeof(payload), "{\"temperature\": %.2f, \"humidity\": %.2f}", temperature, humidity);

    client.publish("sensor_data", payload);

    // Control del ventilador basado en la temperatura
    if (temperature > TEMP_THRESHOLD) {
      ledcWrite(FAN_PIN, FAN_SPEED_HIGH);
    } else {
      ledcWrite(FAN_PIN, FAN_SPEED_LOW);
    }
  }

  delay(1000); // Espera 5 segundos antes de leer nuevamente el sensor
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Conectando a MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado a MQTT!");
    } else {
      delay(1000);
    }
  }
}