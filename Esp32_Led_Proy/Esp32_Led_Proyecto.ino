#include <WiFi.h>
#include <WebServer.h>

// ====== CONFIGURA ESTO ======
const char* ssid     = "C.U.A";
const char* password = "C951u123a";
// ============================

const int LED_PIN = 23;   // GPI23  = LED integrado en la mayoría de ESP32 (o conecta uno externo)
bool ledState = false;

WebServer server(80);

const char PAGINA[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Control LED</title>
<style>
  body { font-family: sans-serif; text-align: center; background: #111; color: #fff; padding-top: 60px; }
  #estado { font-size: 1.5rem; margin-bottom: 30px; }
  button { font-size: 1.6rem; padding: 20px 50px; border: none; border-radius: 16px;
           color: #fff; background: #2e7d32; cursor: pointer; }
  button.on { background: #c62828; }
</style>
</head>
<body>
  <h1>ESP32 · LED</h1>
  <div id="estado">Cargando...</div>
  <button id="btn" onclick="toggle()">...</button>

<script>
  function pintar(on) {
    document.getElementById('estado').textContent = on ? '💡 LED ENCENDIDO' : 'LED APAGADO';
    const b = document.getElementById('btn');
    b.textContent = on ? 'APAGAR' : 'ENCENDER';
    b.className = on ? 'on' : '';
  }
  function estado() {
    fetch('/estado').then(r => r.text()).then(t => pintar(t === '1'));
  }
  function toggle() {
    fetch('/toggle').then(r => r.text()).then(t => pintar(t === '1'));
  }
  estado();
  setInterval(estado, 3000);
</script>
</body>
</html>
)rawliteral";

void handleRoot()   { server.send(200, "text/html", PAGINA); }
void handleEstado() { server.send(200, "text/plain", ledState ? "1" : "0"); }

void handleToggle() {
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState ? HIGH : LOW);
  server.send(200, "text/plain", ledState ? "1" : "0");
}

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
  server.on("/toggle", handleToggle);
  server.begin();
}

void loop() {
  server.handleClient();
}
