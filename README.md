# Arduino-Weather-Station
Provides weather information based on selected location

<p align="center">
   <img src="https://github.com/ManolescuSebastian/Arduino-Weather-Station/blob/master/images/weather_station_picture.jpg" width="80%"></img>
</p>

Hardware requirements
-----

Arduino Weather Station is a 3D Printed homemade project, that has requires only a few components to build it
- Wemos D1 Mini 
- ST7735 1.8" Color TFT Display.
- female to female jumper wires
- (and a 3D Printer of course)

</br>

**No soldering required.**

</br>

This project body/case was printed using PLA (1.75mm) the .STL files are available on [Thingiverse](https://www.thingiverse.com/thing:3769926)

Pin connections between our device and the display.

Pin Connection
-----

Wemos D1 Mini | TFT 1.8" Display
------------ | -------------
5V | VCC
GND | GND
D8 | CS
D4 | RESET
D3 | A0
D7 | SDA
D5 | SCK
LED | 3V3

Future improvements
-----

Improvements that I would like to make in the near future:
- [ ] Add an **MPU-6050** (Gyro+Accelerometer Sensor) so that when I rotate the widget, layout would change from portrait to landscape mode
- [ ] Connect mobile phone to widget so that I can control the data on the screen (i.e. location, units etc)
