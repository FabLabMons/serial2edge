/*
 * Copyright (c) 2020 UMONS Fab-IoT-Lab
 *
 * This file is part of serial2edge.
 *
 *    serial2edge is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    serial2edge is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY * without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with serial2edge.  If not, see <https://www.gnu.org/licenses/>.
 */

#if !defined(MEASURE_PARSER_H)
#define MEASURE_PARSER_H

#include <cstddef>

#include "CircularBuffer.h"

namespace measure
{
const uint16_t kValuesPerMeasure = 6;
const uint16_t kInvalidMeasureCountThreshold = 3;
const uint16_t kLastMeasuresToDrop = 2;
const uint16_t kValidMeasuresToKeep = 3;

const uint16_t kLastValuesToDrop = kLastMeasuresToDrop * kValuesPerMeasure;
const uint16_t kStoreCapacity = (kValidMeasuresToKeep * kValuesPerMeasure) + kLastValuesToDrop;

const uint8_t kParserMaxValues = 16;
const uint16_t kParserBufferCapacity = 1000;
const float kMeasureInitialValue = -9999.9F;

enum ParserState { READY, PREFIX, COMMAND, BEGIN_MEASURE, VALUE, VALIDITY, END_MEASURE };
enum MeasureValidity { VALID, ALARM, WAITING_DECISION };

class Parser {
public:
  Parser();
  void Parse(int c);
protected:
  virtual void OnInvalidMeasure() = 0;
  virtual void OnValidMeasure(float* values, std::size_t value_count) = 0;
private:
  char buffer_[kParserBufferCapacity];
  char* buffer_ptr_;
  ParserState state_ = READY;
  float values_[kParserMaxValues];
  uint8_t value_count_;
  void ResetParser();
  void ResetParser(int c);
  void StoreMeasurement();
};

class Store : public Parser {
public:
  Store() {}
protected:
  void OnValidMeasure(float* values, std::size_t value_count) override;
  void OnInvalidMeasure() override;
  virtual void OnAveragesReady(float* values, std::size_t value_count) = 0;
private:
  CircularBuffer<float, kStoreCapacity> value_store_;
  uint16_t error_count_;
  void IgnoreLastValues();
  void ComputeAverages(float* averages);
};
} // namespace measure

#endif // MEASURE_PARSER_H
