// vim: ts=4 sw=4 et ai
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <DallasTemperature.h>

/// #Aqui
#include <dht.h>

// https://www.hivemq.com/blog/mqtt-client-library-encyclopedia-arduino-pubsubclient/
// http://www.instructables.com/id/ESP8266-and-Multiple-Temperature-Sensors-DS18b20-W/
// https://www.geekstips.com/temperature-sensor-dht22-ds18b20-arduino-tutorial/

#define D0  16
#define D1  5  // I2C Bus SCL (clock)
#define D2  4  // I2C Bus SDA (data)
#define D3  0
#define D4  2  // Same as "LED_BUILTIN", but inverted logic
#define D5  14 // SPI Bus SCK (clock)
#define D6  12 // SPI Bus MISO 
#define D7  13 // SPI Bus MOSI
#define D8  15 // SPI Bus SS (CS)
#define D9  3  // RX0 (Serial console)
#define D10 1  // TX0 (Serial console)

//28a06c03050000cf HA_pt3052
//283816f7040000db HA_juniperEx
//28382d040500007d CA_ptt
//288aa3f6040000a0 CA_nb10
//287acdf604000061 CA_nb08
//284b200405000050 HA_Coloc
//28973304050000db CA_Coloc
//288f9a0305000067 CA_Brocade


// MAC - 68:C6:3A:8A:64:D8
//const char* ssid = "DCC-devices"; //replace this with your WiFi network name
const char* ssid = "SERGIOFAMILIA"; //replace this with your WiFi network name
const char* password = "anastacio123"; //replace this with your WiFi network password
const char* host = "esp8266-webupdate";

// MQTT
const char* mqttServer   = "192.168.1.8";
const int   mqttPort     = 1883;
const char* mqttUser     = "samuel";
const char* mqttPassword = "samuel";

// ONE WIRE
// Data wire is plugged into pin D3
#define ONE_WIRE_BUS D2
#define ONE_WIRE_MAX_DEV 15 //The maximum number of devices
#define AMOUNT_DHT11 1

///#Aqui

dht dht11Sensors[AMOUNT_DHT11];
int dht11Temp[AMOUNT_DHT11], dht11Hum[AMOUNT_DHT11];
int dhtPins[AMOUNT_DHT11];

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature DS18B20(&oneWire);

int numberOfDevices; //Number of temperature devices found
DeviceAddress devAddr[ONE_WIRE_MAX_DEV];  //An array device temperature sensors
float tempDev[ONE_WIRE_MAX_DEV]; //Saving the last measurement of temperature
float tempDevLast[ONE_WIRE_MAX_DEV]; //Previous temperature measurement
long lastTemp; //The last measurement
const int durationTemp = 10000; //The frequency of temperature measurement

WiFiClient espClient;
PubSubClient mqttClient(espClient);

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void reconnectMQTT() {
    // Loop until we're reconnected
    while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqttClient.connect("ESP8266Client", mqttUser, mqttPassword )) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            mqttClient.publish("esp/stats","reconnected!");
            // ... and resubscribe
            mqttClient.subscribe("esp/cmd");
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
	String json = "";
    for (int i = 0; i < length; i++) {
		json += (char)payload[i];
        Serial.print((char)payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");


	//Serial.println("FIM Message");
	//StaticJsonBuffer<200> jsonBuffer;
	//JsonObject& root = jsonBuffer.parseObject(json);
	//if(root.success()) Serial.println("Success"); else Serial.println("Failed");
	//if(root.containsKey("red"))  { color_r = root["red"]; Serial.println("tem red"); }
	//if(root.containsKey("blue")) { color_b = root["blue"]; Serial.println("tem blue"); }
	//if(root.containsKey("green")) { color_g = root["green"]; Serial.println("tem green"); }


}

void setupWifi() {
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.hostname("NodeMCU-CRC");
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WIFI connection Failed! Rebooting...");
        //digitalWrite(D4, LOW); delay(500); digitalWrite(D4, HIGH); delay(500); Serial.print(".");
        delay(5000);
        ESP.restart();
    }
    Serial.print("IP Address is: ");
    Serial.println(WiFi.localIP());
}

