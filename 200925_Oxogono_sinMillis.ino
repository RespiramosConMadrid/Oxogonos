// Oxógono - Medición de partículas volátiles (PM10, PM2.5)
// SEPTIEMBRE 2020 - Silvia Teixeira @ Medialab-Prado
// Arduino MEGA
// Sensor PM, GPS, Lector SD
// Pantalla LCD, NeoPixel LED Ring

// ----------Setup
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h> 
// ----------Lector tarjeta SD
#include <SD.h>
// ----------Pantalla LCD y NeoPixel LED Ring
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
// ----------GPS
#include <NMEAGPS.h>
#include <GPSport.h>
#include <Streamers.h>

// ------------Data Pin para el NeoPixel
#define PIN 49
// ------------Numero de LEDS
#define NUMPIXELS 8
// ------------Puerto Serie para sensor PM TX1:18, RX1:19 
#define pmsSerial Serial1

// -------------Definición de parametros de archivo
#define LOG_FILE_PREFIX "PMlog" // Name of the log file.
#define MAX_LOG_FILES 100 // Number of log files that can be made
#define LOG_FILE_SUFFIX "csv" // Suffix of the log file
char logFileName[13]; // Char string to store the log file name
// Data to be logged:
#define LOG_COLUMN_COUNT 12

char * log_col_names[LOG_COLUMN_COUNT] = {
  "1um (ug/m3)", "2.5um (ug/m3)", "10um (ug/m3)","1um", "2.5um", "10um", "Fecha", "Hora", "Latitud", "Longitud","Altitud (GPS)", "Satelites"
}; // log_col_names is printed at the top of the file.

File logFile;

// -------------Parametros LCD 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27,20,4);  
// -------------Parametros NeoPixel
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// intervalos para controlar los tiempos de lectura y escritura
// millis() da el tiempo en ms desde que la placa comenzó a ejecutar el programa
unsigned long interval = 1000;
unsigned long intervalLCD = 2000;

unsigned long previousMillis = 0;
unsigned long previousMillisGPS = 0;
unsigned long previousMillisLCD = 0;
unsigned long previousMillisLCD2 = 0;

// Pin del lector de tarjeta SD
const int ChipSelect=10;

// Setup de GPS
static NMEAGPS  gps;
static gps_fix  fix;

// Estructura de los datos que mide el sensor PM
struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

struct pms5003data data;


void ImprimeSerial()
{   
    if (gps.available( gpsPort )) 
    {
      fix = gps.read();
      Serial.print("Fecha:");
      Serial.print(fix.dateTime.year);
      Serial.print('/');
      Serial.print(fix.dateTime.month);
      Serial.print('/');
      Serial.print(fix.dateTime.date);
      Serial.println();
      Serial.print("Hora (UTC):");
      Serial.print(fix.dateTime.hours);
      Serial.print(':');
      Serial.print(fix.dateTime.minutes);
      Serial.println();
      Serial.print("Latitud,Longitud:");
      Serial.print(fix.latitudeL());
      Serial.print(',');
      Serial.print(fix.longitudeL());
      Serial.println();
      Serial.print("Altitud (float m):");
      Serial.println(fix.altitude());
      Serial.print("Satélites:");
      Serial.println(fix.satellites);
    
      Serial.println();
    }

    if (readPMSdata(&pmsSerial)) 
    {
      // reading data was successful!
      Serial.println();
      Serial.println("---------------------------------------");
      Serial.println("Unidades de Concentration (ug/m3)");
      Serial.print("PM 1.0: "); Serial.print(data.pm10_env);
      Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_env);
      Serial.print("\t\tPM 10: "); Serial.println(data.pm100_env);
      Serial.println("---------------------------------------");
      Serial.print("Particulas > 0.3um / 0.1L aire:"); Serial.println(data.particles_03um);
      Serial.print("Particulas > 2.5um / 0.1L aire:"); Serial.println(data.particles_25um);
      Serial.print("Particulas > 10.0 um / 0.1L aire:"); Serial.println(data.particles_100um);
      Serial.println("---------------------------------------");
     }
     //previousMillis = currentMillis;
   // }
  }



void setup() {
  
  gpsPort.begin( 9600 ); //GPS
  Serial.begin(9600);   // Puerto Serie
  pmsSerial.begin(9600); // Sensor PM

  //------ Inicializa pantalla LCD
  lcd.init();
  lcd.backlight();
  // ------ Inicializa LEDS
  pixels.begin();

  // --- Comprueba que la SD está activa
  if (!SD.begin(ChipSelect))
  {
    Serial.println("Error iniciando la tarjeta SD.");
    return;
  }
  Serial.println("Tarjeta SD iniciada.");

  // ------ Función que actualiza el nombre de archivo
  updateFileName(); 
  // ------ Función que imprime una cabecera de archivo nueva
  printHeader();

  //-------- Abre el archivo
  logFile=SD.open("nombreArchivo", FILE_WRITE);

  // ------- Si el archivo se abre, escribe la cabecera
  if (logFile) 
  {
    Serial.println("Open File");
    logFile.println("Particle Concentration");
  }

  // cierra el archivo
  logFile.close();
}




