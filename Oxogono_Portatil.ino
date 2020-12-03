// Oxógono Portátil - Medición de partículas volátiles (PM10, PM2.5)
// NOVIEMBRE 2020 - Silvia Teixeira @ Medialab-Prado
// Arduino MEGA
// Sensor PM, GPS, Lector SD
// NeoPixel LED Jewel

// ----------Setup
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h> 
// ----------Lector tarjeta SD
#include <SD.h>
// ---------- NeoPixel LED Ring

#include <Adafruit_NeoPixel.h>
// ----------GPS
#include <NMEAGPS.h>
#include <GPSport.h>
#include <Streamers.h>

// ------------Data Pin para el NeoPixel
#define PIN 49
// ------------Numero de LEDS
#define NUMPIXELS 7
// ------------Puerto Serie para sensor PM TX1:18, RX1:19 
#define pmsSerial Serial1

// -------------Definición de parametros de archivo
#define LOG_FILE_PREFIX "PMlog" // Nombre del archivo
#define MAX_LOG_FILES 100 // Número de archivos máximo
#define LOG_FILE_SUFFIX "csv" // Sufijo del archivo
char logFileName[13]; // String que almacena el nombre

#define LOG_COLUMN_COUNT 7 // Cuántos datos almacenamos

char * log_col_names[LOG_COLUMN_COUNT] = {
  "1um (ug/m3)", "2.5um (ug/m3)", "10um (ug/m3)", "Fecha", "Hora", "Latitud", "Longitud"
}; // log_col_names se imprime en la cabecera del archivo

File logFile;

// -------------Parámetros NeoPixel
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// intervalos para controlar los tiempos de lectura y escritura
// millis() da el tiempo en ms desde que la placa comenzó a ejecutar el programa

///////////---- SI VAS A USAR EL OXÓGONO EN UN VEHÍCULO RÁPIDO O QUIERES DEJARLO ESTÁTICO, 
///////////---- QUIZÁ TE INTERESE MODIFICAR ESTE INTERVALO.

unsigned long interval = 5000; // Intervalo de 5 s

unsigned long previousMillis = 0; // Almacena los intervalos

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


// Vemos los datos en el puerto serie
void ImprimeSerial()
{   
    if (gps.available( gpsPort )) 
    {
      // ¡el GPS envía señal correctamente!
      
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
      // ¡leemos los datos correctamente!
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
  }



void setup() {
  
  gpsPort.begin( 9600 ); //GPS
  Serial.begin(9600);   // Puerto Serie
  pmsSerial.begin(9600); // Sensor PM

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
  unsigned long currentMillis = millis(); //Inicia millis()
  if((currentMillis-previousMillis)>interval && fix.valid.location){ //Comprueba cada 5sg y que hay posicionamiento GPS
    GuardarPM(); // Guarda en la SD
    previousMillis=currentMillis; //Actualiza previousMillis (5sg,10sg,15sg,....)
  }

  // Enciende LEDS acorde a la cantidad de PM medida
  mostrarLEDS();
  
}


byte GuardarPM()
{
    logFile = SD.open(logFileName, FILE_WRITE);
    if(logFile)
    {
      logFile.print(data.pm10_env);
      logFile.print(';');
      logFile.print(data.pm25_env);
      logFile.print(';');
      logFile.print(data.pm100_env);
      logFile.print(';');
      logFile.print(fix.dateTime.date);
      logFile.print('/');
      logFile.print(fix.dateTime.month);
      logFile.print('/');
      logFile.print(fix.dateTime.year);
      logFile.print(';');
      logFile.print(fix.dateTime.hours);
      logFile.print(':');
      logFile.print(fix.dateTime.minutes);
      logFile.print(';');
      logFile.print(fix.latitude(), 6);
      logFile.print(';');
      logFile.print(fix.longitude(), 6);
      logFile.println();
      logFile.close();
      return 1;
    }
    return 0;
 }

void mostrarLEDS(){

  //verde oscuro
  if(data.pm25_env < 5){
        for(int i=0;i<NUMPIXELS;i++){ // encience un pixel de cada vez, excepto si delay (0);
          pixels.setPixelColor(i, pixels.Color(0, 20, 0)); 
          pixels.show(); // Envía el nuevo color de pixel
          //delay (0);
        }
    }
  
  //verde claro
  else if(10 > data.pm25_env >= 5){
        for(int i=0;i<NUMPIXELS;i++){
          pixels.setPixelColor(i, pixels.Color(10, 60, 0)); 
          pixels.show(); 
          //delay (0);
        }
    }
  
  //amarillo
  else if(data.pm25_env  >= 10 && data.pm25_env < 15){
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(60, 60, 0)); 
        pixels.show(); 
        //delay (0);
      }
  }
  //naranja
  else if(data.pm25_env >= 15 && data.pm25_env < 20){
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(60, 20, 0));
        pixels.show(); 
        //delay (0);
      }
  }
  // rojo
  else if(data.pm25_env >= 25){    
        for(int i=0;i<NUMPIXELS;i++){
          pixels.setPixelColor(i, pixels.Color(20, 0, 0));
          pixels.show(); 
          //delay (0);
      }
  }
}
  
void printHeader()
{
  logFile = SD.open(logFileName, FILE_WRITE); // Abre el archivo

  if (logFile) // Si el archivo se ha abierto correctamente, añade los valores a las columnas
  {
    for (int i=0; i < LOG_COLUMN_COUNT; i++)
    {
      logFile.print(log_col_names[i]);
      if (i < LOG_COLUMN_COUNT - 1) // Si no es la última columna
        logFile.print(';'); // separador, PUNTO Y COMA
      else // Si es la última columna
        logFile.println(); 
    }
    logFile.close(); // cierra el archivo
  }
}

void updateFileName()
{
  int i = 0;
  for (; i < MAX_LOG_FILES; i++)
  {
    memset(logFileName, 0, strlen(logFileName)); 
    // Cambia el logFileName a "PMlogXX.csv":
    sprintf(logFileName, "%s%d.%s", LOG_FILE_PREFIX, i, LOG_FILE_SUFFIX);
    if (!SD.exists(logFileName)) 
    {
      break; 
    }
    else 
    {
      Serial.print(logFileName);
      Serial.println(" exists");
    }
  }
  Serial.print("File name: ");
  Serial.println(logFileName); 
}

//Necesario para darle formato a los datos del sensor
boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }
  
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }

  if (s->available() < 32) {
    return false;
  }
    
  uint8_t buffer[32];    
  uint16_t sum = 0;
  s->readBytes(buffer, 32);

  for (uint8_t i=0; i<30; i++) {
    sum += buffer[i];
  }

  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }
  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  return true;
}
