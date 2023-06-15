#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo motor;

const char* ssid = "ESTACIO-VISITANTES";
const char* password = "estacio@2014";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000);

WiFiServer server(80);

int servoAngle = 0;
int targetHour = 0;
int targetMinute = 0;
bool motorActivated = false;

const int initialAngle = 45;     // Ângulo inicial do servo motor
const int executionAngle = 90;  // Ângulo durante a execução

void setup() {
  Serial.begin(115200);

  motor.attach(D3);
  motor.write(initialAngle);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("PET FEEDER");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.begin();

  timeClient.begin();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("PET FEEDER");
  display.setCursor(0, 20);
  display.println("Conectado ao WiFi");
  display.setCursor(0, 40);
  display.print("IP: ");
  display.setTextSize(1);
  display.println(WiFi.localIP());
  display.display();
  delay(3000);
}

void loop() {
  motor.write(initialAngle);
  timeClient.update();

  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    if (request.indexOf("/horario") != -1) {
      int index = request.indexOf("?hora=");
      if (index != -1) {
        int index2 = request.indexOf("&", index);
        if (index2 != -1) {
          String horaStr = request.substring(index + 6, index2);
          int hora = horaStr.toInt();
          int minuto = request.substring(index2 + 8).toInt();

          if (hora >= 0 && hora <= 23 && minuto >= 0 && minuto <= 59) {
            targetHour = hora;
            targetMinute = minuto;
            motorActivated = true;
            Serial.print("Motor servo ativado para ");
            Serial.print(targetHour);
            Serial.print(":");
            Serial.println(targetMinute);
          }
        }
      }
    }
    client.println("<html lang=\"pt-br\"><head>");
    client.println("<title>PET FEEDER</title>");
    client.println("<meta charset=\"UTF-8\">");
    client.println("<style>");
    client.println("body { font-family: Arial, sans-serif; background-color: #000000; }");
    client.println(".container { width: 400px; margin: 50px auto; background-color: #FFFFFF; border-radius: 10px; padding: 20px; }");
    client.println(".title { background-color: #006400; color: #FFFFFF; padding: 10px; font-size: 20px; text-align: center; border-radius: 10px 10px 0 0; }");
    client.println(".description { padding: 20px; margin-bottom: 40px; font-style: italic; color: #006400; }");
    client.println(".time-input { display: flex; justify-content: center; align-items: center; }");
    client.println(".time-input label { color: #FFFFFF; font-weight: bold; margin-right: 10px; }");
    client.println(".time-input input[type='number'] { width: 45%; padding: 10px; margin-bottom: 10px; border-radius: 5px; }");
    client.println("input[type='submit'] { width: 100%; padding: 10px; margin-top: 10px; background-color: #006400; color: #FFFFFF; border: none; border-radius: 5px; cursor: pointer; }");
    client.println(".dog-icon { font-size: 24px; }");
    client.println(".alert { background-color: #FF0000; color: #FFFFFF; padding: 10px; text-align: center; border-radius: 5px; }");
    client.println("</style>");
    client.println("</head><body>");
    client.println("<div class='container'>");
    client.println("<div class='title'>Alimentador Automático <span class='dog-icon'>.</span></div>");
    client.println("<div class='description'>O melhor alimentador para seu pet!</div>");

    client.println("<form action='/horario' method='get'>");
    client.println("<div class='time-input'>");
    client.println("<label for='hora' style='color: #FFFFFF;'>Hora:</label>");
    client.print("<input type='number' name='hora' placeholder='00' min='0' max='23' value='");
    client.print(targetHour);
    client.print("' required>");
    client.println("<label for='minuto' style='color: #FFFFFF;'>Minutos:</label>");
    client.print("<input type='number' name='minuto' placeholder='00' min='0' max='59' value='");
    client.print(targetMinute);
    client.print("' required>");
    client.println("</div>");
    client.println("<br>");
    client.println("<input type='submit' value='Adicionar Horário' style='margin-top: 20px;'>");
    client.println("</form>");
    client.println("</div>");
    client.println("</body></html>");

    delay(10);
    client.stop();
    Serial.println("Client disconnected");
  }

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("PET FEEDER");

  display.setTextSize(2);
  display.setCursor(0, 18);
  display.print(formatDigits(timeClient.getHours()));
  display.print(":");
  display.print(formatDigits(timeClient.getMinutes()));
  display.print(":");
  display.println(formatDigits(timeClient.getSeconds()));

  // Exibir horário definido pelo usuário
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print("Hora definida: ");
  display.print(formatDigits(targetHour));
  display.print(":");
  display.print(formatDigits(targetMinute));

  display.setCursor(0, 54);
  display.print("IP: ");
  display.println(WiFi.localIP());

  display.display();

  if (motorActivated && timeClient.getHours() == targetHour && timeClient.getMinutes() == targetMinute) {
    motor.write(executionAngle); // Movimenta para o ângulo de execução
    motorActivated = false;
    delay(3000); // Mantém na posição por 2 segundos
    motor.write(initialAngle); // Volta para a posição inicial
    delay(3000); // Aguarda 3 segundos antes de iniciar novamente
  }
}

String formatDigits(int digits) {
  if (digits < 10) {
    return "0" + String(digits);
  }
  return String(digits);
}