void loop() {

  while (gps.available( gpsPort )) {
    fix = gps.read();
  }

  // Imprime por puerto serie los valores para comprobación
  ImprimeSerial();
  // Guarda los datos en la tarjeta uSD
  GuardarPM();
  // Muestra los datos por pantalla
  mostrarLCD();
  // Enciende LEDS acorde a la cantidad de PM medida
  mostrarLEDS();
  
}


byte GuardarPM()
{
    logFile = SD.open(logFileName, FILE_WRITE);
    if(logFile)
    {
      logFile.print(data.pm10_env);
      logFile.print(',');
      logFile.print(data.pm25_env);
      logFile.print(',');
      logFile.print(data.pm100_env);
      logFile.print(',');
      logFile.print(data.particles_03um);
      logFile.print(',');
      logFile.print(data.particles_25um);
      logFile.print(',');
      logFile.print(data.particles_100um);
      logFile.print(',');
      logFile.print(fix.dateTime.date);
      logFile.print('/');
      logFile.print(fix.dateTime.month);
      logFile.print('/');
      logFile.print(fix.dateTime.year);
      logFile.print(',');
      logFile.print(fix.dateTime.hours);
      logFile.print(':');
      logFile.print(fix.dateTime.minutes);
      logFile.print(',');
      logFile.print(fix.latitudeL());
      logFile.print(',');
      logFile.print(fix.longitudeL());
      logFile.print(',');
      logFile.print(fix.altitude());
      logFile.print(',');
      logFile.print(fix.satellites);
      logFile.println();
      logFile.close();
      return 1;
    }
    return 0;
 }

void mostrarLCD(){

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("PM 2.5: "); 
      lcd.setCursor(8,0);
      lcd.print(data.pm25_env );
      lcd.setCursor(10,0);
      lcd.print("ug/m3"); 
      lcd.setCursor(0,1);
      lcd.print("PM 10: "); 
      lcd.setCursor(7,1);
      lcd.println(data.pm100_env );
      lcd.setCursor(9,1);
      lcd.print(" ug/m3");
 }

void mostrarLEDS(){

  if(data.pm25_env && data.pm100_env < 10){
      for(int i=0;i<NUMPIXELS;i++){ // lights up one pixel at a time, unless delay (0);
        pixels.setPixelColor(i, pixels.Color(0, 20, 0)); // green
        pixels.show(); // This sends the updated pixel color to the hardware.
        //delay (0);
      }
  }
 else if(data.pm25_env && data.pm100_env >= 10 && data.pm10_env && data.pm100_env <= 20){
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(60, 20, 0)); // orange
        pixels.show(); 
        //delay (0);
      }
  }
  else if(data.pm25_env && data.pm100_env >= 20){
  
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(20, 0, 0)); // red
        pixels.show(); 
        //delay (0);
      }
  }
}
  
void printHeader()
{
  logFile = SD.open(logFileName, FILE_WRITE); // Open the log file

  if (logFile) // If the log file opened, print our column names to the file
  {
    for (int i=0; i < LOG_COLUMN_COUNT; i++)
    {
      logFile.print(log_col_names[i]);
      if (i < LOG_COLUMN_COUNT - 1) // If it's anything but the last column
        logFile.print(','); // print a comma
      else // If it's the last column
        logFile.println(); // print a new line
    }
    logFile.close(); // close the file
  }
}

void updateFileName()
{
  int i = 0;
  for (; i < MAX_LOG_FILES; i++)
  {
    memset(logFileName, 0, strlen(logFileName)); // Clear logFileName string
    // Set logFileName to "PMlogXX.csv":
    sprintf(logFileName, "%s%d.%s", LOG_FILE_PREFIX, i, LOG_FILE_SUFFIX);
    if (!SD.exists(logFileName)) // If a file doesn't exist
    {
      break; // Break out of this loop. We found our index
    }
    else // Otherwise:
    {
      Serial.print(logFileName);
      Serial.println(" exists"); // Print a debug statement
    }
  }
  Serial.print("File name: ");
  Serial.println(logFileName); // Debug print the file name
}


boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }
  
  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }

  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }
    
  uint8_t buffer[32];    
  uint16_t sum = 0;
  s->readBytes(buffer, 32);

  // get checksum ready
  for (uint8_t i=0; i<30; i++) {
    sum += buffer[i];
  }

  
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }

  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}

