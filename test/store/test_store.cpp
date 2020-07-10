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

class FakeStoreImpl : public Store {
public:
  float* received_values_;
  size_t received_value_count_;

  ~FakeStoreImpl() {
    delete[] received_values_;
  }

  void OnAveragesReady(float* values, size_t value_count) {
    received_value_count_ = value_count;
    received_values_ = new float[value_count];
    for (size_t i = 0; i < value_count; i++) {
      received_values_[i] = values[i];
    }
  }

  void ParseInput(const char* input) {
    for (int i = 0; input[i] != '\0'; i++) {
      Parse(input[i]);
    }
  }
};

FakeStoreImpl* store;


void test_Store_1_valid_then_20_invalid(void) {
  // store one valid value
  store->ParseInput("MM,+0004.60,0,+001.361,0,+007.100,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  // store values to drop
  for (uint16_t i = 0; i < kLastMeasuresToDrop; i++) {
    store->ParseInput("MM,+0004.60,0,+001.361,0,+007.100,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  }
  // notify invalid values to reach threshold
  for (uint8_t i = 0; i < kInvalidMeasureCountThreshold; i++) {
    store->ParseInput("MM,+0004.60,1,+001.361,0,+007.100,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  }

  TEST_ASSERT_EQUAL_INT(kValuesPerMeasure, store->received_value_count_);
  TEST_ASSERT_EQUAL_FLOAT(4.6F, store->received_values_[0]);
  TEST_ASSERT_EQUAL_FLOAT(1.361F, store->received_values_[1]);
  TEST_ASSERT_EQUAL_FLOAT(7.1F, store->received_values_[2]);
  TEST_ASSERT_EQUAL_FLOAT(4.116F, store->received_values_[3]);
  TEST_ASSERT_EQUAL_FLOAT(3.233F, store->received_values_[4]);
  TEST_ASSERT_EQUAL_FLOAT(123.456F, store->received_values_[5]);
}

void test_Store_5_valid_then_20_invalid(void) {
  store->ParseInput("MM,+0001.00,0,+003.000,0,+004.000,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  store->ParseInput("MM,+0003.00,0,+003.000,0,+005.000,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  store->ParseInput("MM,+0002.00,0,+003.000,0,+004.500,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  store->ParseInput("MM,+0003.00,0,+003.000,0,+004.000,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  store->ParseInput("MM,+0001.00,0,+003.000,0,+005.000,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  // store values to drop
  for (uint16_t i = 0; i < kLastMeasuresToDrop; i++) {
    store->ParseInput("MM,+0004.60,0,+001.361,0,+005.100,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  }
  // notify invalid values to reach threshold
  for (uint8_t i = 0; i < kInvalidMeasureCountThreshold; i++) {
    store->ParseInput("MM,+0004.60,1,+001.361,0,+007.100,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  }

  TEST_ASSERT_EQUAL_INT(kValuesPerMeasure, store->received_value_count_);
  TEST_ASSERT_EQUAL_FLOAT(2.0F, store->received_values_[0]);
  TEST_ASSERT_EQUAL_FLOAT(3.0F, store->received_values_[1]);
  TEST_ASSERT_EQUAL_FLOAT(4.5F, store->received_values_[2]);
  TEST_ASSERT_EQUAL_FLOAT(4.116F, store->received_values_[3]);
  TEST_ASSERT_EQUAL_FLOAT(3.233F, store->received_values_[4]);
  TEST_ASSERT_EQUAL_FLOAT(123.456F, store->received_values_[5]);
}

void test_Store_many_valid_then_20_invalid(void) {
  // store valid values that should be ignored
  for (size_t i = 0; i < 100; i++) {
    store->ParseInput("MM,+0100.00,0,+003.000,0,+004.000,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  }
  // store values we want to get back
  for (size_t i = 0; i < 10; i++) {
    store->ParseInput("MM,+0020.00,0,+0040.00,0,+0050.00,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  }
  // store values to drop
  for (uint16_t i = 0; i < kLastMeasuresToDrop; i++) {
    store->ParseInput("MM,+0004.60,0,+001.361,0,+005.100,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  }
  // notify invalid values to reach threshold
  for (uint8_t i = 0; i < kInvalidMeasureCountThreshold; i++) {
    store->ParseInput("MM,+0004.60,1,+001.361,0,+007.100,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  }

  TEST_ASSERT_EQUAL_INT(kValuesPerMeasure, store->received_value_count_);
  TEST_ASSERT_EQUAL_FLOAT(20.0F, store->received_values_[0]);
  TEST_ASSERT_EQUAL_FLOAT(40.0F, store->received_values_[1]);
  TEST_ASSERT_EQUAL_FLOAT(50.0F, store->received_values_[2]);
}

void test_Reset_between_batches(void) {
  for (size_t i = 0; i < kValidMeasuresToKeep + kLastMeasuresToDrop; i++) {
    store->ParseInput("MM,+0020.00,0,+0040.00,0,+0050.00,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  }
  // notify invalid values to reach threshold
  for (uint8_t i = 0; i < kInvalidMeasureCountThreshold; i++) {
    store->ParseInput("MM,+0004.60,1,+001.361,0,+007.100,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  }

  for (size_t i = 0; i < kValidMeasuresToKeep + kLastMeasuresToDrop; i++) {
    store->ParseInput("MM,+002.000,0,+004.000,0,+005.000,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  }
  // notify invalid values to reach threshold
  for (uint8_t i = 0; i < kInvalidMeasureCountThreshold; i++) {
    store->ParseInput("MM,+0004.60,1,+001.361,0,+007.100,0,+004.116,0,+003.233,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0,+123.456,0\r");
  }

  TEST_ASSERT_EQUAL_INT(kValuesPerMeasure, store->received_value_count_);
  TEST_ASSERT_EQUAL_FLOAT(2.0F, store->received_values_[0]);
  TEST_ASSERT_EQUAL_FLOAT(4.0F, store->received_values_[1]);
  TEST_ASSERT_EQUAL_FLOAT(5.0F, store->received_values_[2]);
}

void process() {
  UNITY_BEGIN();
  RUN_TEST(test_Store_1_valid_then_20_invalid);
  RUN_TEST(test_Store_5_valid_then_20_invalid);
  RUN_TEST(test_Store_many_valid_then_20_invalid);
  RUN_TEST(test_Reset_between_batches);
  UNITY_END();
}

void setUp(void) {
  store = new FakeStoreImpl();
}

void tearDown(void) {
  delete store;
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
