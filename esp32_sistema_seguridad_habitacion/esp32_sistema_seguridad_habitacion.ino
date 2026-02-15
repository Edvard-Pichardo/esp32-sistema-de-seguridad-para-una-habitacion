/*
 * Proyecto: Sistema de seguridad, confort y control ambiental en una habitación mediante la placa ESP32 
 * Seminario de Ciencias de la Computación A: Introducción a IoT 
 * Escrito por Pichardo Rico Cristian Eduardo
 * Última fecha de modificación: 16-11-2024
*/

//Incluimos bibliotecas
#include <WiFi.h>                  //Controla la conectividad inalámbrica del ESP32.
#include <DHT.h>                   //Driver para sensores de temperatura y humedad tipo DHT.
#include <WiFiClientSecure.h>      //Cliente TCP con cifrado TLS/SSL.
#include <UniversalTelegramBot.h>  //Cliente de alto nivel para la API de Telegram.


//Configuración WiFi
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

//Token del bot de Telegram
const char* BOT_TOKEN = "YOUR_TELEGRAM_BOT_TOKEN"; //identifica el bot.

//ID de usuario de Telegram
String CHAT_ID = "YOUR_CHAT_ID";  //restringe el sistema a un usuario autorizado.

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);


//Variables de los sensores
#define led1 14      // LED 1; iluminación
#define led2 27      // LED 2
#define led3 26      // LED 3
#define led4 25      // LED 4 
#define led5 33      // LED 5
#define luz 34       // LDR; nivel de iluminación ambiental
#define PIR 2        // PIR; detección de movimiento 
#define buz 23       // Buzzer; alarma sonora
#define infra 22     // Infrarrojo; apertura de caja/compartimento
#define pinDHT 32    // DHT11; temperatura y humedad
#define DHTTYPE DHT11

//Inicializamos el sensor DHT
DHT dht(pinDHT, DHTTYPE);

//Definimos los leds que se usarán en un arreglo
const int leds[5] = {led1, led2, led3, led4, led5};

//Variables para el control de Telegram
unsigned long contador;
const int botRequestDelay = 500;

//Variables para el control de las Alarmas
int umbral = 1500; //Umbral inicial de la fotorresistencia
int buzzer = 0;

bool alarmEnabled = false; //Estado de la alarma de movimiento
bool alarmEnabled2 = false; //Estado de la alarma de la caja de seguridad
bool alarmEnabled3 = false; //Estado de la alarma de luces automaticas

const int intervalo = 3000; //Intervalo de la alarma de movimiento
const int intervalo2 = 1000; //Intervalo de la caja de seguridad
const int intervalo3 = 2000; //Intervalo para las luces automaticas
const int intervalo4 = 20000; //Intervalo para apagar luces automaticas
const int intervalo5 = 30000; //Intervalo para leer temperatura

unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
unsigned long previousMillis5 = 0;
unsigned long ultimaDeteccion = 0;

void setup() {
  //Iniciamos comunicación Serial
  Serial.begin(115200);
  //Iniciamos el DHT
  dht.begin(); 

  //Definimos pines de los sensores
  pinMode(luz, INPUT); 
  pinMode(PIR, INPUT); 
  pinMode(buz, OUTPUT);
  pinMode(infra, INPUT);

  //Definimos los leds como salida dentro de un ciclo for
  for (int i = 0; i < 5 ; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }
  

  //Conectamos el WiFi
  WiFi.begin(ssid, password);
  Serial.println("Conectando WiFi...");
  WiFi.setSleep(false); 
  WiFi.setAutoReconnect(true);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  Serial.print("Conectado a: ");
  Serial.println(ssid);
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  delay(1000);

  //Configuración del cliente seguro
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); //Asegura la conexión HTTPS

  Serial.println("Bot inicializado");
}



