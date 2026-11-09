# ESP32 · Control de LED desde el teléfono

Proyecto que convierte un ESP32 en un pequeño servidor web. Desde el navegador del teléfono (conectado a la misma red WiFi) se puede encender y apagar un LED con un botón.

## Materiales

- ESP32 (Dev Module)
- 1 LED
- 1 resistencia de 220 Ω
- Cable USB con datos
- Protoboard y cables (opcional)

## Conexión

| ESP32   | Componente                      |
|---------|---------------------------------|
| GPIO 23 | Resistencia 220 Ω → LED (ánodo) |
| GND     | LED (cátodo)                    |

> Para usar el LED integrado de la placa, cambia `LED_PIN` a `2` en el código.

## Cómo usarlo

1. Instala el soporte para ESP32 en el Arduino IDE (paquete **esp32** de Espressif Systems).
2. Abre `esp32_led_web/esp32_led_web.ino`.
3. Cambia `TU_WIFI` y `TU_CLAVE` por los datos de tu red (2.4 GHz).
4. Selecciona la placa **ESP32 Dev Module** y el puerto COM correspondiente.
5. Sube el código. Si se queda en "Connecting...", mantén presionado el botón **BOOT**.
6. Abre el Monitor Serie a **115200 baudios** y copia la IP que aparece.
7. Con el teléfono en la misma red, abre `http://<IP>` en el navegador.

## Cómo funciona

El ESP32 se conecta a la red WiFi y levanta un servidor HTTP en el puerto 80 con tres rutas:

| Ruta       | Función                                   |
|------------|-------------------------------------------|
| `/`        | Página con el botón de control            |
| `/toggle`  | Cambia el estado del LED y devuelve 1 o 0 |
| `/estado`  | Devuelve el estado actual (1 o 0)         |

La página consulta `/estado` cada 3 segundos para mantener el botón sincronizado.

## Problemas comunes

- **No aparece el puerto COM:** instala el driver del chip USB (CP210x o CH340) y usa un cable que transmita datos.
- **"Wrong boot mode detected":** mantén presionado **BOOT** mientras sube el código.
- **No conecta a la WiFi:** verifica que sea de 2.4 GHz. Algunas redes institucionales aíslan los dispositivos; prueba con el hotspot del teléfono.

## Licencia

MIT
