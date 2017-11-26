#include <ArduinoJson.h>
#include "userAuthentication.h"
#include "wifiConnection.h"
#include "motorControl.h"

char packetBuffer[255];          // buffer to hold incoming packet
char ReplyBuffer[] = "ACK";      // a string to send back

void setup() {
  // initialize serial for debugging
  Serial.begin(115200);
  wifiConnection::init();
  motorControl::init();
}

void loop() {
  // If pwmTimeout has been set
  if (motorControl::pwmTimeout > 0) {
    // Compare the current time to pwmTimeout
    if (millis() > motorControl::pwmTimeout) {
      // If pwmTimeout has elapsed, zero pwmTimeout and stop the motors by zeroing the PWM signals
      motorControl::pwmTimeout = 0;
      motorControl::writePwms (0, 0);
    }
  }

  // if there's data available, read a packet
  int packetSize = wifiConnection::Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = wifiConnection::Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(wifiConnection::Udp.remotePort());

    // read the packet into packetBufffer
    int len = wifiConnection::Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    // Use a switch statement to examine the first character of the packet buffer
    switch (packetBuffer[0]) {
      case 'w':
        // If the value is 'w', move the robot forward
        motorControl::goForward();
        break;
      case 'a':
        // If the value is 'a', move the robot left
        motorControl::goLeft();
        break;
      case 's':
        // If the value is 's', move the robot backward
        motorControl::goBack();
        break;
      case 'd':
        // If the value is 's', move the robot right
        motorControl::goRight();
        break;
    }

    // send a reply, to the IP address and port that sent us the packet we received
    wifiConnection::Udp.beginPacket(wifiConnection::Udp.remoteIP(), wifiConnection::Udp.remotePort());
    wifiConnection::Udp.write(ReplyBuffer);
    wifiConnection::Udp.endPacket();

    // Zero the packetBuffer first character until we receive a new packet
    packetBuffer[0] = 0;
  }
}