void setupMQTT() {
    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(callback);
    while (!mqttClient.connected()) {
        Serial.println("Connecting to MQTT...");
        if (mqttClient.connect("ESP8266Client", mqttUser, mqttPassword )) {
            Serial.println("connected");  
        } else {
            Serial.print("failed with state ");
            Serial.println(mqttClient.state());
            delay(2000);
        }
    }
    mqttClient.publish("esp/stats", "Hello from ESP8266");
    mqttClient.subscribe("esp/cmd");
}

void setup() 
{
    pinMode(D4, OUTPUT);

    dhtPins[0] = D0;
    
    Serial.begin(9600);
    Serial.println("Booting");
    delay(1000);

    // ---------------- WIFI Configuration -----------------------
  	setupWifi();

    // ---------------- HTTP Configuration -----------------------
    httpUpdater.setup(&httpServer);
    httpServer.on("/", HandleRoot);
    httpServer.onNotFound( HandleNotFound );
    httpServer.begin();
    Serial.println("HTTP server started at ip " + WiFi.localIP().toString() );

    // ---------------- MQTT Configuration -----------------------
	  setupMQTT();

    // ---------------- DS18b20 Configuration -----------------------
  	DS18B20.begin();
  	SetupDS18B20();
}

void loop() 
{
	int d = random(20,500);
	digitalWrite(D4, LOW); delay(d); digitalWrite(D4, HIGH); delay(d);

	httpServer.handleClient();

	if(!mqttClient.connected()) {
		reconnectMQTT();
	}
	mqttClient.loop();

	long t = millis();
	TempLoop(t);

	//char data[200];
	//String payload = "{ \"idx\": 2, \"nvalue\": 0, \"svalue\": \""+String(d)+"\" }";
	//payload.toCharArray(data, (payload.length() + 1));
	//mqttClient.publish("stats/xyz",data);
}

//------------------------------------------
//Convert device id to String
String GetAddressToString(DeviceAddress deviceAddress){
	String str = "";
	for (uint8_t i = 0; i < 8; i++){
		if( deviceAddress[i] < 16 ) str += String(0, HEX);
		str += String(deviceAddress[i], HEX);
	}
	return str;
}

//Setting the temperature sensor
void SetupDS18B20(){
	numberOfDevices = DS18B20.getDeviceCount();
	Serial.print( "Device count: " );
	Serial.println( numberOfDevices );

	lastTemp = millis();
	DS18B20.requestTemperatures();

	// Loop through each device, print out address
	for(int i=0;i<numberOfDevices; i++){
		// Search the wire for address
		if( DS18B20.getAddress(devAddr[i], i) ){
			Serial.print("Found device ");
			Serial.print(i, DEC);
			Serial.print(" with address: " + GetAddressToString(devAddr[i]));
			Serial.println();
		}else{
			Serial.print("Found ghost device at ");
			Serial.print(i, DEC);
			Serial.print(" but could not detect address. Check power and cabling");
		}

		//Get resolution of DS18b20
		Serial.print("Resolution: ");
		Serial.print(DS18B20.getResolution( devAddr[i] ));
		Serial.println();

		//Read temperature from DS18b20
		float tempC = DS18B20.getTempC( devAddr[i] );
		Serial.print("Temp C: ");
		Serial.println(tempC);
	}
}

//Loop measuring the temperature
void TempLoop(long now){
	int NoD = DS18B20.getDeviceCount();
	if(NoD != numberOfDevices) {
		Serial.println("Number of devices changed. Recalculating...");
		SetupDS18B20();
	}
	if( now - lastTemp > durationTemp ){ //Take a measurement at a fixed time (durationTemp = 5000ms, 5s)
		for(int i=0; i<numberOfDevices; i++){
			float tempC = DS18B20.getTempC( devAddr[i] ); //Measuring temperature in Celsius
			tempDev[i] = tempC; //Save the measured value to the array
		}
		DS18B20.setWaitForConversion(false); //No waiting for measurement
		DS18B20.requestTemperatures(); //Initiate the temperature measurement
		lastTemp = millis();  //Remember the last time measurement

    /// #Aqui
    for(int i = 0; i < AMOUNT_DHT11; i++) {
      dht11Sensors[i].read11(dhtPins[i]);
    
      dht11Temp[i] = dht11Sensors[i].temperature;
      dht11Hum[i]  = dht11Sensors[i].humidity;
    }

		publishSensors();
	}
}

