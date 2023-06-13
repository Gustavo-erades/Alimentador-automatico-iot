#include <Servo.h>
#define led D0
Servo servoD1;
int potenciometro=A0;
int valor_potenciometro;
/* para mexer com o display oled */
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define NUMFLAKES     10
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };
/* para conectar ao wife */
#include "ESP8266WiFi.h"
const char* ssid="NET_2GBB6DD6";
const char* password="E2BB6DD6";
WiFiServer server(80);
String HTTP_req; 
String URLValue;
String getURLRequest(String *requisicao);
bool mainPageRequest(String *requisicao);
void processaPorta(byte porta, byte posicao, WiFiClient cl);
void lePortaDigital(byte porta, byte posicao, WiFiClient cl);        
void lePortaAnalogica(byte porta, byte posicao, WiFiClient cl); 
const byte qtdePinosAnalogicos = 1;
byte pinosAnalogicos[qtdePinosAnalogicos] = {A0};


/*void escrever_display(int valor1) {
  display.clearDisplay();

  display.setTextSize(2);             
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(40,25);             
  display.println(valor1);
  
  display.setCursor(0,0);
  display.println("IOT-V4.1.7");

  if(WiFi.status()== WL_CONNECTED){
    display.setTextSize(1); 
    display.setCursor(75,50);             
    display.println(":)");
    display.setCursor(0,50);
    display.println(WiFi.localIP());
  }else{
    display.setTextSize(1); 
    display.setCursor(62,50);             
    display.println(":(");
    display.setCursor(0,50);
    display.println("Deu ruim!");
  }
  display.display();
  delay(10);
}*/
void escrever_display() {
  display.clearDisplay();

  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(35,25);
  display.println("Aperte 'OK' \n\n usando o IP abaixo");  
  display.setTextSize(2);      
  display.setCursor(0,0);
  display.println("IOT-V4.1.7");

  if(WiFi.status()== WL_CONNECTED){
    display.setTextSize(1); 
    display.setCursor(25,50);
    display.println(WiFi.localIP());
  }else{
    display.setTextSize(1); 
    display.setCursor(62,50);             
    display.println(":(");
    display.setCursor(5,50);
    display.println("Deu ruim!");
  }
  display.display();
  delay(10);
}
void abrir(int val){
  servoD1.write(val);
  escrever_display();
  delay(1000);
}

void setup() {
  servoD1.attach(D3);
  Serial.begin(9600);
  pinMode(led,INPUT);

  /*para mexer com o display oled*/
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  WiFi.begin(ssid,password);
  server.begin();
}

void loop() {
  WiFiClient  client = server.available();
  if(client){
    String currentLine="";
    while(client.connected()){
      if(client.available()){
        char c=client.read();
        if(c=='\n'){
          if(currentLine.length()==0){
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            client.println("<title>IOT VERSÃO 4.1.7</title>");
            client.println("<meta charset=\"UTF-8\">");
            client.println("<style>");
            client.println("h1{");
            client.println("font-family: 'Trebuchet MS', 'Lucida Sans Unicode', 'Lucida Grande', 'Lucida Sans', Arial, sans-serif;");
            client.println("}");
            client.println("a{");
            client.println("color:#000;");
            client.println("font-size:500px;");
            client.println("text-decoration: none;");
            client.println("font-family: 'Trebuchet MS', 'Lucida Sans Unicode', 'Lucida Grande', 'Lucida Sans', Arial, sans-serif;");
            client.println("font-weight: bold;");
            client.println("}");
            client.println("div{");
            client.println("display: inline-block;");
            client.println("background-color: rgb(218, 218, 218);");
            client.println("box-shadow: 2px 2px 3px;");
            client.println("border-radius: 12px;");
            client.println("padding: 8px;");
            client.println("cursor: pointer;");
            client.println("}");
            client.println("</style>");
            client.println("<h1>Alimentador Automático Versão <u>4.1.7</u></h1>");
            client.print("<div>");
            client.print("<a href=\"/H\">OK</a>");
            client.print("</div>");
            client.println();
            break;
          }else{
            currentLine="";
          }
        }else if(c!='\r'){
          currentLine+=c;
        }
          if(currentLine.endsWith("GET /H")){
          abrir(104);
        }/*else{
          
        }*/
      }
    }
  }
  client.stop();
  digitalWrite(led,HIGH);
  servoD1.write(19);
  /*valor_potenciometro=analogRead(potenciometro);
  valor_potenciometro=map(valor_potenciometro,0,1023,0,180);
  servoD1.write(valor_potenciometro);
  Serial.println(valor_potenciometro);
  display.clearDisplay();
  escrever_display(valor_potenciometro); 
  display.display();*/
}