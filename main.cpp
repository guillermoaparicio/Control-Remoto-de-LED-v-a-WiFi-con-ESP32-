//Importa la librería para manejar conexiones WiFi
#include <WiFi.h> 

//Almacenan las credenciales de la red WiFi
const char* ssid = "WiFiclient";
const char* password = "pasword";

//Asigna el pin GPIO5 para controlar el LED
const int ledPin = 5;

//Crea un servidor web en el puerto 80 (HTTP)
WiFiServer server(80); //se almacena el servidor en el puerto 80

void setup() {
  Serial.begin(115200); //Inicia la comunicación serial a 115200 baudios para poder enviar mensajes de depuración al monitor serial del IDE Arduino.

  pinMode(ledPin, OUTPUT);//Establece el pin del LED (definido previamente como GPIO5) como salida digital.
  digitalWrite(ledPin, LOW);//Pone el pin en estado LOW (0V) para asegurar que el LED comience apagado.

  WiFi.begin(ssid, password);//Intenta conectar a la red WiFi usando las credenciales (SSID y contraseña) predefinidas.
  Serial.print("Conectando a WiFi");//Muestra "Conectando a WiFi" en el monitor serial.
  while (WiFi.status() != WL_CONNECTED) { //Bucle while que la conexion WiFi se establezca correctamente
    delay(1000);//Espera 1 segundo (delay(1000))
    Serial.print(".");//Muestra puntos ".." para indicar progreso
  }
  
  Serial.println("\nConectado a WiFi");//Mensaje de confirmación ("Conectado a WiFi")
  Serial.println("IP local: ");
  Serial.println(WiFi.localIP());//La dirección IP local asignada al ESP32 en la red

  server.begin();//Inicia el servidor en el puerto 80
  if (WiFi.status() == WL_CONNECTED) {
  Serial.println("Servidor iniciado."); //mensaje del servido de incio
  Serial.print("Accedé desde navegador: http://");//mensaje indicacitivo sobre de que va acceder
  Serial.println(WiFi.localIP());//accede desde el navegador
  }
}

void loop() {
  //Verifica si hay un usuario intentando conectarse al servidor web del ESP32s
  WiFiClient client = server.available();//detecta conexion entrande en el port80
  if (client) {//si available devolvio un objeto WiFiClient=conexion enlazada
    Serial.println("Cliente conectado");//se imprime cliente conectado

    String request = ""; //declaracion de variable tipo string para LED=ON/OFF
    unsigned long timeout = millis();//tiempo maximo de espera 
    
    //verifica si user sigue conectador comprobando que haya pasado 1 segundo
    while (client.connected() && millis() - timeout < 1000) {
      if (client.available()) {//verifica si hay datos disponibles del cliente
        request = client.readStringUntil('\r');//lee todo hasta encontrar '/r'
        Serial.println("Petición: " + request);//Imprime peticion del monitor serial
        break;//salida del bule
      }
    }
    //limpia el buffer del cliente
    client.flush();

    // Comandos LED
    if (request.indexOf("/LED=ON") != -1) {
      digitalWrite(ledPin, HIGH);//Enciende el LED
    } else if (request.indexOf("/LED=OFF") != -1) {
      digitalWrite(ledPin, LOW);//Apaga el LED
    }

    // Respuesta HTTP
    client.println("HTTP/1.1 200 OK");//indica que la solicitud fue exitosa
    client.println("Content-Type: text/html");//indica que el contenido será HTML
    client.println("Connection: close"); //Evita almacenamiento en caché
    client.println();

    // Respuesta HTTP con CSS embebido
    client.println("<!DOCTYPE html><html><head>");//Inicicia el documento HTML
    client.println("<title>ESP32 LED</title>");//Muestra ESP32 LED"

    //Estilos CSS
    client.println("<style>");
    client.println("body { font-family: Arial; text-align: center; background-color: #f0f0f0; }");
    client.println("h1 { color: #333; }");
    client.println("button { padding: 15px 30px; font-size: 16px; margin: 10px;");
    client.println("background-color: #4CAF50; color: white; border: none; border-radius: 8px; cursor: pointer; }");
    client.println("button:hover { background-color: #45a049; }"); 
    client.println("a { text-decoration: none; }");

    //Cuerpo de la pagina
    client.println("</style>");
    client.println("</head><body>");
    client.println("<h1>Control de LED</h1>");//Titulo control de LED
    client.println("<a href=\"/LED=ON\"><button>Encender</button></a>");
    client.println("<a href=\"/LED=OFF\"><button>Apagar</button></a>");
    client.println("</body></html>");

    delay(1);
    client.stop();
    Serial.println("Cliente desconectado");
  }
}
