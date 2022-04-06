#include <Ethernet.h>

//TODO set the clock
//TODO get the data every 5 minutes
//TODO save on SD In json format
//TODO transmit the json on a server

#include <SPI.h>
#include <dht_nonblocking.h>
#include <LiquidCrystal.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11

//TempHumidity server
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// assign an IP address for the controller:
IPAddress ip(192, 168, 1, 20);
// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);


float temperature;
float humidity;
  
//part related to read the senso
static const int DHT_SENSOR_PIN = 2; //pin of the sensor of temperature and humidity
const int IS_LCD_ON = 0;
const int TEMP_INTERVAL = 60000;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );
LiquidCrystal lcd (12,11,5,4,3,2);




/*
 * Initialize the serial port.
 */
void setup( )
{
  SPI.begin();
  // start the Ethernet connection
  Ethernet.begin(mac, ip);
  
  Serial.begin( 9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start listening for clients
  server.begin();
  //lcd setup
  lcd.begin(16,2);
}


void listenForEthernetClients() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("Got a client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          // print the current readings, in HTML format:
          client.print("Temperature: ");
          client.print(temperature);
          client.print(" degrees C");
          client.println("<br />");
          client.print("Humidity: " + String(humidity));
          client.print(" Pa");
          client.println("<br />");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}


/*
 * Poll for a measurement, keeping the state machine alive.  Returns
 * true if a measurement is available.
 */
static bool measure_environment( float *temperature, float *humidity )
{
    if(dht_sensor.measure( temperature, humidity ) == true)
    {
      return true ;
    }
  return false;
}


/**
 * Display lcd infos about temperature
 */
void displayLcdInfos(float temperature, float humidity) {
  if(IS_LCD_ON>0) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("T: ");
    lcd.print(temperature);
    lcd.print(" C");
    lcd.setCursor(0,1);
    lcd.print("H: ");
    lcd.print(humidity);
    lcd.print(" %");
  }
}

/*
 * Main program loop.
 */
void loop( )
{
  /* Measure temperature and humidity.  If the functions returns
     true, then a measurement is available. */
  if( measure_environment( &temperature, &humidity ) == true )
  {
    Serial.print( "For Serial >>>>" );
    Serial.print( "T = " );
    Serial.print( temperature, 1 );
    Serial.print( " deg. C, H = " );
    Serial.print( humidity, 1 );
    Serial.println( "%" );
    displayLcdInfos(temperature, humidity);
    delay(TEMP_INTERVAL);
  }
  listenForEthernetClients();
}
