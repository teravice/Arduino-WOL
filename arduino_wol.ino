#include <SPI.h>
#include <UIPEthernet.h>


// network configuration.  gateway and subnet are optional.

 // the media access control (ethernet hardware) address for the shield:
byte mac[] = { 0x00, 0x00, 0x0B, 0xAA, 0xBB, 0xCC };  
//the IP address for the shield:
//byte ip[] = { 192, 168, 1, 10 };    
// the router's gateway address:
//byte gateway[] = { 192, 168, 1, 1 };
// the subnet:
//byte subnet[] = { 255, 255, 255, 0 };

// telnet defaults to port 23
EthernetServer server = EthernetServer(23);

//The broadcast IP of our network
byte broadCastIp[] = { 255, 255, 255, 255 };
//Mac address of the machine we want to WOL
byte remote_MAC_ADD[] = { 0x00, 0x25, 0x22, 0xD8, 0x16, 0xD1 };
//UDP WOL port
int wolPort = 9;
// whether or not you got a message from the client yet
boolean gotAMessage = false;
//time in milliseconds of last activity
unsigned long timeOfLastActivity; 
//30 seconds allowed connection time
unsigned long allowedConnectTime = 10000; //10 seconds

void setup()
{

     // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  Serial.println("cypher is initializing. Please wait...");
  
  Serial.println("");
  // initialize the ethernet device
  Ethernet.begin(mac);
  
  // print your local IP address:
  
  Serial.print("My IP address: ");
  
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.println("");
  Serial.println("Initializing Telnet Server...");
  Serial.println("");
  // start listening for clients
  server.begin();
  Serial.println("Telnet Server initialized...");
  
}

void loop()
{
  
  // if an incoming client connects, there will be bytes available to read:
  EthernetClient client = server.available();
  
   
  if (client == true) {
    
    if (!gotAMessage) {

      client.print("\n\n    Welcome to cypher.\n");
          client.print("    Do you want to start motherboard?\n");
          client.print("    If yes hit Enter.\n");
          client.print("    Otherwise just close the terminal window.\n");
          client.print("    Connection will be closed automatically after 10 seconds.\n");
          client.print("\n>");
      gotAMessage = true;
      client.flush();
      char c;
      timeOfLastActivity = millis();
      

      do 
      {
          c = client.read();
          if(millis() - timeOfLastActivity > allowedConnectTime && c != 0x0D) {
          client.print("    Timeout: Disconnected.\n");
          client.stop();
          }
      }while (c != 0x0D && client == true);
          
          if(c == 0x0D){
              byte magicPacket[102];
              int i,c1,j=0;
             
              for(i = 0; i < 6; i++,j++){
                  magicPacket[j] = 0xFF;
              }
              for(i = 0; i < 16; i++){
                  for( c1 = 0; c1 < 6; c1++,j++)
                    magicPacket[j] = remote_MAC_ADD[c1];
              }
              
              EthernetUDP Udp;
              Udp.begin(wolPort);
              Udp.beginPacket(broadCastIp, wolPort);
              Udp.write(magicPacket, sizeof magicPacket);
              Udp.endPacket();
              client.println("\nDone.\n");
              client.stop();
          }
        
    
      }
   }
}
