// https://m2msupport.net/m2msupport/atcipstatus-get-the-current-connection-status/

#include <SoftwareSerial.h>

SoftwareSerial GSM(2, 3); // RX, TX

// String serial_data = "";
// String gsm_data = "";


#define LED_PIN 6
#define POWER_PIN 7
#define SIGNAL_PIN A5

int value = 0;

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, 0);

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

  // end value to server via GSM

  GSMcommand("AT+CIPSTATUS", "OK", "yy", "ERR", 5000, 2);
  GSMcommand("AT+CGATT?", "OK", "yy", "ERR", 5000, 2);
  GSMcommand("AT+CGATT=1", "OK", "yy", "ERR", 5000, 2);
  GSMcommand("AT+CIPSTATUS", "OK", "yy", "ERR", 5000, 2);
  GSMcommand("AT+CGDCONT=1,\"IP\",\"\"", "OK", "yy", "ERR", 5000, 2); // bring up wireless connection
  GSMcommand("AT+CIPSTATUS", "OK", "yy", "ERR", 5000, 2);
  GSMcommand("AT+CGACT=1,1", "OK", "yy", "ERR", 5000, 2);
  GSMcommand("AT+CIPSTATUS", "OK", "yy", "ERR", 5000, 2);
  // GSMcommand("AT+CIICR", "OK", "yy", "ERR", 10000, 2); //bring up wireless connection
  GSMcommand("AT+CIPSTATUS", "OK", "ALR", "yy", 5000, 2);
  sendData();
  GSMcommand("AT+CIPCLOSE", "OK", "CLOSE", "ERR", 5000, 1); // sending
  GSMcommand("AT+CIPSTATUS", "OK", "yy", "CLOSE", 5000, 2);
}

byte GSMcommand(String command, String response1, String response2, String response3, long timeOut, int repetitions)
{
  byte returnValue = NOTOK;
  byte count = 0;
  while (count < repetitions && returnValue != OK)
  {
    GSM.println(command);
    Serial.print(F("send: "));
    Serial.println(command);
    byte waitFor = GSMwaitFor(response1, response2, response3, timeOut);
    if (waitFor == OK)
    {
      //  Serial.println("OK");
      returnValue = OK;
    }
    else
    {
      returnValue = NOTOK;
    }
    count++;
  }
  return returnValue;
}

byte GSMwaitFor(String response1, String response2, String response3, long timeOut)
{
  unsigned long entry = millis();
  int count = 0;
  String reply = A6read();
  byte retVal = 99;
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
    retVal = TIMEOUT;
  }
  else
  {
    if (reply.indexOf(response1) >= 0 || reply.indexOf(response2) >= 0 || reply.indexOf(response3) >= 0)
    {
      retVal = OK;
    }
    else
    {
      retVal = NOTOK;
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
