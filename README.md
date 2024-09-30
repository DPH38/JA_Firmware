# JA_Firmware

Este projeto é um firmware para ESP32 que se conecta a um broker MQTT para publicar leituras de temperatura e umidade de um sensor DHT11. O firmware também utiliza um cliente NTP para obter a hora atual.

## Estrutura do Projeto

.gitignore .pio/ .vscode/ include/ lib/ platformio.ini README.md src/ test/

## Dependências

As bibliotecas necessárias para este projeto estão listadas no arquivo platformio.ini:

```ini
lib_deps =
  knolleary/PubSubClient @ ^2.8
  adafruit/DHT sensor library @ ^1.4.3
  adafruit/Adafruit Unified Sensor @ ^1.1.6
  WiFiClientSecure
  arduino-libraries/NTPClient @ ^3.1.0
  ```

### Configuração

Clone o repositório.
Abra o projeto no PlatformIO.
Conecte seu ESP32 ao computador.
Compile e faça o upload do firmware para o ESP32.

Funcionalidades
Conexão WiFi
A função setup_wifi configura a conexão WiFi:

```cpp
void setup_wifi() {
  // Implementação da configuração WiFi
}
```

### Leitura de Sensor

A função loop lê os dados do sensor DHT11 e publica no broker MQTT:

```cpp
void loop() {
  // Implementação da leitura do sensor e publicação MQTT
}
```

### Formatação de Data e Hora

A função getFormattedDateTime formata a data e hora obtidas do cliente NTP:

```cpp
String getFormattedDateTime(unsigned long epochTime) {
  // Implementação da formatação de data e hora
}
```

Licença
Este projeto está licenciado sob a MIT License.
