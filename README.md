# wordclockESP V 7.0
 Wordclock for Expressif ESP8266, ESP-01S, Wemos D1 mini/pro based on FastLED

## Wordclock operation
- is designed to drive NeoPixel/WS2812B stripes
- supports different wordclock languages
- uses WiFi and LittleFS
- includes a webpages to configure wordclock
- starts an access point
- after connected to an existing network its a WiFi client
- connection properties as well as other properties are stored as JSON file in LittleFS
- settings like language, color, brightness, ntp server, timezone etc. can be changed using webpage
- firmware can be updated via HTTP from the webpage (no wiring necessary)
- MQTT support
- Alexa support
- Determines sunrise/sunset time according to latitude and longitude in order to adjust brightness slightly
- Supports different colors for time words and minutes corner leds

## Installation
- Edit and adapt Configuration.h
- Compile and upload sketch to ESP (check Arduino Settings for ESP)
- Start ESP and connect your computer to WiFi called "Wortuhr". If not changed in Configuration.h
- Start Browser and open IP which was displayed as ticker text while starting wordclock
- Select your WiFi and set apropriate password
- If wordclock now connects to your network you could now upload  2 html files which are located in data directory
  Necessary URL http://wortuhr/init
- If index.html and expert.htm is installed on ESP it's now possible to configure your wordclock
- Install all icon files from data directory in SPIFFS. Will be used to display icons on led matrix

## Settings in Arduino IDE

```
Build on Arduino IDE 1.8.15
```

For ESP8266
```bash
   Board:         esp8266 by esp8266 comunity Version 3.0.2
                  NodeMCU 1.0 (ESP-12E Module)
   CPU Frequency: 80 Mhz
   Flash Size:    4 M (1M SPIFFS)
   IwIP Variant:  v2 Higher Bandwidth
```


For ESP-01S:
```bash
   Board:         esp8266 by esp8266 comunity Version 2.7.4
                  Generic ESP8266 Module
   CPU Frequency: 80 Mhz
   Flash Mode:    QIO (fast)
   Reset Method:  no dtr (aka ck)
   IwIP Variant:  v2 Low Memory
   Partition Schema:  1MB (FS:64KB OTA:~470KB)
```
Used Libraries in Arduino IDE:
```bash
    ArduinoJson           6.19.4
    PubSubClient          2.8.0
    ESP8266WiFi           1.0 
    ESPAsyncTCP           1.2.2
    ESPAsyncUDP           1.1.0  
    ESPAsync_WiFiManager  1.13.0
    ESPAsyncDNSServer     1.0.0
    sunset                1.1.6
    ESP32-targz           1.1.2
    FastLED               3.5.0
```


## Special Actions
Some special actions are availabe 
```bash
http://<wordclock IP>/init              <- initial upload of files
http://<wordclock IP>/reset_wifi        <- resets WiFi settings
http://<wordclock IP>/format            <- formats SPIFFS, settings and index.html will be deleted
http://<wordclock IP>/state             <- Display state of Wordclock
http://<wordclock IP>/reboot            <- Reboots Wordclock
http://<wordclock IP>/show_icon         <- Displays an icon file from SPIFFS
http://<wordclock IP>/mqttenable        <- Enabled MQTT Server Connection
http://<wordclock IP>/mqttdisable       <- Disables MQTT Server Connection
http://<wordclock IP>/do?remove=xxxxxx  <- remove file from SPIFFS
http://<wordclock IP>/do?hostname=xxxxx <- sets a new hostname for wordclock
http://<wordclock IP>/do?effect=n       <- shows nice patterns, n=1-12
http://<wordclock IP>/do?datapin		<- switch to another data pin if wiring differs from configuration
http://<wordclock IP>/do?unset=xxxx		<- deletes a name=value pair from json settings file
http://<wordclock IP>/do?setxxxx=yyy	<- sets a name=value pair from json settings file


```

## License
[Mit](https://mit-license.org/)
