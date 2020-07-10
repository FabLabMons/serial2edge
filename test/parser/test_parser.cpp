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

#include "unity.h"

#include "measure.hpp"

using namespace measure;


class FakeParserImpl : public Parser {
public:
  float* received_values_;
  bool valid_;

  ~FakeParserImpl() {
    delete[] received_values_;
  }

  void OnValidMeasure(float* values, std::size_t value_count) override {
    received_values_ = new float[value_count];
    for (size_t i = 0; i < value_count; i++) {
      received_values_[i] = values[i];
    }
    valid_ = true;
  }

  void OnInvalidMeasure() override {
    valid_ = false;
  }
};

FakeParserImpl* parser;

void parseInput(const char* input) {
  for (int i = 0; input[i] != '\0'; i++) {
    parser->Parse(input[i]);
  }
}

void test_parse_one_value(void) {
  parseInput("MM,+0004.60,0\r");
  TEST_ASSERT_TRUE(parser->valid_);
  TEST_ASSERT_EQUAL_FLOAT(4.6F, parser->received_values_[0]);
}

void test_Parse_one_negative_value(void) {
  parseInput("MM,-0004.60,0\r");
  TEST_ASSERT_TRUE(parser->valid_);
  TEST_ASSERT_EQUAL_FLOAT(-4.6F, parser->received_values_[0]);
}

void test_Parse_one_value_twice(void) {
  parseInput("MM,+0004.60,0\r");
  parseInput("MM,+001.361,0\r");
  TEST_ASSERT_TRUE(parser->valid_);
  TEST_ASSERT_EQUAL_FLOAT(1.361F, parser->received_values_[0]);
}

void test_Parse_16_valid_values(void) {
  parseInput("MM,+0004.60,0,+001.361,0,+007.100,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  TEST_ASSERT_TRUE(parser->valid_);
  TEST_ASSERT_EQUAL_FLOAT(4.6F, parser->received_values_[0]);
  TEST_ASSERT_EQUAL_FLOAT(1.361F, parser->received_values_[1]);
  TEST_ASSERT_EQUAL_FLOAT(7.1F, parser->received_values_[2]);
  TEST_ASSERT_EQUAL_FLOAT(4.116F, parser->received_values_[3]);
  TEST_ASSERT_EQUAL_FLOAT(3.233F, parser->received_values_[4]);
  TEST_ASSERT_EQUAL_FLOAT(123.456F, parser->received_values_[5]);
}

void test_Parse_invalid_values(void) {
  parseInput("MM,+0004.60,0,+001.361,0,+007.100,1,+004.116,0,+003.233,0,-999.999,0,-999.999,0,-9999.99,0,-99.9999,0,-999.999,0,-999.999,1,-999.999,1,-999.999,1,-999.999,1,-999.999,1,-999.999,1\r");
  TEST_ASSERT_FALSE(parser->valid_);
}

void test_Parse_interrupted_input1(void) {
  parseInput("AMM,+0004.60,0,+001.361,0,+007.100,0\r");
  TEST_ASSERT_TRUE(parser->valid_);
  TEST_ASSERT_EQUAL_FLOAT(4.6F, parser->received_values_[0]);
  TEST_ASSERT_EQUAL_FLOAT(1.361F, parser->received_values_[1]);
  TEST_ASSERT_EQUAL_FLOAT(7.1F, parser->received_values_[2]);
}

void test_Parse_interrupted_input2(void) {
  parseInput("MAMM,+0004.60,0,+001.361,0,+007.100,0\r");
  TEST_ASSERT_TRUE(parser->valid_);
  TEST_ASSERT_EQUAL_FLOAT(4.6F, parser->received_values_[0]);
  TEST_ASSERT_EQUAL_FLOAT(1.361F, parser->received_values_[1]);
  TEST_ASSERT_EQUAL_FLOAT(7.1F, parser->received_values_[2]);
}

void test_Parse_interrupted_input3(void) {
  parseInput("MM,MM,+0004.60,0,+001.361,0,+007.100,0\r");
  TEST_ASSERT_TRUE(parser->valid_);
  TEST_ASSERT_EQUAL_FLOAT(4.6F, parser->received_values_[0]);
  TEST_ASSERT_EQUAL_FLOAT(1.361F, parser->received_values_[1]);
  TEST_ASSERT_EQUAL_FLOAT(7.1F, parser->received_values_[2]);
}

void test_Parse_interrupted_input4(void) {
  parseInput("MM,+MM,+0004.60,0,+001.361,0,+007.100,0\r");
  TEST_ASSERT_TRUE(parser->valid_);
  TEST_ASSERT_EQUAL_FLOAT(4.6F, parser->received_values_[0]);
  TEST_ASSERT_EQUAL_FLOAT(1.361F, parser->received_values_[1]);
  TEST_ASSERT_EQUAL_FLOAT(7.1F, parser->received_values_[2]);
}

void test_Parse_interrupted_input5(void) {
  parseInput("MM,+0MM,+0004.60,0,+001.361,0,+007.100,0\r");
  TEST_ASSERT_TRUE(parser->valid_);
  TEST_ASSERT_EQUAL_FLOAT(4.6F, parser->received_values_[0]);
  TEST_ASSERT_EQUAL_FLOAT(1.361F, parser->received_values_[1]);
  TEST_ASSERT_EQUAL_FLOAT(7.1F, parser->received_values_[2]);
}

void test_Parse_interrupted_input6(void) {
  parseInput("MM,+004.116,0,+003.233,0,-999.999,1,-999.999,1,-999.999MM,+0004.60,0,+001.361,0,+007.100,0\r");
  TEST_ASSERT_TRUE(parser->valid_);
  TEST_ASSERT_EQUAL_FLOAT(4.6F, parser->received_values_[0]);
  TEST_ASSERT_EQUAL_FLOAT(1.361F, parser->received_values_[1]);
  TEST_ASSERT_EQUAL_FLOAT(7.1F, parser->received_values_[2]);
}

void test_Parse_invalid_then_valid(void) {
  parseInput("MM,+0004.60,0,+001.361,0,+007.100,0,+004.116,0,+003.233,1,-999.999,0,-999.999,0,-9999.99,0,-99.9999,0,-999.999,0,-999.999,1,-999.999,1,-999.999,1,-999.999,1,-999.999,1,-999.999,1\r");
  TEST_ASSERT_FALSE(parser->valid_);
  parseInput("MM,+0004.60,0,+001.361,0,+007.100,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  TEST_ASSERT_TRUE(parser->valid_);
}

void process() {
  UNITY_BEGIN();
  RUN_TEST(test_parse_one_value);
  RUN_TEST(test_Parse_one_value_twice);
  RUN_TEST(test_Parse_16_valid_values);
  RUN_TEST(test_Parse_invalid_values);
  RUN_TEST(test_Parse_interrupted_input1);
  RUN_TEST(test_Parse_interrupted_input2);
  RUN_TEST(test_Parse_interrupted_input3);
  RUN_TEST(test_Parse_interrupted_input4);
  RUN_TEST(test_Parse_interrupted_input5);
  RUN_TEST(test_Parse_interrupted_input6);
  RUN_TEST(test_Parse_invalid_then_valid);
  UNITY_END();
}

void setUp(void) {
  parser = new FakeParserImpl();
}

void tearDown(void) {
  delete parser;
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
