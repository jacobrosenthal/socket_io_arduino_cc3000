#include <SocketIOClient.h>
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11

#define WLAN_SSID       "myNetwork"        // cannot be longer than 32 characters!
#define WLAN_PASS       "myPassword"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define HOSTNAME "myHostname"
#define PORT 8080


Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIV2); // you can change this clock speed
 
SocketIOClient client;

void setup()
{
  Serial.begin(115200);
  
  InitializeCC30000();
  Serial.println("Initialized");
  
  /* Display the IP address DNS, Gateway, etc. */  
  while (! displayConnectionDetails()) {
    delay(1000);
  }
   		
  if (!client.connect(cc3000,HOSTNAME,PORT))
  {
    Serial.println("Not connected.");
  }
  else
  {
    Serial.println("Connected.");
  }
  client.setDataArrivedDelegate(ondata);

}

void loop()
{
  client.monitor(cc3000);
  client.sendMessage("hello");
  delay(2000);
}

void InitializeCC30000(void)
{
    // Initialise the module
    Serial.println("\nInitializing...");
    
    if (!cc3000.begin())
    {
      Serial.println("Couldn't begin()! Check your wiring?");
      while(1);
    }
    
    // Optional SSID scan
    if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) 
    {
      Serial.println("Failed!");
      while(1);
    }
    
    Serial.println("Connected!");
    /* Wait for DHCP to complete */
    Serial.println("Request DHCP");
    while (!cc3000.checkDHCP())
    {
      delay(100); // ToDo: Insert a DHCP timeout!
    }
}

void ondata(SocketIOClient client, char *data) 
{
  Serial.println("Data Arrived: ");
  Serial.println(data);
}

bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}