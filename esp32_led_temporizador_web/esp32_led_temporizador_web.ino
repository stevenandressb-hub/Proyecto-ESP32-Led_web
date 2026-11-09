#include <WiFi.h>
#include <WebServer.h>

// ====== CONFIGURAR ======
const char* ssid     = "C.U.A";
const char* password = "C951u123a";
// ============================

const int LED_PIN = 23;                        
const unsigned long DURACION_MS = 10000;       

bool ledState = false;
unsigned long marcaEncendido = 0;              

WebServer server(80);

const char PAGINA[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Control LED</title>
<style>
  body { font-family: sans-serif; text-align: center; background: #111; color: #fff; padding-top: 50px; }
  #estado { font-size: 1.5rem; margin-bottom: 10px; }
  #cuenta { font-size: 1.2rem; margin-bottom: 30px; color: #ffb300; min-height: 1.5em; }
  button { font-size: 1.4rem; padding: 18px 36px; margin: 8px; border: none; border-radius: 16px;
           color: #fff; cursor: pointer; }
  #on  { background: #2e7d32; }
  #off { background: #c62828; }
</style>
</head>
<body>
  <h1>ESP32 · LED</h1>
  <div id="estado">Cargando...</div>
  <div id="cuenta"></div>
  <button id="on"  onclick="orden('/on')">ENCENDER</button>
  <button id="off" onclick="orden('/off')">APAGAR</button>

<script>
  function pintar(txt) {
    const [on, rest] = txt.split(',');
    const encendido = on === '1';
    document.getElementById('estado').textContent = encendido ? '💡 LED ENCENDIDO' : 'LED APAGADO';
    document.getElementById('cuenta').textContent = encendido ? ('Se apaga en ' + rest + ' s') : '';
  }
  function estado() { fetch('/estado').then(r => r.text()).then(pintar); }
  function orden(ruta) { fetch(ruta).then(r => r.text()).then(pintar); }
  estado();
  setInterval(estado, 1000);
</script>
</body>
</html>
)rawliteral";

// Segundos que faltan para el apagado automático (redondeado hacia arriba)
unsigned long segundosRestantes() {
  unsigned long transcurrido = millis() - marcaEncendido;
  if (transcurrido >= DURACION_MS) return 0;
  return (DURACION_MS - transcurrido + 999) / 1000;
}

String estadoTexto() {
  return String(ledState ? 1 : 0) + "," + String(ledState ? segundosRestantes() : 0);
}

void encender() {
  ledState = true;
  digitalWrite(LED_PIN, HIGH);
  marcaEncendido = millis();   // Cada orden Encender reinicia los 10 s
  Serial.printf("[%lu ms] LED ENCENDIDO (temporizador reiniciado)\n", marcaEncendido);
}

void apagar(const char* motivo) {
  ledState = false;
  digitalWrite(LED_PIN, LOW);
  Serial.printf("[%lu ms] LED APAGADO (%s)\n", millis(), motivo);
}

void handleRoot()   { server.send(200, "text/html", PAGINA); }
void handleEstado() { server.send(200, "text/plain", estadoTexto()); }
void handleOn()     { encender();           server.send(200, "text/plain", estadoTexto()); }
void handleOff()    { apagar("orden manual"); server.send(200, "text/plain", estadoTexto()); }

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado. Abre en tu teléfono: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/estado", handleEstado);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.begin();
}

void loop() {
  server.handleClient();   // Atiende al teléfono sin esperar

  // Revisión no bloqueante del temporizador: se evalúa en cada vuelta del loop
  if (ledState && (millis() - marcaEncendido >= DURACION_MS)) {
    apagar("temporizador de 10 s");
  }
}
