// https://m2msupport.net/m2msupport/atcipstatus-get-the-current-connection-status/

#include <SoftwareSerial.h>

SoftwareSerial GSM(2, 3); // RX, TX

// String serial_data = "";
// String gsm_data = "";

String host = "domain.com";
char end_c[2];

#define LED_PIN 6
#define POWER_PIN 7
#define SIGNAL_PIN A5

int value = 0;

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, 0);

  // ctrlZ String definition
  end_c[0] = 0x1a;
  end_c[1] = '\0';

  Serial.begin(9600);
  while (!Serial)
    ;
  ;
  GSM.begin(9600);
}

void loop()
{
  // while (Serial.available() > 0) {
  //   char s = Serial.read();
  //   serial_data += s;
  //   delay(10);
  // }
  // if (serial_data.length() > 0) {
  //   GSM.println(serial_data);
  //   serial_data = "";
  // }
  // while (GSM.available() > 0) {
  //   char s = GSM.read();
  //   gsm_data += s;
  //   delay(10);
  // }
  // if (gsm_data.length() > 0) {
  //   Serial.println(gsm_data);
  //   gsm_data = "";
  // }
  while (GSM.available() > 0)
  {
    Serial.write(GSM.read());
  }
  while (Serial.available() > 0)
  {
    GSM.write(Serial.read());
  }

  // if (value >= 100)
  // {
  //   digitalWrite(LED_PIN, 1);
  // }
  // else
  // {
  //   digitalWrite(LED_PIN, 0);
  // }
  // delay(1000);
  Serial.println(millis());
}

void sendData()
{
  digitalWrite(POWER_PIN, 1);
  delay(10);
  value = analogRead(SIGNAL_PIN);
  digitalWrite(POWER_PIN, 0);
  delay(10);

  // send value to server via GSM
}

void sendData(String data) {
    A6command("AT+CIPSTATUS", "OK", "yy", "ERR", 5000, 2);
    A6command("AT+CGATT?", "OK", "yy", "ERR", 5000, 2);
    A6command("AT+CGATT=1", "OK", "yy", "ERR", 5000, 2);
    A6command("AT+CIPSTATUS", "OK", "yy", "ERR", 5000, 2);
    A6command("AT+CGDCONT=1,\"IP\",\"\"", "OK", "yy", "ERR", 5000, 2); // bring up wireless connection
    A6command("AT+CIPSTATUS", "OK", "yy", "ERR", 5000, 2);
    A6command("AT+CGACT=1,1", "OK", "yy", "ERR", 5000, 2);
    A6command("AT+CIPSTATUS", "OK", "yy", "ERR", 5000, 2);
    // A6command("AT+CIICR", "OK", "yy", "ERR", 10000, 2); //bring up wireless connection
    A6command("AT+CIPSTATUS", "OK", "ALR", "yy", 5000, 2);
    // ---------------------------------
    A6command("AT+CIFSR", "OK", "yy", "yy", 5000, 3); // get local IP adress
    A6command("AT+CIPSTATUS", "OK", "ALR", "yy", 5000, 3);
    A6command("AT+CIPSTART=\"TCP\",\"" + host + "\",80", "CONNECT", "ALR", "ERR", 5000, 3); // start up the connection
    // A6input();
    delay(1000);
    // A6command("AT+CIPSTATUS", "OK", "ALR", "yy", 5000, 2);
    A6command("AT+CIPSEND", ">", "ERR", "ALR", 5000, 1); // begin send data to remote server
    delay(500);
    // --------------------------
    A6board.print(F("POST /xxxxxxxxx"));
    A6board.print(F(" HTTP/1.1"));
    A6board.print(F("\r\n"));
    A6board.print(F("HOST: "));
    A6board.print(host);
    A6board.print(F("\r\n"));
    A6board.print(F("Content-Type: application/json; charset=utf-8"));
    A6board.print(F("\r\n"));
    A6board.print(F("Content-Length: "));
    A6board.print(data.length());
    A6board.print(F("\r\n"));
    A6board.print(F("Connection: keep-alive"));
    A6board.print(F("\r\n"));
    A6board.print(F("Keep-Alive: timeout=30, max=100"));
    A6board.print(F("\r\n"));
    A6board.print(F("\r\n"));

    A6board.print(data);
    A6board.print(F("\r\n"));
    A6board.print(F("\r\n"));

    Serial.print(F("POST /xxxxxxxxxx"));
    Serial.print(F(" HTTP/1.1"));
    Serial.print(F("\r\n"));
    Serial.print(F("HOST: "));
    Serial.print(host);
    Serial.print(F("\r\n"));
    Serial.print(F("Content-Type: application/json; charset=utf-8"));
    Serial.print(F("\r\n"));
    Serial.print(F("Content-Length: "));
    Serial.print(data.length());
    Serial.print(F("\r\n"));
    Serial.print(F("Connection: keep-alive"));
    Serial.print(F("\r\n"));
    Serial.print(F("Keep-Alive: timeout=30, max=100"));
    Serial.print(F("\r\n"));
    Serial.print(F("\r\n"));

    Serial.print(data);
    Serial.print(F("\r\n"));
    Serial.print(F("\r\n"));
    // --------------------------
    A6command(end_c, "HTTP/1.1", "CLOSED", "ERR", 60000, 1); // begin send data to remote server
    // A6board.println(end_c); //sending ctrlZ
    // unsigned long entry = millis();
    Serial.println(F("After sending data: checking if connection is still OPEN"));
    delay(500);
    uint8_t still = A6command("AT+CIPSTATUS", "CONNECT OK", "ALR", "CLOSE", 5000, 2);
    if (still == 1)
    { // send another data if TCP connection is still on
        Serial.println(F("Send data again, connection is OPEN"));
        sendData(dataToSend);
    }
    // else
    // {
    //     Serial.println(F("Connection is CLOSED, exiting"));
    //     delay(500);
    // }
    // ----------------------------------
    A6command("AT+CIPCLOSE", "OK", "CLOSE", "ERR", 5000, 1); // sending
    A6command("AT+CIPSTATUS", "OK", "yy", "CLOSE", 5000, 2);
}

