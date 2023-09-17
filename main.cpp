#include <WiFi.h>
#include <DHT.h>

// Configuración del sensor DHT22
#define DHT_PIN 4       // Pin al que está conectado el sensor
#define DHT_TYPE DHT22  // Tipo de sensor (DHT11, DHT22, AM2302)

DHT dht(DHT_PIN, DHT_TYPE);
 
const char* ssid = "JCTORRESO";
const char* password =  "3167732449";
const char* host = "dweet.io";


String getTipoCifrado(wifi_auth_mode_t tipoCifrado) {
 
  switch (tipoCifrado) {
    case (WIFI_AUTH_OPEN):
      return "Open";
    case (WIFI_AUTH_WEP):
      return "WEP";
    case (WIFI_AUTH_WPA_PSK):
      return "WPA_PSK";
    case (WIFI_AUTH_WPA2_PSK):
      return "WPA2_PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
      return "WPA_WPA2_PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
      return "WPA2_ENTERPRISE";
  }
}
 
void scanNetworks() {
 
  int numberOfNetworks = WiFi.scanNetworks();
 
  Serial.print("Numero de redes encontradas: ");
  Serial.println(numberOfNetworks);
 
  for (int i = 0; i < numberOfNetworks; i++) {
 
    Serial.print("Nombre de red: ");
    Serial.println(WiFi.SSID(i));
 
    Serial.print("Fuerza de la señal: ");
    Serial.println(WiFi.RSSI(i));
 
    Serial.print("Direccion MAC: ");
    Serial.println(WiFi.BSSIDstr(i));
 
    Serial.print("Tipo de cifrado: ");
    String tipoCifradoDescription = getTipoCifrado(WiFi.encryptionType(i));
    Serial.println(tipoCifradoDescription);
    Serial.println("-----------------------");
 
  }
}
 
void connectToNetwork() {
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Estableciendo conexion WiFi..");
  }
 
  Serial.println("Conectado a la red");
 
}
 
void setup() {
 
  Serial.begin(9600);
 
  scanNetworks(); //Escanear las redes WiFi disponibles
  connectToNetwork();  //Conectarse a la red que especificamos arriba
 
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
 
//  WiFi.disconnect(true);
Serial.println(WiFi.localIP());
// Inicializa el sensor DHT22
dht.begin();
 
}
 
void loop() {

  // Lee la humedad y la temperatura desde el sensor DHT22
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Comprueba si la lectura del sensor fue exitosa
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Error al leer el sensor DHT22");
    return;
  }
  //inicia conexion al servicio dweet
   Serial.println("Conectandose a:");
   Serial.println(host);

   WiFiClient client;

   const int httpPort = 80;
   
   if(!client.connect(host, httpPort)){
     Serial.println("Conexion Fallida");
     delay(1000);
     return;
   }
  String message = "{";
    message += "\"Humidity\":" + String(humidity) + ",";
    message += "\"Temperature\":" + String(temperature) + "}";
  //String cadena = "hola"+String("mundo")+String(variable);
  //String url = "/dweet/for/uceva08?temperatura=23.5&humedad=80&indice=20";

  //Se pueden leer los dweets en  https://dweet.io/get/latest/dweet/for/uceva08

  Serial.print("Solicitando el recurso: ");
  Serial.println(message);

  client.print(String("GET ") + message + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" 
                    + "Connection: close\r\n\r\n");
  unsigned long timeout = millis(); //Leo la hora
  while(client.available() == 0){   //Mientras hayan cero letras disponibles...
      if(millis() - timeout > 5000){ //Resto la hora actual a la hora inicial, si es mas de 5s
        Serial.println("Expiro el tiempo de espera");
        client.stop();
        return;
      }
  }

  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.println(line);
  }
  
  Serial.println("Fin de conexion");
  client.stop();
  delay(20000);
}