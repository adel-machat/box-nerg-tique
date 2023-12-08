#include "DHT.h"
#define DHTPIN 33
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

unsigned long duration = 0;

#include "EmonLib.h"
EnergyMonitor emon1;
int tension=220;


#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>

#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define API_KEY "AIzaSyA0FYxU3z1BsFNyrEHZwvflt6S9xQrqMro"
#define DATABASE_URL "https://connect-fe748-default-rtdb.firebaseio.com/" 
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;

#define wifi_ssid "Galaxy A30sF0DE"
#define wifi_password "1231231234"

#include <PubSubClient.h>
#define mqtt_server "192.168.69.68"
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
int value = 0;

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

   char strbuf[50];
   char strbuf_2[50];

   
void setup(){
  emon1.current(36,11);
  lcd.begin(16, 2); 
  pinMode(4,OUTPUT);
  lcd.init(); //  
  lcd.backlight();
  afiichage();
    
  pinMode(DHTPIN, INPUT);
  dht.begin();
  Serial.begin(9600);
  setup_wifi();
 
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")){
    signupOK = true;
  }
 
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

   client.setServer(mqtt_server, 1883);
   client.setCallback(callback);
}


void setup_wifi() {
  
  delay(10);
  // We start by connecting to a WiFi network
 

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("connecting to :");
    lcd.setCursor(2, 1);
    lcd.print(wifi_ssid);





  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  
    Serial.print(".");
  }
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("IP adresse :");
    lcd.setCursor(2, 1);
    lcd.print(WiFi.localIP());
 

}


void callback(char* topic, byte* message, unsigned int length) {

  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
 
}


 void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266client")) {
      client.subscribe("esp8266/dht11");
   
    }

  }
}

void afiichage(){
    String word = "hello every one ";
  int wordLength = word.length();
  
  for (int i = 0; i <= 16 - wordLength; i++) {
    lcd.clear();
    lcd.setCursor(i, 0);
    lcd.print(word);
     lcd.setCursor(i, 1);
    lcd.print("ANd");
    delay(500);
  }
  for (int i = 16 - wordLength; i >= 0; i--) {
    lcd.clear();
    lcd.setCursor(i, 0);
    lcd.print(word);
    lcd.setCursor(i, 1);
    lcd.print(" AND ");
    delay(500);
  }
}

/*void temp (){
float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    
  if(temperature>20){
    digitalWrite(4,1);
    delay(5000); 
    digitalWrite(4,0);
    delay(5000);
    
 }
    
   
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
    char humString[8];
    dtostrf(humidity, 1, 2, humString);
    sprintf(strbuf, "{\"temperature\": %s}", tempString);
     sprintf(strbuf_2, "{\"humidity\": %s}",  humString);
    client.publish("esp32/temp", strbuf);
    client.publish("esp32/hum",strbuf_2);
  

  if (Firebase.ready() && signupOK ) {
    
    Firebase.RTDB.setFloat(&fbdo, "DHT/humidity",humidity);
    Firebase.RTDB.setFloat(&fbdo, "DHT/temperature", temperature);  }

    
  
}


*/

void loop(){
    

  if (!client.connected()) {
    reconnect();
  }
     afiichage();


      double Irms = emon1.calcIrms(1480);

       char strIRMS[10]; 
       dtostrf(Irms, 5, 2, strIRMS);
       double valeurIRMS = strtod(strIRMS, NULL);
       
       double pa = valeurIRMS*220;

       
       char strp[20]; 
       dtostrf(pa, 11, 2, strp);
       double puissance = strtod(strp, NULL);

       


       

  char tempString[8];
    dtostrf(Irms, 1, 2, tempString);
    char humString[8];
    dtostrf(puissance, 1, 2, humString);
    sprintf(strbuf, "{\"courant\": %s}", tempString);
     sprintf(strbuf_2, "{\"puissance\": %s}",  humString);
    client.publish("esp32/i", strbuf);
    client.publish("esp32/p",strbuf_2);
  
  
  if (Firebase.ready() && signupOK ) {
    
    Firebase.RTDB.setFloat(&fbdo, "/BOX1/ipv",valeurIRMS);
    Firebase.RTDB.setFloat(&fbdo, "/BOX1/ppv", puissance);  }


  
if((millis()- duration)>= 1000){ 
      duration = millis();
             
             Firebase.RTDB.getString(&fbdo,"/BOX1/minutes");
             int seconds =0  ;
             int minutes =fbdo.stringData().toInt();
             minutes ++ ;
            Firebase.RTDB.setString(&fbdo, "/BOX1/minutes",(String) minutes);
            
    }

    
 

    }

