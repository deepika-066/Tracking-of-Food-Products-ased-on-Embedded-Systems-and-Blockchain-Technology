#include<WiFi.h>
#include<PubSubClient.h>
#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
#include "DHT.h"

#define DHTPIN 5   
// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);
const char* ssid = "test2";
const char* password = "12345678";
const char* mqtt_server = "broker.mqtt-dashboard.com";
long lasMsg =0;
char msg[50];

int value=0;
String message="";
char val[50];
String top;

void connectwifi() {
  
   delay(10);
  Serial.print("CONNECTING TO");
  Serial.println(ssid);

  WiFi.begin(ssid,password);
  while (WiFi.status() !=WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP ADDRESS:");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length)
{
  top = topic;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message += '\0';
  Serial.println(message);
  Serial.println();
}

void reconnect() {
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("node");
    }
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqtt()
{
   client.setCallback(callback);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
 
}

String rcv;
int gas=2;
int acc= 32;
int gas1;
int acc1;
float t;
float h;
void setup() {
    connectwifi();
  client.setServer(mqtt_server,1883);
  
Serial.begin(9600);
 dht.begin();
lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  delay(2000);
  lcd.clear();
}

void loop() {
  mqtt();
  
   gas1=digitalRead(gas);
   acc1=analogRead(acc);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print(F("Humidity: "));
  Serial.println(h);
  Serial.print(F("%  Temperature: "));
  Serial.println(t);
  Serial.print("acc value: ");
  Serial.println(acc1);
  Serial.print("gas value: ");
  Serial.println(gas1);
  delay(2000);
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("T:");
      lcd.setCursor(2,0);
      lcd.print(t);
      lcd.setCursor(5,0);
      lcd.print("H:");
      lcd.setCursor(8,0);
      lcd.print(h);
      lcd.setCursor(0,1);
      lcd.print("g:");
      lcd.setCursor(2,1);
      lcd.print(gas1);
      lcd.setCursor(5,1);
      lcd.print("a:");
      lcd.setCursor(7,1);
      lcd.print(acc1);
      delay(1000);
 
  if(t>32&&h>52)
  {
    client.publish("FOOD","TEMP IS HIGH&FOOD WILL BE ROTTEN");
    Serial.println("FOOD WILL BE ROTTEN");
    client.publish("FOOD","13.009164,80.3646");
    Serial.println("13.009164,80.3646");
    delay(1000);
  }
  if(gas1==0)
  {
    client.publish("FOOD","GAS IS HIGH&FOOD WILL BE WASTED");
    Serial.println("FOOD WILL BE WASTED");
    client.publish("FOOD","13.009164,80.3646");
    Serial.println("13.009164,80.3646");
    delay(1000);
  }
  if(acc1>3590)
  {
    client.publish("FOOD","ACC IS HIGH&FOOD WILL BE DAMAGED");
    Serial.println("FOOD WILL BE DAMAGED");
    client.publish("FOOD","13.009164,80.3646");
    Serial.println("13.009164,80.3646");
    delay(1000);
  }

if(Serial.available())
{
  rcv=Serial.readStringUntil('\n');
  Serial.println(rcv);
  if(rcv=="14004B42D1CC")

  {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("BLOCK CHAIN");
      lcd.setCursor(0,1);
      lcd.print("STARTED");
      delay(2000);
    Serial.println("BLOCK CHAIN STARTED");
  }
  else if(rcv=="14004C1BA5E6")
  {
    lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("BLOCK CHAIN");
      lcd.setCursor(0,1);
      lcd.print("ENDED");
      delay(2000);
    Serial.println("BLOCK CHAIN ENDED");
  }
}
}
