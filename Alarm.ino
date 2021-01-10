#include <Adafruit_ESP8266.h> //Biblioteca ESP
#include <SoftwareSerial.h>

//Pinos ESP
#define ARD_RX_ESP_TX   2
#define ARD_TX_ESP_RX   3
#define ESP_RST         4

SoftwareSerial softser(ARD_RX_ESP_TX, ARD_TX_ESP_RX);
Adafruit_ESP8266 wifi(&softser, &Serial, ESP_RST);

#define ESP_SSID "OSEUSSID"
#define ESP_PASS "ASUAPASS"

//Dados do IFTTT
#define HOST     "maker.ifttt.com"
#define PAGE     "/trigger/*********/with/key/**************"
#define PORT     80

int count = 0; //Var para os timers

//Vars da leitura dos sensores IR
int sensor1 = 0;
int sensor2 = 0;
int sensor3 = 0;

char buffer[50]; //Var requerida pela bilbioteca

void setup() {
  pinMode(5, OUTPUT); //Pin do Buzzer
  pinMode(6, OUTPUT); //Pin da Sirene
  digitalWrite(6, HIGH);
  digitalWrite(5, HIGH);
  delay(1000); //Apito longo para indicar que ligou
  digitalWrite(5, LOW);

  //20 segudos para o alarme accionar
  for (count = 0; count < 10 ; count++) {
    digitalWrite(5, HIGH);
    delay(1000);
    digitalWrite(5, LOW);
    delay(1000);
  }
}

void loop() {
  
  //Sensor1
  //Foram usados os pinos analógicos, pois pequenos picos de corrente originavam falsos disparos.
  sensor1 = analogRead(A0);
  if (sensor1 >= 500) {
    //Aviso com buzzer durante 20 segundos antes de disparar alarme
    for (count = 0; count < 10 ; count++)  {
      digitalWrite(5, HIGH);
      delay(1000);
      digitalWrite(5, LOW);
      delay(1000);
    }
    
    digitalWrite(6, LOW); //Liga a sirene
    mail(); //Envia notificação
    for (count = 0; count < 60 ; count++) { //Espera 1 minuto para parar a sirene
      delay(1000);
    }
    digitalWrite(6, HIGH);
  }

  //Sensor 2
  sensor2 = analogRead(A1);
  if (sensor2 >= 500) {
    for (count = 0; count < 10 ; count++) {
      digitalWrite(5, HIGH);
      delay(1000);
      digitalWrite(5, LOW);
      delay(1000);
    }
    digitalWrite(6, LOW);
    mail();
    for (count = 0; count < 15 ; count++)
    {
      delay(1000);
    }
    digitalWrite(6, HIGH);
  }

  //Sensor 3
  sensor3 = analogRead(A2);
  if (sensor3 >= 500) {
    for (count = 0; count < 10 ; count++) {
      digitalWrite(5, HIGH);
      delay(1000);
      digitalWrite(5, LOW);
      delay(1000);
    }
    digitalWrite(6, LOW);
    mail();
    for (count = 0; count < 60 ; count++) {
      delay(1000);
    }
    digitalWrite(6, HIGH);
  }
  delay(10); //Foi adicionado este delay que corrigiu falsos disparos

}

//Envia o email
void mail() {
 
  wifi.setBootMarker(F("Version:0.9.2.4]\r\n\r\nready")); //  Versão Firmware do ESP
  softser.begin(9600);
  Serial.begin(57600);
  
  // Teste ao ESP
  if (!wifi.hardReset()) {
    Serial.println(F("no response from module."));
    digitalWrite(5, HIGH); //Notifica erro no modulo pelo buzzer
    delay(500);
    digitalWrite(5, LOW);
  }
  if (!wifi.softReset()) {
    Serial.println(F("no response from module."));
    digitalWrite(5, HIGH);
    delay(500);
    digitalWrite(5, LOW);
  }
  Serial.print(F("Checking firmware version..."));
  wifi.println(F("AT+GMR"));
  if (wifi.readLine(buffer, sizeof(buffer))) {
    wifi.find();
  } else {
    Serial.println(F("error"));
    digitalWrite(5, HIGH);
    delay(500);
    digitalWrite(5, LOW);
  }
  //Fim do teste

  //Conecta ao WiFi
  Serial.print(F("Connecting to WiFi..."));
  wifi.connectToAP(F(ESP_SSID), F(ESP_PASS));

  //Conecta-se ao IFTTT
  wifi.connectTCP(F(HOST), PORT);
  wifi.requestURL(F(PAGE));
}
