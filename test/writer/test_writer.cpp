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

#include <iostream>

#include <ArduinoJson.h>
#include <unity.h>
#include <jsonwriter.h>

using namespace jsonwriter;

void setUp(void) {
// set stuff up here
}

void tearDown(void) {
// clean stuff up here
}

void test_no_dimension(void) {
  Dimension dimensions[] = {};
  Writer* writer = new Writer();
  WriterOutput output = writer->Write(0, dimensions, sizeof(dimensions) / sizeof(Dimension));

  StaticJsonDocument<1000> doc;
  deserializeJson(doc, output.buffer);
  TEST_ASSERT_EQUAL_STRING("coupole_profile", doc["sensor_id"]);
  TEST_ASSERT_NOT_NULL(doc["dimensions"]);
  delete writer;
}

void test_one_dimension(void) {
  Dimension dimensions[] = {
    Dimension{"A", 4.5F, true}
  };
  Writer* writer = new Writer();
  WriterOutput output = writer->Write(1000, dimensions, sizeof(dimensions) / sizeof(Dimension));

  StaticJsonDocument<1000> doc;
  deserializeJson(doc, output.buffer);
  TEST_ASSERT_EQUAL(1000, doc["time"]);
  TEST_ASSERT_NOT_NULL(doc["dimensions"]["A"]);
  TEST_ASSERT_EQUAL_FLOAT(4.5F, doc["dimensions"]["A"]["value"]);
  TEST_ASSERT_TRUE(doc["dimensions"]["A"]["valid"]);
  delete writer;
}

void test_many_dimensions(void) {
  Dimension dimensions[] = {
    Dimension{"A", 4.5F, true},
    Dimension{"B", 3.6F, false},
    Dimension{"C", 97.5F, true}
  };
  Writer* writer = new Writer();
  WriterOutput output = writer->Write(1000, dimensions, sizeof(dimensions) / sizeof(Dimension));

  StaticJsonDocument<1000> doc;
  deserializeJson(doc, output.buffer);
  TEST_ASSERT_EQUAL_FLOAT(4.5F, doc["dimensions"]["A"]["value"]);
  TEST_ASSERT_TRUE(doc["dimensions"]["A"]["valid"]);
  TEST_ASSERT_EQUAL_FLOAT(3.6F, doc["dimensions"]["B"]["value"]);
  TEST_ASSERT_FALSE(doc["dimensions"]["B"]["valid"]);
  TEST_ASSERT_EQUAL_FLOAT(97.5F, doc["dimensions"]["C"]["value"]);
  TEST_ASSERT_TRUE(doc["dimensions"]["C"]["valid"]);
  TEST_ASSERT_EQUAL_UINT(159, output.length);
  delete writer;
}

void process() {
  UNITY_BEGIN();
  RUN_TEST(test_no_dimension);
  RUN_TEST(test_one_dimension);
  RUN_TEST(test_many_dimensions);
  UNITY_END();
}

#ifdef ARDUINO

#include <Arduino.h>
void setup() {
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  process();
}

void loop() {
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(500);
}

#else

int main(int argc, char **argv) {
  process();
  return 0;
}

#endif
