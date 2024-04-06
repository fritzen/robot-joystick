#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <Servo.h>

#include "captive_html.h" 
#include "joystick_html.h"

Servo myservo1;
Servo myservo2;

WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer server(80);
DNSServer dnsServer;

const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);


const int pwmMotorA = 5;
const int pwmMotorB = 4;
const int dirMotorA = 0;
const int dirMotorB = 2;

IPAddress local_IP(192,168,0,1);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);


bool isClaw = false;
bool useServo = true; // Define como verdadeiro para usar servos

void setup() {
  Serial.begin(9600);
  myservo1.attach(5);
  myservo2.attach(16);

  pinMode(pwmMotorA, OUTPUT);
  pinMode(pwmMotorB, OUTPUT);
  pinMode(dirMotorA, OUTPUT);
  pinMode(dirMotorB, OUTPUT);

  WiFi.mode(WIFI_AP);
  //WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAPConfig(local_IP, gateway, subnet);
  
  WiFi.softAP("ROBOT-NETWORK");

  //dnsServer.start(DNS_PORT, "*", apIP);

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  server.on("/", []() { server.send_P(200, "text/html", joystick_html); });
//  server.on("/joystick", []() { server.send_P(200, "text/html", joystick_html); }); // Rota para o aplicativo do joystick de robô

  // server.onNotFound([]() {
  //   server.send(200, "text/html", joystick_html); // Por padrão, direciona para o portal cativo
  // });

  server.begin();
}

void loop() {
  
  webSocket.loop();
  server.handleClient();
  //dnsServer.processNextRequest();
}

bool dns = true;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  if (type == WStype_TEXT) {
    String txt = "";
    for (int i = 0; i < length; i++) {
      txt = txt + ((char)payload[i]);
    }

    int delimiterIndex = txt.indexOf(',');

    if (delimiterIndex != -1) {

      String xStr = txt.substring(0, delimiterIndex);
      String yStr = txt.substring(delimiterIndex + 1);
      int x = xStr.toInt();
      int y = yStr.toInt();

      if (useServo) {
        controlServo(x, y);
      } else {
        controlMotor(x, y);
      }
    }
  }
}

void controlMotor(int x, int y) {
  int speedA = map(x, -255, 255, -255, 255);
  int speedB = map(y, -255, 255, -255, 255);

  digitalWrite(dirMotorA, speedA >= 0 ? HIGH : LOW);
  digitalWrite(dirMotorB, speedB >= 0 ? HIGH : LOW);
  
  analogWrite(pwmMotorA, abs(speedA));
  analogWrite(pwmMotorB, abs(speedB));
}

void controlServo(int x, int y) {

  // Mapeia os valores de entrada para o intervalo de ângulo típico de 0 a 180 graus
  int angleA = map(x, -255, 255, 0, 180);
  int angleB = map(y, -255, 255, 180, 0); // Inverte a direção para o segundo servo

    
  myservo1.write(angleA);
  myservo2.write(angleB);
}
