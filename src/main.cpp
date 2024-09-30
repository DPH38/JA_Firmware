#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "DHT.h"
#include "config.h"
#include <vector>
#include <string>
#include <FS.h>
#include <SPIFFS.h>

WiFiClientSecure espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (500)
char msg[MSG_BUFFER_SIZE];
int value = 0;
bool printFlag = true; // Flag to control printing

// DHT Sensor settings
#define DHTPIN 13     // Pino conectado ao DHT11 pino 13
#define DHTTYPE DHT11 // Tipo do sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// NTP Client settings
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000); // UTC-3 for São Paulo, update interval set to 60 seconds

// HiveMQ Cloud Let's Encrypt CA certificate
static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

// Variáveis globais para armazenar os valores do arquivo de configuração
std::vector<String> ssids = {"SSID1", "SSID2", "SSID3"};
std::vector<String> passwords = {"PASS1", "PASS2", "PASS3"};
String mqtt_server = "....s1.eu.hivemq.cloud";
String mqtt_username = "USERNAME";
String mqtt_password = "PASSWORD";

void setup_wifi()
{
  delay(1000);
  Serial.println();

  while (WiFi.status() != WL_CONNECTED)
  {
    for (size_t i = 0; i < ssids.size(); i++)
    {
      Serial.print("Connecting to ");
      Serial.println(ssids[i].c_str());

      WiFi.mode(WIFI_STA);
      WiFi.begin(ssids[i].c_str(), passwords[i].c_str());

      // Adicionar um pequeno atraso para garantir que o módulo WiFi tenha tempo para inicializar
      delay(1000);

      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 20)
      {
        delay(500);
        Serial.print(".");
        attempts++;
      }

      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        return; // Conexão bem-sucedida, sair da função
      }
      else
      {
        Serial.println("");
        Serial.print("Failed to connect to WiFi: ");
        Serial.println(ssids[i].c_str());
        Serial.print("Error code: ");
        int status = WiFi.status();
        Serial.println(status);
        switch (status)
        {
        case WL_IDLE_STATUS:
          Serial.println("Error: WiFi is in idle mode.");
          break;
        case WL_NO_SSID_AVAIL:
          Serial.println("Error: SSID not available.");
          break;
        case WL_SCAN_COMPLETED:
          Serial.println("Error: WiFi scan completed.");
          break;
        case WL_CONNECT_FAILED:
          Serial.println("Error: Connection failed.");
          break;
        case WL_CONNECTION_LOST:
          Serial.println("Error: Connection lost.");
          break;
        case WL_DISCONNECTED:
          Serial.println("Error: Disconnected from WiFi.");
          break;
        default:
          Serial.println("Error: Unknown error.");
          break;
        }
      }
    }

    // Se não conseguir conectar a nenhuma rede após 3 tentativas, reiniciar o módulo WiFi
    Serial.println("Could not connect to any WiFi network, restarting WiFi module...");
    WiFi.disconnect();
    delay(5000); // Esperar antes de tentar novamente
  }
}

void check_wifi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    setup_wifi();
  }
}

void reconnect()
{
  check_wifi(); // Ensure WiFi is connected before attempting MQTT connection

  // Loop until we’re reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection… ");
    String clientId = "ESP32Client";
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username.c_str(), mqtt_password.c_str()))
    {
      Serial.println("connected!");
    }
    else
    {
      Serial.print("failed, rc = ");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
      yield(); // Allow the OS to reset the WDT
    }
  }
}

void setup()
{
  delay(1000);
  // When opening the Serial Monitor, select 9600 Baud
  Serial.begin(115200);
  delay(1000);

  setup_wifi();

  espClient.setCACert(root_ca);
  client.setServer(mqtt_server.c_str(), 8883);

  dht.begin();
  timeClient.begin();
}

String getFormattedDateTime(unsigned long epochTime)
{
  struct tm *ptm = gmtime((time_t *)&epochTime);
  char dateTime[25];
  sprintf(dateTime, "%04d-%02d-%02d %02d:%02d:%02d",
          ptm->tm_year + 1900,
          ptm->tm_mon + 1,
          ptm->tm_mday,
          ptm->tm_hour,
          ptm->tm_min,
          ptm->tm_sec);
  return String(dateTime);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  timeClient.update();

  unsigned long now = millis();
  if (now - lastMsg > 2000)
  {
    lastMsg = now;

    // Read temperature and humidity
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t))
    {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Print the readings to the Serial Monitor
    if (printFlag)
    {
      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.println(" *C");
    }

    // Get the current timestamp
    unsigned long epochTime = timeClient.getEpochTime();
    String formattedDateTime = getFormattedDateTime(epochTime);

    // Publish the readings to the MQTT broker with timestamp
    snprintf(msg, MSG_BUFFER_SIZE, "Timestamp: %s, Humidity: %.2f %% Temperature: %.2f *C", formattedDateTime.c_str(), h, t);
    if (printFlag)
    {
      Serial.print("Publish message: ");
      Serial.println(msg);
    }
    client.publish("sensor/humidity_temperature", msg);
  }
}