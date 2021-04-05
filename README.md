# Los Oxógonos

Los Oxógonos son nuestros medidores. Hemos desarrollado su diseño, programación y fabricación a través de procesos colaborativos que han tenido lugar en Medialab Prado durante el transcurso del proyecto. Los Oxógonos son el fruto del pensamiento colectivo, la energía y el saber hacer de muchas personas que se han involucrado en el proceso, provenientes de distintos contextos, saberes, edades, lugares y momentos. Los Oxógonos respiran, están vivos y abiertos a la experimentación; son libres, como su código. 

Puedes llevarte un Oxógono portátil en tus paseos por la ciudad, en tus rutas en bicicleta o cuando salgas a correr. Puedes tomar uno prestado en Medialab o fabricarte el tuyo propio utilizando nuestros códigos y modelos para impresión 3D y corte láser. 

## Oxógono Portátil

Este es nuestro medidor portátil. Con él podremos pasear por la ciudad y ver la contaminación a la que nos hemos visto expuestas volcando los datos en la página web.

- Oxógono_Portatil.ino
- Oxogono_Portatil_Carcasa.stl
- Oxogono_Portatil_Corte_metacrilato.dxf

(Ambos diseños de corte laser e impresión 3D han sido desarrollados por Jorge Ruiz @Medialab-Prado)

- Arduino MEGA 2560 https://store.arduino.cc/arduino-mega-2560-rev3
- Sensor de Partículas Volátiles: PMS5003 https://www.adafruit.com/product/3686
- Adaptardor de tarjeta Micro SD https://es.aliexpress.com/item/32907897214.html
- Módulo GNSS, GPS, GLONASS GG-1802 https://es.aliexpress.com/item/32851353457.html
- NeoPixel Jewel LED

### Datos
Los archivos generados tienen el siguiente formato:

|| 1um (ug/m3) || 2.5um (ug/m3) || 10um (ug/m3) || Fecha || Hora || Latitud || Longitud || Altitud (GPS) || Satelites 

- || 1um (ug/m3) || 2.5um (ug/m3) || 10um (ug/m3) || corresponden a los datos en ug/m3 de las partículas volátiles de diferentes diámetros.
- || Fecha || Hora ||
- || Latitud || Longitud || Localización GPS
- || Altitud (GPS) || Altitud calculada por el GPS
- || Satelites || Número de satélites disponibles involucrados en los datos recibidos

## Gran Oxógono

El Gran Oxógono es un medidor fijo, que visualiza con luz y color cómo de amable es el aire que respiramos en ese lugar. Hemos construido el prototipo en Medialab Prado, a través de talleres colaborativos y en colaboración con UMBRAL: https://www.medialab-prado.es/actividades/taller-de-construccion-de-oxogonos-medidores-de-la-calidad-del-aire-creando-el 

- Gran_Oxogono.ino

- Arduino UNO
- Sensor de Partículas Volátiles: PMS5003 https://www.adafruit.com/product/3686
- NeoPixel LED Strip
- Condensador de 1000 uF
- Resistencia 470 Ohms
- 4 x Pilas AA
- Diodo Zener

[Respiramos_con_Madrid_10MB.pdf](https://github.com/RespiramosConMadrid/Oxogonos/files/6259516/Respiramos_con_Madrid_10MB.pdf)
<img width="650" alt="Captura de pantalla 2021-03-31 a las 11 42 42" src="https://user-images.githubusercontent.com/22861753/113603558-63214780-9644-11eb-9a80-3428e09671dc.png">
<img width="650" alt="Captura de pantalla 2021-03-31 a las 11 42 50" src="https://user-images.githubusercontent.com/22861753/113603591-6ae0ec00-9644-11eb-80ee-8bd18ca8b562.png">
