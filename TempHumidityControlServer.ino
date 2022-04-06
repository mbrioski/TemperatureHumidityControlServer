#include <Ethernet.h>
#include <SPI.h>
#include <dht_nonblocking.h>
//#include <time.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11

//TempHumidity server
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 178);
// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);


float temperature;
float humidity;
  
//part related to read the senso
static const int DHT_SENSOR_PIN = 2; //pin of the sensor of temperature and humidity
const int IS_LCD_ON = 0;
const int TEMP_INTERVAL = 1000;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );


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
}


void listenForEthernetClients() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        boolean currentLineIsBlank = true;
        char c = client.read();
        if (c == '\n' && currentLineIsBlank) {
           //time_t rawtime;
           //struct tm * timeinfo;
           //time ( &rawtime );
           //timeinfo = localtime ( &rawtime );
           //client.println("HTTP/1.1 200 OK");
           //client.println("Content-Type: application/json");
           //client.println("{");
           //client.println("\"temperature: \"" + String(temperature) + ',');
           //client.println("\"humidity: \"" + String(humidity));
           //client.println("\"datetime: \"" + String(asctime (timeinfo)));
           //client.println("}");
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
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
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


/*
 * Main program loop.
 */
void loop( )
{
  if( measure_environment( &temperature, &humidity ) == true )
  {
    Serial.print( "For Serial >>>>" );
    Serial.print( "T = " );
    Serial.print( temperature, 1 );
    Serial.print( " deg. C, H = " );
    Serial.print( humidity, 1 );
    Serial.println( "%" );
    delay(TEMP_INTERVAL);
  }
  listenForEthernetClients();
}
