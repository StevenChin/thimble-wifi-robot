#include <ArduinoJson.h>
#include "userAuthentication.h"
#include "wifiConnection.h"
#include "motorControl.h"

#define MAX_BUFFER_LEN 150

StaticJsonBuffer<MAX_BUFFER_LEN> jsonBuffer;

void setup() {
  // initialize serial for debugging
  Serial.begin(115200);
  wifiConnection::init();
  motorControl::init();
}

void handleMoveCmd(JsonObject& payload) {
  bool validToken = userAuthentication::isTokenMatch(payload["authToken"]);

  if (validToken) {
    moveRobot(payload["data"]);
  } else {
    sendInvalidTokenResponse();
  }
}

void moveRobot(JsonObject& payload) {
  char *direction = payload["direction"];

  if (strcmp(direction, "w") == 0) {
    motorControl::goForward();
  } else if (strcmp(direction, "a") == 0) {
    motorControl::goLeft();
  } else if (strcmp(direction, "s") == 0) {
    motorControl::goBack();
  } else if (strcmp(direction, "d") == 0) {
    motorControl::goRight();
  }
}

void sendInvalidCmdResponse() {
  char *reason = "Command not supported";
  sendResponse(createErrorResponse(reason));
}

void sendInvalidTokenResponse() {
  char *reason = "Authentication token is invalid";
  sendResponse(createErrorResponse(reason));
}

void sendParsingErrorResponse() {
  char *reason = "Failed to parse payload, ensure valid json is sent";
  sendResponse(createErrorResponse(reason));
}

JsonObject& createErrorResponse(char *reason) {
  JsonObject& response = jsonBuffer.createObject();
  response["status"] = "failed";
  response["reason"] = reason;
  return response;
}

void sendResponse(JsonObject& response) {
  char replyBuffer[MAX_BUFFER_LEN];
  response.printTo(Serial);
  Serial.println();
  response.printTo(replyBuffer, MAX_BUFFER_LEN);
  wifiConnection::Udp.beginPacket(wifiConnection::Udp.remoteIP(), wifiConnection::Udp.remotePort());
  wifiConnection::Udp.write(replyBuffer);
  wifiConnection::Udp.endPacket();
  memset(replyBuffer, 0, sizeof(replyBuffer));
}

void handleAuthCmd(JsonObject& payload) {
  char *token = userAuthentication::initSession();
  JsonObject& response = jsonBuffer.createObject();
  response["status"] = "success";
  JsonObject& data = response.createNestedObject("data");
  data["authToken"] = token;
  sendResponse(response);
}

void cleanup() {
  jsonBuffer.clear();
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

  int packetSize = wifiConnection::Udp.parsePacket();
  if (packetSize) {
    char packetBuffer[MAX_BUFFER_LEN];
    // read the packet into packetBufffer
    int len = wifiConnection::Udp.read(packetBuffer, MAX_BUFFER_LEN);
    if (len > 0) {
      packetBuffer[len] = 0;
    }

    JsonObject& payload = jsonBuffer.parseObject(packetBuffer);
    if (!payload.success()) {
      sendParsingErrorResponse();
    } else {
      char *cmd = payload["command"];
      if (strcmp(cmd, "move") == 0) {
        handleMoveCmd(payload);
      } else if (strcmp(cmd, "auth") == 0) {
        handleAuthCmd(payload["data"]);
      } else {
        sendInvalidCmdResponse();
      }
    }

    memset(packetBuffer, 0, sizeof(packetBuffer));
    cleanup();
  }
}

