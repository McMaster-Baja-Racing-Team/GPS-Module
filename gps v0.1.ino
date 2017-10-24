#include <Adafruit_GPS.h>
#include <SoftwareSerial.h> //For GPS - Current IDE needs this library
#include <SPI.h>
#include <SD.h>

SoftwareSerial txrx(3, 2); //(TX,RX) pins on GPS module
Adafruit_GPS GPS(&txrx);
File gpsFile;
int R = 9;
int G = 6;
int B = 5;
int button = 8;
int bt = 3; //Button Tracker, 1 = Trying to Write, 2 = Rdy to Power off, 3 = SD Not Set Up/Failed
uint32_t timer = millis();
int counter = 0;
int d = 300;  // Button Press Delay

void setup()  
{
  setColor(255,80,0);
  //Setup GPS and Console
  Serial.begin(115200); // Begin Console, @ This speed, we can read GPS and echo
  GPS.begin(9600);      //9600 Default for GPS TTL Serial
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // Update rate can be changed to 1/5/10 Hz (max)| For another rate, check baja box docs (TODO)
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz recommended for parsing and printing
  if (!SD.begin(10)) {
    Serial.println("sd init failed!");
    // Infinite Blinking -> Restart
    while(true){
    setColor(255,80,0);
    delay(d/2);
    setColor(0, 0, 0);
    delay(d/2);
    }
  }
  else
  {
    Serial.println("sd init");
    setColor(0,0,255);
  }
  //Setup Button
  pinMode(button, INPUT_PULLUP);  //Pulled to High, Pressed -> Low
  bt = 0;
}

void setColor(int red, int green, int blue)
{
  analogWrite(R, 255-red); //Remove "255-" if com. cath.
  analogWrite(G, 255-green);
  analogWrite(B, 255-blue);  
}

void loop()
{
  if (digitalRead(button) == LOW)
  {
    if (bt==0)
      bt = 1;
    else
      bt = 0;
    Serial.println(bt);
    delay(d);
  }
  if (bt == 1)
  {
    char c = GPS.read(); // Raw NMEA sentences, if (c) Serial.print(c); - To print to console
    if (GPS.newNMEAreceived()) {  
      if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
        return;  // Parse Fail = Wait for another
    }
    
    // if millis() or timer overflows, reset it... takes approx 50 days, should be unnecessary
    //if (timer > millis())  timer = millis();
  
    // timer > #, # = write new data interval, currently every 3 seconds
    if (millis() - timer > 1000) { 
      timer = millis(); // reset timer   
      if (GPS.fix) {
        setColor(0,255,0);  //Set LED Green: Writing
        counter++;
        Serial.println (counter);
        gpsFile = SD.open("gpsData.txt", FILE_WRITE);
        //Time(H:M:S)| Lat&Long (Deg) | Speed | Altitude | Satellites
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
        setColor(255,0,0);  //Set LED Red: No fix after start writing command
    }
  }
  else
    setColor(0,0,255); //Set LED Blue: Button Pressed = Paused/Rdy to Shutdown 
}

/*Pinouts
 * GPS Vin-3V2, GND, RX-2, TX-3
 * SD GND, Vcc-5V, MISO-12, MOSI-11, SCK-13, CS-10
 * RGB R-9, GND+5V, G-6, B-5 **Remember Resistors 
 * Button (Opposite Corners)-> (8/GND)
*/
