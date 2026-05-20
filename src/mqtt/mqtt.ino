#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "raposoplay";
const char* password = "esnupicore";

const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

int relePin = 27;

void callback(char* topic, byte* message, unsigned int length) {
  String msg;

  for (int i = 0; i < length; i++) {
    msg += (char)message[i];
  }

  Serial.print("Mensagem recebida: ");
  Serial.println(msg);

  if (msg == "ON") {
    digitalWrite(relePin, LOW);  // ativa relé; nivel logico baixo devido ao uso do modulo de esp01
  } 
  else if (msg == "OFF") {
    digitalWrite(relePin, HIGH);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");

    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");

      client.subscribe("casa/rele");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(relePin, OUTPUT);
  digitalWrite(relePin, HIGH);

  WiFi.begin(ssid, password);
  randomSeed(micros());

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi conectado!");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}