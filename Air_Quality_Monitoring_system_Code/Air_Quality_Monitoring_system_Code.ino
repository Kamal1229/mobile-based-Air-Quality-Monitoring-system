// project Name:- "mobile-based Air Quality Monitoring system"
// By RAJKAMAL VISHWAKARMA 
#include <ThingSpeak.h>
#include <WiFi.h>
#include <MQUnifiedsensor.h>
//#include <MQSpaceData.h>

// WiFi credentials
char ssid[] = "Kamal";
char password[] = "AK1234567890";

// ThingSpeak
unsigned long Channel_ID = 2589128;
const char *write_api_key = "KEWWPCMMEE9J87PS";
WiFiClient client;

// MQ sensor configuration
#define placa "ESP-32"
#define Voltage_Resolution 5
#define ADC_Bit_Resolution 10

#define MQ135_PIN 34

MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, MQ135_PIN, "MQ-135");

void setup() {
  Serial.begin(115200);

  // WiFi Connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  ThingSpeak.begin(client);

  // Sensor initialization
  MQ135.setRegressionMethod(1);
  MQ135.init();
  float r0_135 = 0;
  for (int i = 0; i < 10; i++) {
    MQ135.update();
    r0_135 += MQ135.calibrate(3.6);
    delay(200);
  }
  MQ135.setR0(r0_135 / 10);
}

void loop() {
  MQ135.update();
  MQ135.setA(605.18); MQ135.setB(-3.937); // CO
  float CO = MQ135.readSensor();

  MQ135.setA(110.47); MQ135.setB(-2.862); // CO2
  float CO2 = MQ135.readSensor();

  MQ135.setA(700000000); MQ135.setB(-7.703); // LPG
  float LPG = MQ135.readSensor();

  Serial.print("CO: "); Serial.println(CO);
  Serial.print("CO2: "); Serial.println(CO2);
  Serial.print("LPG: "); Serial.println(LPG);

  ThingSpeak.setField(1, CO);
  ThingSpeak.setField(2, CO2 + 400); // adjusted baseline for atmospheric CO2
  ThingSpeak.setField(3, LPG);

  int x = ThingSpeak.writeFields(Channel_ID, write_api_key);
  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  delay(15000); 
}
