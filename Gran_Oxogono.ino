// Gran Oxógono - Medición de partículas volátiles (PM10, PM2.5)
// NOVIEMBRE 2020 - Silvia Teixeira @ Medialab-Prado
// Arduino UNO
// Sensor PM
// NeoPixel LED Strip, 1000uF capacitor, 470 Ohms Resistor
// 4 x AA Batteries + Zener Diode

#include <SoftwareSerial.h>
SoftwareSerial pmsSerial(2, 3);

#include <Adafruit_NeoPixel.h>
#define LED_PIN    6
#define LED_COUNT 400

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {

  Serial.begin(9600);
  pmsSerial.begin(9600);
   
  strip.begin();           
  strip.show();            
  strip.setBrightness(50);
}


struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};
struct pms5003data data;

void loop() {

  if (readPMSdata(&pmsSerial)) {
    // reading data was successful!
    Serial.println();
    Serial.println("---------------------------------------");
    Serial.println("Concentración de partículas (ug/m3)");
    Serial.print("PM 1.0: "); Serial.print(data.pm10_env);
    Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_env);
    Serial.print("\t\tPM 10: "); Serial.println(data.pm100_env);
    Serial.println("---------------------------------------");
//    Serial.print("Particles > 0.3um / 0.1L air:"); Serial.println(data.particles_03um);
//    Serial.print("Particles > 0.5um / 0.1L air:"); Serial.println(data.particles_05um);
//    Serial.print("Particles > 1.0um / 0.1L air:"); Serial.println(data.particles_10um);
//    Serial.print("Particles > 2.5um / 0.1L air:"); Serial.println(data.particles_25um);
//    Serial.print("Particles > 5.0um / 0.1L air:"); Serial.println(data.particles_50um);
//    Serial.print("Particles > 10.0 um / 0.1L air:"); Serial.println(data.particles_100um);
//    Serial.println("---------------------------------------");

    mostrarLEDS();

    delay(1000);
  }
}


void mostrarLEDS(){
  
  // VERDE OSCURO
  if(data.pm25_env < 5){
      for(int i=0;i<strip.numPixels();i++){ 
        strip.setPixelColor(i, strip.Color(0, 255, 0)); 
        strip.show(); 
        delay (1);
      }
  }

  // VERDE CLARO
   else if(data.pm25_env >= 5 && data.pm25_env < 10){
      for(int i=0;i<strip.numPixels();i++){ 
        strip.setPixelColor(i, strip.Color(100, 255, 0)); 
        strip.show(); 
        delay (1);
      }
  }
    // AMARILLO
     else if(data.pm25_env >= 10 && data.pm25_env < 15){
      for(int i=0;i<strip.numPixels();i++){ 
        strip.setPixelColor(i, strip.Color(255, 255, 0)); 
        strip.show(); 
        delay (1);
      }
  }
    // NARANJA
    else if(data.pm25_env >= 15 && data.pm25_env < 20){
          for(int i=0;i<strip.numPixels();i++){ 
            strip.setPixelColor(i, strip.Color(255, 200, 0)); 
            strip.show(); 
            delay (1);
          }
      }

      // MORADO
     else if(data.pm25_env >= 20 && data.pm25_env < 25){
          for(int i=0;i<strip.numPixels();i++){ 
            strip.setPixelColor(i, strip.Color(255, 0, 0)); 
            strip.show(); 
            delay (1);
          }
      }

      // ROJO
      else if(data.pm25_env >= 25){
          for(int i=0;i<strip.numPixels();i++){ 
            strip.setPixelColor(i, strip.Color(255, 0, 0)); 
            strip.show(); 
            delay (1);
          }
      }
      
    else {

       for(int i=0;i<strip.numPixels();i++){ 
            strip.setPixelColor(i, strip.Color(0, 0, 255)); 
            strip.show(); 
            delay (1);
      
      }
  }
}

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

  /* debugging
  for (uint8_t i=2; i<32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
  }
  Serial.println();
  */
  
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
