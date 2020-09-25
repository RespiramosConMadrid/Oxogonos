# Los Oxógonos

Los Oxógonos son nuestros medidores. Hemos desarrollado su diseño, programación y fabricación a través de procesos colaborativos que han tenido lugar en Medialab Prado durante el transcurso del proyecto. Los Oxógonos son el fruto del pensamiento colectivo, la energía y el saber hacer de muchas personas que se han involucrado en el proceso, provenientes de distintos contextos, saberes, edades, lugares y momentos. Los Oxógonos respiran, están vivos y abiertos a la experimentación; son libres, como su código. 

Puedes llevarte un Oxógono portátil en tus paseos por la ciudad, en tus rutas en bicicleta o cuando salgas a correr. Puedes tomar uno prestado en Medialab o fabricarte el tuyo propio. 

# Partes

- Arduino MEGA 2560 https://store.arduino.cc/arduino-mega-2560-rev3
- Sensor de Partículas Volátiles: PMS5003 https://www.adafruit.com/product/3686
- Adaptardor de tarjeta Micro SD https://es.aliexpress.com/item/32907897214.html
- Módulo GNSS, GPS, GLONASS GG-1802 https://es.aliexpress.com/item/32851353457.html
- Pantalla LCD
- NeoPixel LED Ring

# Datos
Los archivos generados tienen el siguiente formato:

|| 1um (ug/m3) || 2.5um (ug/m3) || 10um (ug/m3) || Fecha || Hora || Latitud || Longitud || Altitud (GPS) || Satelites 

- || 1um (ug/m3) || 2.5um (ug/m3) || 10um (ug/m3) || corresponden a los datos en ug/m3 de las partículas volátiles de diferentes diámetros.
- || Fecha || Hora ||
- || Latitud || Longitud || Localización GPS
- || Altitud (GPS) || Altitud calculada por el GPS
- || Satelites || Número de satélites disponibles involucrados en los datos recibidos
