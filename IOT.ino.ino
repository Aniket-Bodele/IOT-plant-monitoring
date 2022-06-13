#define BLYNK_PRINT Serial
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
char auth[] = "Authentification code sent by blynk";       
char ssid[] = "Your WiFi SSID";                       
char pass[] = "WiFi Password";
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define DHTPIN D5          
DHT dht(DHTPIN, DHT22);
#define ONE_WIRE_BUS D7    
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define buzzer D4 
#define rainPin D6       
int rain = 0;
int lastRain = 0;
const int AirValue = 590;  
const int WaterValue = 300;  
const int SoilSensorPin = A0;   
int soilMoisture = 0;
int moisturepercent = 0;
int relay = D0;


#define pirPin D3
int pirValue;
int pinValue;

BLYNK_WRITE(V0)
{
  pinValue = param.asInt();
}


void setup()
{
  Serial.begin(115200);
  delay(100);
  Blynk.begin(auth, ssid, pass);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);   
  display.clearDisplay();                       
  pinMode(relay, OUTPUT);
  pinMode(buzzer, OUTPUT);
  sensors.begin(); // Dallas temperature
  dht.begin();
}

void getPirValue(void)        //Get PIR Data
{
  pirValue = digitalRead(pirPin);
  if (pirValue)
  {
    Serial.println("Motion detected");
    Blynk.notify("Motion detected in your farm") ;
  }
}
void loop() {
  Blynk.run();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  sensors.requestTemperatures();      //send the command to get temperature
  float temp = sensors.getTempCByIndex(0);      

  Serial.print("Soil Temperature: ");
  Serial.println(temp);
  Serial.print("Temperature: ");
  Serial.println(t);
  Serial.print("Humidity: ");
  Serial.println(h);

  Blynk.virtualWrite(V3, t);  //V3 is for Temperature
  Blynk.virtualWrite(V4, h);  //V4 is for Humidity
  Blynk.virtualWrite(V2, temp); //Dallas Temperature

  soilMoisture = analogRead(SoilSensorPin);  //put Sensor insert into soil
  Serial.println(MoistureValue);

  moisturepercent = map(MoistureValue, AirValue, WaterValue, 0, 100);

  Blynk.virtualWrite(V1, moisturepercent); //Soil Moisture sensor

  if (moisturepercent > 100)
  {
    Serial.println("100 %");
    delay(1500);
    display.clearDisplay();

    // display Soil temperature
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 5);
    display.print("RH of Soil: ");
    display.print("100");
    display.print(" %");

    // display Air temperature
    display.setCursor(0, 20);
    display.print("Soil Temp: ");
    display.print(temp);
    display.print(" ");
    display.cp437(true);
    display.write(167);
    display.print("C");

    // display relative humidity of Soil
    display.setCursor(0, 35);
    display.print("Air Temp: ");
    display.print(t);
    display.print(" ");
    display.cp437(true);
    display.write(167);
    display.print("C");

    // display relative humidity of Air
    display.setCursor(0, 50);
    display.print("RH of Air: ");
    display.print(h);
    display.print(" %");

    display.display();
    delay(1500);
  }
  else if (moisturepercent < 0)
  {
    Serial.println("0 %");
    delay(1500);
    display.clearDisplay();

    // display Soil temperature
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 5);
    display.print("RH of Soil: ");
    display.print("0");
    display.print(" %");

    // display Air temperature
    display.setCursor(0, 20);
    display.print("Soil Temp: ");
    display.print(temp);
    display.print(" ");
    display.cp437(true);
    display.write(167);
    display.print("C");

    // display relative humidity of Soil
    display.setCursor(0, 35);
    display.print("Air Temp: ");
    display.print(t);
    display.print(" ");
    display.cp437(true);
    display.write(167);
    display.print("C");

    // display relative humidity of Air
    display.setCursor(0, 50);
    display.print("RH of Air: ");
    display.print(h);
    display.print(" %");

    display.display();
    delay(1500);
  }
  else if (moisturepercent >= 0 && moisturepercent <= 100)
  {
    Serial.print(moisturepercent);
    Serial.println("%");
    delay(1500);
    display.clearDisplay();

    // display Soil temperature
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 5);
    display.print("RH of Soil: ");
    display.print(moisturepercent);
    display.print(" %");

    // display Air temperature
    display.setCursor(0, 20);
    display.print("Soil Temp: ");
    display.print(temp);
    display.print(" ");
    display.cp437(true);
    display.write(167);
    display.print("C");

    // display relative humidity of Soil
    display.setCursor(0, 35);
    display.print("Air Temp: ");
    display.print(t);
    display.print(" ");
    display.cp437(true);
    display.write(167);
    display.print("C");

    // display relative humidity of Air
    display.setCursor(0, 50);
    display.print("RH of Air: ");
    display.print(h);
    display.print(" %");

    display.display();
    delay(1500);
  }
  if (moisturepercent >= 0 && moisturepercent <= 30)
  {
    Serial.println("needs water, send notification");
    //send notification
    Blynk.notify("Plants need water... Pump is activated") ;
    digitalWrite(relay, LOW);
    digitalWrite(buzzer, HIGH);
    Serial.println("Motor is ON");
    WidgetLED PumpLed(V5);
    PumpLed.on();
    delay(1000);
  }
  else if (moisturepercent > 30 && moisturepercent <= 100)
  {
    Serial.println("Soil Moisture level looks good...");
    digitalWrite(relay, HIGH);
    digitalWrite(buzzer, LOW);
    Serial.println("Motor is OFF");
    WidgetLED PumpLed(V5);
    PumpLed.off();
    delay(1000);
  }

  rain = digitalRead(rainPin);
  Serial.println(rain);

  if (rain == 0 && lastRain == 0) {
    Serial.println("It's Raining outside!");
    Blynk.notify("It's Raining outside!") ;
    lastRain = 1;
    delay(1000);
    //send notification

  }
  else if (rain == 0 && lastRain == 1) {
    delay(1000);
  }
  else {
    Serial.println("No Rains...");
    lastRain = 0;
    delay(1000);
  }

  if (pinValue == HIGH)
  {
    getPirValue();
  }
  delay(100);
}