void publishSensors() {
	char temperatureString[6];
	char message[200], topicArray[200], humidityString[200];
	for(int i=0;i<numberOfDevices;i++){

		StaticJsonBuffer<200> jsonBuffer;
		JsonObject& root = jsonBuffer.createObject();
		dtostrf(tempDev[i], 2, 2, temperatureString);
		root["temperature"] = temperatureString;
		root["addr"] = GetAddressToString( devAddr[i] );
		root.printTo(message,sizeof(message));

		mqttClient.publish("stats/sensor", message);
	}

    //#Aqui
    for(int i = 0; i < AMOUNT_DHT11; i++) {

      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.createObject();

      dtostrf(dht11Temp[i], 2, 2, temperatureString);
      dtostrf(dht11Hum[i], 2, 2, humidityString);

      root["temperature"] = temperatureString;
      root["humidity"] = humidityString;
      root.printTo(message,sizeof(message));
      mqttClient.publish("stats/sensor", message);
    }
}

//------------------------------------------
void HandleRoot(){
	String message = "<html><head><meta http-equiv='refresh' content='15'><title>ESP</title></head></body>";
  message += "Number of DS18B20 devices: ";
	message += numberOfDevices;
	message += "\r\n<br>";

  //#Aqui
  message += "Number of DHT11 devices: ";
  message += AMOUNT_DHT11;
  message += "\r\n<br><br>";
	char temperatureString[6], humidityString[6];

	message += "<table border='1'>\r\n";
	message += "<tr><td>Device DS18B20 id</td><td>Temperature</td></tr>\r\n";
	for(int i=0;i<numberOfDevices;i++){
		dtostrf(tempDev[i], 2, 2, temperatureString);
		Serial.print( "Sending DS18B20 temperature: " );
		Serial.println( temperatureString );
    Serial.println();

		message += "<tr><td><pre>";
		message += GetAddressToString( devAddr[i] );
		message += "</pre></td>\r\n";
		message += "<td><pre>";
		message += temperatureString;
		message += "</pre></td></tr>\r\n";
		message += "\r\n";
	}
	message += "</table></body></html>\r\n";

  //#Aqui
  message += "<table border='1'>\r\n";
  message += "<tr><td>Humidade</td><td>Temperature</td></tr>\r\n";
  for(int i=0;i<numberOfDevices;i++){
    dtostrf(dht11Temp[i], 2, 2, temperatureString);
    dtostrf(dht11Hum[i], 2, 2, humidityString);
    Serial.print( "Sending DHT11 temperature: " );
    Serial.println( temperatureString );
    Serial.print( "Sending DHT11 humidity: " );
    Serial.println( humidityString );
    Serial.println();

    message += "<tr><td><pre>";
    message += humidityString;
    message += "</pre></td>\r\n";
    message += "<td><pre>";
    message += temperatureString;
    message += "</pre></td></tr>\r\n";
    message += "\r\n";
  }
  message += "</table></body></html>\r\n";

	httpServer.send(200, "text/html", message );
}
   
void HandleNotFound(){
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += httpServer.uri();
	message += "\nMethod: ";
	message += (httpServer.method() == HTTP_GET)?"GET":"POST";
	message += "\nArguments: ";
	message += httpServer.args();
	message += "\n";
	for (uint8_t i=0; i<httpServer.args(); i++){
	  message += " " + httpServer.argName(i) + ": " + httpServer.arg(i) + "\n";
	}
	httpServer.send(404, "text/html", message);
}
