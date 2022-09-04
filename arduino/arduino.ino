#include "DHT.h"
#define DHTPIN D6
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE); //Inicializa o objeto DHT

#include <ESP8266WiFi.h> //inclui biblioteca
const char* ssid     = "whalter03"; //nome rede
const char* password = "pinto0003"; //senha rede

#include <PubSubClient.h> //inclui a biblioteca para usar mqtt
const char* mqtt_server = "mqtt.eclipseprojects.io"; //O brocker que ira usar

WiFiClient espClient;
PubSubClient client(espClient); //executará o pub sub usando esta rede


int h, t; //para que sejam globais
void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  dht.begin(); //Inicia o objeto que tinha sido declarado, o DHT
  wifiConnect();
  client.setServer(mqtt_server, 1883);
}

void loop() {
    if (!client.connected()) {
       mqtt_reconnect();
    }

  // Wait a few seconds between measurements.
  delay(2000);
  dhtRead();
  mqtt_publish();
}

void dhtRead(){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.println("LEITURAS -->");
  Serial.println(F("Humidity: "));
  Serial.println(h);
  Serial.println(F("%  Temperature: "));
  Serial.println(t);
  Serial.println(F("°C "));

  
  //Serial.print(f);
  //Serial.print(F("°F  Heat index: "));
  //Serial.print(hic);
  //Serial.print(F("°C "));
  //Serial.print(hif);
  //Serial.println(F("°F"));
}

void wifiConnect(){
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA); //modo de opreação do wifi
  WiFi.begin(ssid, password); //inicializa o wifi, faz a conexão

  while (WiFi.status() != WL_CONNECTED) { //enquanto nao conectar, continua tentando
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqtt_reconnect(){
     client.connect("ProjetoIrrigacao"); //conecta usando este nickname
}

void mqtt_publish(){ //publicar a leitura
    // Once connected, publish an announcement...
    client.publish("Atual/Temp",  String(t).c_str(), true); //tópico que receberá a informação/a mensagem/
    Serial.print("publicando"); // c_srt para passar para char
    client.publish("Atual/Humid",  String(h).c_str(), true);
    // permanece a mensagem no servidor, por mais que eles não tenham sido postados e lidos sumuntanemanete
    // ... and resubscribe
}
