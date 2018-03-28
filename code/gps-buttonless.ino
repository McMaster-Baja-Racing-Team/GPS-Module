#include <Adafruit_GPS.h>
#include <SoftwareSerial.h> //For GPS - Current IDE needs this library
#include <SPI.h>
#include <SD.h>

/*Pinouts
 * GPS Vin-3V3, GND, RX-2, TX-3
 * SD GND, Vcc-5V, MISO-12, MOSI-11, SCK-13, CS-10
 * RGB R-9, GND+5V, G-6, B-5 **Remember Resistors 
 * Button (Opposite Corners)-> (7/GND) and (RST/GND)
*/
/*Serial monitor
 * In setup: "Serial.begin(115200);"
 * "Serial.println("Text");" 
*/

#define R 9
#define G 6
#define B 5
//#define BUTTON 7

SoftwareSerial txrx(3, 2); //(TX,RX) pins
Adafruit_GPS GPS(&txrx);
File gpsFile;

//byte mode = 0;      //Mode Tracker, 0 = Not Writing, 1 = Trying to Write
uint32_t timer = millis();
int counter = 0;    // Log counter
//int d = 300;        // Button Press Delay

void setColor(byte red, byte green, byte blue)
{
  analogWrite(R, 255-red); //Remove "255-" if com. cath.
  analogWrite(G, 255-green);
  analogWrite(B, 255-blue);  
}

void setup()  
{
  setColor(255,80,0);   // Solid Orange
  GPS.begin(9600);      //9600 Default for GPS TTL Serial
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz recommended for parsing and printing
  if (!SD.begin(10)) {
    while(true){          // Infinite Blinking -> Restart Module
    setColor(255,80,0);
    delay(150);
    setColor(0, 0, 0);
    delay(150);
    }
  }
  else
  {
    setColor(0,0,255);
  }
  //Setup Mode Button: Pulled to High, Pressed -> Low
  //pinMode(BUTTON, INPUT_PULLUP);
  //mode = 0;
  //mode = 1;
  Serial.begin(115200);
  Serial.println("Text");
}

void loop()
{
  char c = GPS.read(); // Raw NMEA sentences, if (c) Serial.print(c); - To print to console
  if (c) Serial.print(c);
  if (GPS.newNMEAreceived()) {  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // Parse Fail = Wait for another
  }
  if (millis() - timer > 1000) {  //Write every 1 second if data is available
    timer = millis(); // reset timer   
    if (GPS.fix) {
      setColor(0,255,0);  // Green: Writing
      counter++;
      //Log # | Time(H:M:S)| Lat&Long (Deg) | Speed (Km/h) | Altitude | # Satellites
      gpsFile = SD.open("gpsData.txt", FILE_WRITE);
      gpsFile.print(counter); gpsFile.print('|');
      gpsFile.print(GPS.hour, DEC); gpsFile.print(':');
      gpsFile.print(GPS.minute, DEC); gpsFile.print(':');
      gpsFile.print(GPS.seconds, DEC); gpsFile.print('|');
      gpsFile.print(GPS.latitudeDegrees, 6);gpsFile.print(", "); 
      gpsFile.print(GPS.longitudeDegrees, 6); gpsFile.print('|');
      gpsFile.print(GPS.speed*1.852); gpsFile.print('|');
      gpsFile.print(GPS.altitude); gpsFile.print('|');
      gpsFile.println((int)GPS.satellites);
      gpsFile.close();
    }
    else
      setColor(255,0,0);  //Red: No fix after start writing command
  }
}


