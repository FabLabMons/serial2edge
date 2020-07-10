/*
 * Copyright (c) 2020 UMONS Fab-IoT-Lab
 *
 * serial2edge - UMONS Fab-IoT-Lab (ROLAND Francois) is free software: you can
 * redistribute it and/or modify it under the terms of the MIT License.  This
 * program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the MIT License for more details.
 *
 * You should have received a copy of the MIT License along with this program.  If
 * not, see https://spdx.org/licenses/MIT.html. Each use of this software must be
 * attributed to University of MONS - Fab-IoT-Lab (ROLAND Francois).
 */

#include <Arduino.h>
#include <Adafruit_SleepyDog.h>
#include <Wire.h>
#include <variant.h>

#include <MKRGSM.h>
#include <cstddef>

#include "jsonwriter.h"
#include "measure.hpp"
#include "arduino_secrets.h"

#define VALUES_PER_MEASURE 6
#define VALUES_MAX 16
#define GSM_RETRY_MAX 3
#define ACQUIRE_PERIOD_MS 500
#define BLINK_PERIOD_MS 10
#define WDT_TIMEOUT_MS 30000

const char PINNUMBER[]     = SECRET_PINNUMBER;
const char GPRS_APN[]      = SECRET_GPRS_APN;
const char GPRS_LOGIN[]    = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;

const char SERVER[] = SECRET_SERVER;
const char PATH[] = SECRET_URL;
const int SERVER_PORT = 443;

const char* DIMENSION_NAMES[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P"};

GSMSSLClient client;
GPRS gprs;
GSM gsmAccess;
jsonwriter::Dimension dimToSend[16];
jsonwriter::WriterOutput out;
jsonwriter::Writer writer;

void sendData(unsigned long time, float *dimensions, std::size_t dimensions_size) {
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
  digitalWrite(2, HIGH);
  if (client.connectSSL(SERVER, SERVER_PORT)) {
    // Make a HTTP request:
    client.print("POST ");
    client.print(PATH);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(SERVER);
    client.print("Connection: close\r\n");
    client.print("Content-type: application/json\r\n");
    for (std::size_t i = 0; i < dimensions_size; i++) {
      const char* dim_name = DIMENSION_NAMES[i];
      jsonwriter::Dimension dim = {dim_name, dimensions[i], true};
      dimToSend[i] = dim;
    }
    out = writer.Write(millis(), dimToSend, dimensions_size);
    client.print("Content-Length: ");
    client.print(out.length, 10);
    client.print("\r\n");
    client.print("\r\n");
    client.print(out.buffer);
    client.flush();
    client.stop();
  } else {
    // Wait until WDT resets the board
    while (true);
  }
  digitalWrite(2, LOW);
}

class CustomParser : public measure::Store {
protected:
  void OnAveragesReady(float* values, std::size_t value_count) {
    sendData(millis(), values, value_count);
  }
};

CustomParser parser;

bool gsmConnect() {
  bool connected = false;
  unsigned int retries = GSM_RETRY_MAX;
  while (!connected && retries > 0) {
    if ((gsmAccess.begin(PINNUMBER) == GSM_READY) &&
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
      digitalWrite(3, HIGH);
      connected = true;
    } else {
      digitalWrite(3, HIGH);
      delay(100);
      digitalWrite(3, LOW);
    }
    retries--;
  }
  return connected;
}

void setup() {
  gsmAccess.setTimeout(180000);
  gprs.setTimeout(180000);
  Serial1.begin(115200);
#ifdef SETUP_MODE
  Serial.begin(115200);
  Serial.println();
  Serial.println("SETUP MODE");
#endif
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  digitalWrite(0, HIGH);
  delay(100);
  digitalWrite(1, HIGH);
  delay(100);
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(3, HIGH);
  delay(200);
  digitalWrite(0, LOW);
  delay(100);
  digitalWrite(1, LOW);
  delay(100);
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(3, LOW);

  gsmConnect();

  Watchdog.enable(WDT_TIMEOUT_MS);
#ifdef SETUP_MODE
  Serial.println("starting measures");
#endif
}

unsigned long start = 0;
unsigned long received = 0;
unsigned long now = 0;

void loop() {
  Watchdog.reset();

  now = millis();
  if (now - received > BLINK_PERIOD_MS) {
    digitalWrite(0, LOW);
  }
  if (now - start > BLINK_PERIOD_MS) {
    digitalWrite(1, LOW);
  }
  if (now - start > ACQUIRE_PERIOD_MS && now - received > ACQUIRE_PERIOD_MS && Serial1.availableForWrite()) {
    start = now;
    String request = "MM,1,";
    for (size_t i = 0; i < VALUES_PER_MEASURE; i++) {
      request += "1";
    }
    for (size_t i = VALUES_PER_MEASURE; i < VALUES_MAX; i++) {
      request += "0";
    }
    request += "\r";
    Serial1.write(request.c_str());
    Serial1.flush();
    digitalWrite(1, HIGH);
#ifdef SETUP_MODE
    Serial.println();
    Serial.print(now, DEC);
    Serial.print(",");
#endif
  }
  if (Serial1.available()) {
    digitalWrite(0, HIGH);
    received = now;
    char c = Serial1.read();
#ifdef SETUP_MODE
    Serial.print(c);
#endif
    parser.Parse(c);
  }
}