uint8_t GSMcommand(String command, String response1, String response2, String response3, long timeOut, int repetitions)
{
  uint8_t returnValue = 0;
  byte count = 0;
  while (count < repetitions && returnValue != 1)
  {
    GSM.println(command);
    Serial.print(F("send: "));
    Serial.println(command);
    uint8_t waitFor = GSMwaitFor(response1, response2, response3, timeOut);
    if (waitFor == 1)
    {
      //  Serial.println("OK");
      returnValue = 1;
    }
    else
    {
      returnValue = 0;
    }
    count++;
  }
  return returnValue;
}

uint8_t GSMwaitFor(String response1, String response2, String response3, long timeOut)
{
  unsigned long entry = millis();
  int count = 0;
  String reply = A6read();
  uint8_t retVal = 99;
  do
  {
    reply = GSMread();
    if (reply != "")
    {
      Serial.print(F("reply "));
      Serial.print((millis() - entry));
      Serial.println(F(" ms "));
      Serial.println(reply);
      Serial.println(F("--------------------------"));
      delay(100);
    }
  } while ((reply.indexOf(response1) + reply.indexOf(response2) + reply.indexOf(response3) == -3) && millis() - entry < timeOut && (millis() - entry) < 120000);
  if ((millis() - entry) >= timeOut || (millis() - entry) >= 120000)
  {
    retVal = 2;
  }
  else
  {
    if (reply.indexOf(response1) >= 0 || reply.indexOf(response2) >= 0 || reply.indexOf(response3) >= 0)
    {
      retVal = 1;
    }
    else
    {
      retVal = 0;
    }
  }
  //  Serial.print("retVal = ");
  //  Serial.println(retVal);
  return retVal;
}

String GSMread()
{
  String reply = "";
  if (GSM.available())
  {
    reply = GSM.readString();
  }
  return reply;
}
