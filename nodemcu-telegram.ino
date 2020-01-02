#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266TelegramBOT.h>

#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "";
const char* password = "";

#define BOTtoken ""
#define BOTname ""
#define BOTusername ""

#define RouterUrl ""
#define Referer ""

#define TelegramChatId ""

TelegramBOT bot(BOTtoken, BOTname, BOTusername);

String devices[10];
String newDevices[10];

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  while (!Serial) {
    //
  }

  delay(3000);

  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("\nConnecting WiFi.");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nWiFi connected");
  digitalWrite(LED_BUILTIN, HIGH);

  bot.begin();

  newDevices[0] = "";
  newDevices[1] = "";
  newDevices[2] = "";
  newDevices[3] = "";
  newDevices[4] = "";
  newDevices[5] = "";
  newDevices[6] = "";
  newDevices[7] = "";
  newDevices[8] = "";
  newDevices[9] = "";
}

void loop() {
  digitalWrite(LED_BUILTIN, LOW);

  String page = getDevicesPage();
  updateDevicesFromPage(page);

  Serial.println("Old devices:");
  Serial.println(devices[0]);
  Serial.println(devices[1]);
  Serial.println(devices[2]);
  Serial.println(devices[3]);
  Serial.println(devices[4]);
  Serial.println(devices[5]);
  Serial.println(devices[6]);
  Serial.println(devices[7]);
  Serial.println(devices[8]);
  Serial.println(devices[9]);

  Serial.println("New devices:");
  Serial.println(newDevices[0]);
  Serial.println(newDevices[1]);
  Serial.println(newDevices[2]);
  Serial.println(newDevices[3]);
  Serial.println(newDevices[4]);
  Serial.println(newDevices[5]);
  Serial.println(newDevices[6]);
  Serial.println(newDevices[7]);
  Serial.println(newDevices[8]);
  Serial.println(newDevices[9]);

  for (int i = 0; i < 10; i++) {
    if (isConnectedDevice(newDevices[i])) {
      sendMessage(newDevices[i], true);
    }

    if (isDisconnectedDevice(devices[i])) {
      sendMessage(devices[i], false);
    }
  }

  Serial.println("cool!");

  digitalWrite(LED_BUILTIN, HIGH);

  delay(30000);
}

String getDevicesPage() {
  WiFiClient client;

  HTTPClient http;

  http.begin(client, RouterUrl);
  http.addHeader("Referer", Referer);

  int httpCode = http.GET();

  int len = http.getSize();
  uint8_t buff[185] = { 0 };
  String page = "";

  WiFiClient * stream = http.getStreamPtr();
  stream->readBytes(buff, 185);

  int i = 0;

  while (http.connected() && (len > 0 || len == -1) && i < 2) {
    // get available data size
    size_t size = stream->available();

    if (size) {
      int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

      Serial.println("");
      Serial.println("");
      Serial.println((char*) buff);
      Serial.println("");

      page = page + (char*) buff;

      if (len > 0) {
        len -= c;
      }
    }
    delay(1);
    i++;
  }

  http.end();

  return page;
}

void updateDevicesFromPage(String page) {
  String ARRAY_START = "var hostList = new Array(";
  String ARRAY_END = ");";
  int arrayStartIndex = page.indexOf(ARRAY_START);
  int arrayEndIndex = page.indexOf(ARRAY_END, arrayStartIndex);
  String arrayString = page.substring(arrayStartIndex + ARRAY_START.length(), arrayEndIndex);

  bool isMac = false;
  String currentMac = "";
  int index = 0;

  devices[0] = newDevices[0];
  devices[1] = newDevices[1];
  devices[2] = newDevices[2];
  devices[3] = newDevices[3];
  devices[4] = newDevices[4];
  devices[5] = newDevices[5];
  devices[6] = newDevices[6];
  devices[7] = newDevices[7];
  devices[8] = newDevices[8];
  devices[9] = newDevices[9];

  newDevices[0] = "";
  newDevices[1] = "";
  newDevices[2] = "";
  newDevices[3] = "";
  newDevices[4] = "";
  newDevices[5] = "";
  newDevices[6] = "";
  newDevices[7] = "";
  newDevices[8] = "";
  newDevices[9] = "";

  for (int i = 0; i < arrayString.length(); i++) {
    if (arrayString.charAt(i) == '"') {
      if (isMac) {
         newDevices[index] = currentMac;
         index++;
         currentMac = "";
      }

      isMac = !isMac;

      i++;
    }

    if (isMac) {
      currentMac = currentMac + arrayString[i];
    }
  }
}

bool isConnectedDevice(String device) {
  bool isConnectedDevice = true;

  for (int i = 0; i < 10; i++) {
    if (devices[i].equals(device)) {
      isConnectedDevice = false;
    }
  }

  return isConnectedDevice;
}

bool isDisconnectedDevice(String device) {
  bool isDisconnectedDevice = true;

  for (int i = 0; i < 10; i++) {
    if (newDevices[i].equals(device)) {
      isDisconnectedDevice = false;
    }
  }

  return isDisconnectedDevice;
}

void sendMessage(String device, bool isConnected) {
  String message;

  if (isConnected) {
    message = " connected";
  } else {
    message = " disconnected";
  }

  String deviceName = device;

  if (device.equals("")) {
    deviceName = "Max (PC)";
  } else if (device.equals("")) {
    deviceName = "Max (mobile)";
  } else if (device.equals("")) {
    deviceName = "Lena (PC)";
  } else if (device.equals("")) {
    deviceName = "Lena (mobile)";
  } else if (device.equals("")) {
    deviceName = "TV";
  } else if (device.equals("")) {
    deviceName = "ESP8266";
  } else if (device.equals("")) {
    deviceName = "NodeMCU";
  }

  bot.sendMessage(TelegramChatId, deviceName + message, "");
}
