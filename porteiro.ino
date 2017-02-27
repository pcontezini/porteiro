
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // PUT YOUR LOCAL MAC ADDRESS HERE, DIFFERENT FROM EVERY OTHER DEVICE ON THE NETWORK
IPAddress ip(0,0,0,0); // PUT YOUR LOCAL IP ADDRESS HERE
int port = 80;

String http_request;

EthernetServer server(80);

void setup() {
  Serial.begin(9600);
  for(int i = 0; i < 4; i++) { // how many ports you want to enable to relay? define here.
    pinMode(i, OUTPUT);
  }
  Ethernet.begin(mac, ip);
  Serial.println(Ethernet.localIP());
  server.begin();

}

void abrir_porta(int porta) {
  digitalWrite(porta, HIGH);
  delay(1000);
  digitalWrite(porta, LOW);
  delay(1000);
}

void enviar_pagina_porteiro(EthernetClient client) {
client.println("<!DOCTYPE html>");
client.println("<html>");
client.println("<head>");
client.println("<title>DOORMAN</title>");
client.println("<meta charset=\"utf-8\">");
client.println("<meta http-equiv=\"cache-control\" content=\"no-cache\">");
client.println("<meta name=\"viewport\" content=\"width=device-width\">");
client.println("<style type=\"text/css\">");
client.println("#botao {");
client.println("    width: 100%;  height: 100%;");
client.println("    font-size: xx-large;");
client.println("}");
client.println("</style>");
client.println("<script>");
client.println("function PB(o) {");
client.println("var request = new XMLHttpRequest();");
client.println("request.onreadystatechange = function() {");
client.println("	if (this.readyState == 4) {");
client.println("            if (this.status == 200) {");
client.println("               if (this.responseText != null) {");
client.println("               		window.location.reload(true);");
client.println("                    }");
client.println("		}");
client.println("	    }");
client.println("	}");
client.println("request.open(\"GET\", \"output/\" + o, true);");
client.println("request.send(null);");
client.println("}");
client.println("</script>");
client.println("</head>");
client.println("<body>");
client.println("<h1 align=\"center\">DOORMAN</h1>");
client.println("<p align=\"center\">");
client.println("<button type=\"button\" onclick=\"PB(0);\"  id=\"botao\">DOOR ZERO</button></p><p align=\"center\">");
client.println("<button type=\"button\" onclick=\"PB(1);\"  id=\"botao\">DOOR ONE</button></p><p align=\"center\">");
client.println("<button type=\"button\" onclick=\"PB(2);\"  id=\"botao\">DOOR TWO</button></p><p align=\"center\">");
client.println("<button type=\"button\" onclick=\"PB(3);\"  id=\"botao\">DOOR THREE</button></p><p align=\"center\">");
client.println("<br>");
client.println("</p>");
client.println("</body>");
client.println("</html>");
}

void loop()
{  
  EthernetClient client = server.available();
    if (client) {
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                http_request += c;
                if (c == '\n' && currentLineIsBlank) {
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: keep-alive");
                    client.println();
                    Serial.print(http_request);
                    int offset = http_request.indexOf("/output/");
                    int offset2 = http_request.indexOf(" ",offset+8);
                    Serial.print("ACHOU PORTA INICIO: ");
                    Serial.println(offset);
                    Serial.print("ACHOU PORTA FIM: ");
                    Serial.println(offset2);
                    if (offset > -1) { // eh uma chamada de output
                        int porta = http_request.substring(offset+8,offset+8+offset2).toInt();
                        Serial.print("PORTA=");
                        Serial.println(porta);
                        abrir_porta(porta);
                        client.println("OK"); 
                    }
                    else {  // HTTP request for web page
                      enviar_pagina_porteiro(client);
                    }
                    // display received HTTP request on serial port
                    //Serial.print(http_request);
                    http_request = "";            // finished with request, empty string
                    break;
                }
                if (c == '\n') {
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}