void handleNewMessages(int numNewMessages){
  for (int i = 0; i < numNewMessages; i++) {
    String senderID = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Usuario";
    
    //Validamos bot privado
    if (senderID == CHAT_ID){
      if (text == "/start"){
        bot.sendMessage(CHAT_ID, "Bienvenido " + from_name + ". Soy tu bot privado.", "");
      } 

      //Comando de prueba para garantizar que todo esté bien
      else if (text == "/status"){
        bot.sendMessage(CHAT_ID, "Todo está funcionando correctamente.", "");
      } 

      //Comando para activar la alarma de movimiento
      else if (text == "/alarma_on"){ 
        alarmEnabled = true;
        bot.sendMessage(CHAT_ID, "Alarma de movimiento activada.", "");
        Serial.println("Alarma de movimiento activada.");
      }

      //Comando para desactivar la alarma de movimiento
      else if (text == "/alarma_off"){
        buzzer=0; 
        alarmEnabled = false;
        digitalWrite(buz, LOW);
        bot.sendMessage(CHAT_ID, "Alarma de movimiento desactivada.", "");
        Serial.println("Alarma de movimiento desactivada.");
      }

      //Comando para activar la alarma de la caja de seguridad
      else if (text == "/caja_fuerte_on"){ 
        alarmEnabled2 = true;
        bot.sendMessage(CHAT_ID, "Alarma de la caja de seguridad activada.", "");
        Serial.println("Alarma de la caja de seguridad activada.");
      }

      //Comando para desactivar la alarma de la caja de seguridad
      else if (text == "/caja_fuerte_off"){ 
        alarmEnabled2 = false;
        bot.sendMessage(CHAT_ID, "Alarma de la caja de seguridad desactivada.", "");
        Serial.println("Alarma de la caja de seguridad desactivada.");
      }

      //Comando para activar las luces automáticas
      else if (text == "/luz_automatica_on"){ 
        alarmEnabled3 = true;
        bot.sendMessage(CHAT_ID, "Luces automáticas encendidas.", "");
        Serial.println("Luces automáticas encendidas.");
      }

      //Comando para desactivar las luces automáticas
      else if (text == "/luz_automatica_off"){ 
        alarmEnabled3 = false;
        bot.sendMessage(CHAT_ID, "Luces automáticas desactivadas.", "");
        Serial.println("Luces automáticas desactivadas.");
      }

      //Comando para el control de las luces 
      else if (text == "/luces_on"){
        if(alarmEnabled3){
          bot.sendMessage(CHAT_ID, "Las luces automaticas se han desactivado.", "");
          alarmEnabled3=false; 
          for(int i=0; i<5; i++){
            digitalWrite(leds[i], HIGH);
          } 
          bot.sendMessage(CHAT_ID, "Las luces están encendidas." , "");
          Serial.println("Las luces están encendidas.");
        }
        for(int i=0; i<5; i++){
          digitalWrite(leds[i], HIGH);
        } 
        bot.sendMessage(CHAT_ID, "Las luces están encendidas." , "");
        Serial.println("Las luces están encendidas.");
      }
      
      else if (text == "/luces_off"){
        if(alarmEnabled3){
          bot.sendMessage(CHAT_ID, "Las luces automaticas se han desactivado.", "");
          alarmEnabled3 = false;
        }

        for(int i=0; i<5; i++){
          digitalWrite(leds[i], LOW);
        }
        bot.sendMessage(CHAT_ID, "Las luces están apagadas." , "");
        Serial.println("Las luces están apagadas.");

      }


      //Comando para la temperatura y humedad
      else if(text == "/clima"){
        float h = dht.readHumidity();
        float t = dht.readTemperature();

        //Probamos el DHT
        if (isnan(h) || isnan(t)){
          Serial.println("Error al leer el sensor DHT");
          return;
        }

        bot.sendMessage(CHAT_ID, "Temperatura: " + String(t) + "°C", "");
        Serial.println("Temperatura: " + String(t) + "°C");
        bot.sendMessage(CHAT_ID, "Humedad: " + String(h) + "%");
        Serial.println("Humedad: " + String(h) + "%");
      }
      
      else{
        bot.sendMessage(CHAT_ID, "Comando no reconocido.", "");
      }
    } 
    
    else{
      bot.sendMessage(senderID, from_name + ". No tienes permiso para usar este bot.", "");
    }
  }
}





void loop() {
  
  if (millis() > contador + botRequestDelay){
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    if (numNewMessages){
      handleNewMessages(numNewMessages);
    }
    contador = millis();
  }

  //Alarma de movimiento
  if(alarmEnabled){
    if(millis() - previousMillis >= intervalo){
      previousMillis = millis();
      
      int pirState = digitalRead(PIR);
      if (pirState == HIGH) {
        //Enviar notificación de detección de movimiento
        bot.sendMessage(CHAT_ID, "¡Alarma! Se ha detectado movimiento.", "");
        Serial.println("¡Movimiento detectado!");
        buzzer=buzzer+1;
        //Serial.println(buzzer);
        if(buzzer == 3){
          digitalWrite(buz, HIGH);
          Serial.println("El buzzer se ha activado");
          bot.sendMessage(CHAT_ID, "¡Buzzer activado!", "");
        }
      }
    }
  }

  //Alarma de la caja de seguridad
  if(alarmEnabled2){
    if(millis() - previousMillis2 >= intervalo2){
      previousMillis2 = millis();

      int infraState = digitalRead(infra);
      if (infraState == HIGH){
        //Enviar notificación dde abertura de la caja/cajon
        bot.sendMessage(CHAT_ID, "¡Alarma! Se ha abierto la caja de seguridad.", "");
        Serial.println("¡Se ha abierto la caja de seguridad!");
      }
    }
  }

  //Luces automaticas
  if(alarmEnabled3){
    if(millis() - previousMillis3 >= intervalo3){
      previousMillis3 = millis();

      int pirState = digitalRead(PIR);
      int lightLevel = analogRead(luz);

      if(lightLevel < umbral){
        if (pirState == HIGH) {
          ultimaDeteccion = millis();
          //Encender las luces
          for(int i=0; i<5; i++){
            digitalWrite(leds[i], HIGH);
          }
        }
        else{
          if(millis() - ultimaDeteccion >= intervalo4){
            for(int i=0; i<5; i++){
              digitalWrite(leds[i], LOW);
            }
          }
        }
      }
    }
  }

  //Alarma de ventilación
  if(millis() - previousMillis5 >= intervalo5){
    previousMillis5 = millis();
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if(t>=25){
       bot.sendMessage(CHAT_ID, "¡Alarma! Ventilar habitación.", "");
       bot.sendMessage(CHAT_ID, "Estamos a " + String(t) + "°C", "");
       Serial.println("¡Ventilar habitación!");
    }   
  }
}
