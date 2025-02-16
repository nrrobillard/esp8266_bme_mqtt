# esp8266_bme_mqtt

## Setup
Create a config.h file
```
#define MQTT_USERNAME <your MQTT username>
#define MQTT_PASSWORD <your MQTT password>
#define MQTT_IP <your MQTT broker IP address>
#define SENSOR_NAME <your sensor name>
```

## Building
### Install arduino-cli
Run the following with Git Bash
```
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
```
https://arduino.github.io/arduino-cli/0.25/installation/

Optionally, add arduino-cli to the PATH environment variable so it can be ran with powershell/command prompt. Look up the location running the following in Git Bash
```
which arduino-cli
```
Then add that to your windows PATH variable
https://www.architectryan.com/2018/03/17/add-to-the-path-on-windows-10/



### Setup esp8266 core
```
arduino-cli core install esp8266:esp8266 --additional-urls http://arduino.esp8266.com/stable/package_esp8266com_index.json

```

### Compile
```
arduino-cli compile -v --fqbn esp8266:esp8266:nodemcuv2 esp8266_bme_mqtt.ino
```

### Program board
(Note: that COM port may be a different number. To determine what it is, open Windows Device Manager, and expands the "Ports" section. Plug in the board and see what new COM port appears.)
```
arduino-cli upload -v -p COM4 --fqbn esp8266:esp8266:nodemcuv2 esp8266_bme_mqtt.ino
```