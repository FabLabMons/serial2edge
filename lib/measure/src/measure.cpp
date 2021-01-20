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

#include "measure.hpp"

#include <cctype>
#include <cstdlib>
#include <iostream>

namespace measure
{
Parser::Parser() {
  ResetParser();
}

void Parser::Parse(int c) {
  switch (state_)
  {
  case ParserState::READY:
    if (c == 'M') {
      state_ = ParserState::PREFIX;
    }
    break;
  case ParserState::PREFIX:
    if (c == 'M') {
      state_ = ParserState:: COMMAND;
    } else {
      ResetParser(c);
    }
    break;
  case ParserState::COMMAND:
    if (c == ',') {
      state_ = ParserState::BEGIN_MEASURE;
    } else {
      ResetParser(c);
    }
    break;
  case ParserState::BEGIN_MEASURE:
    if (c == '-') {
      *buffer_ptr_ = c;
      buffer_ptr_++;
      state_ = ParserState::VALUE;
    } else if (c == '+') {
      state_ = ParserState::VALUE;
    } else {
      ResetParser(c);
    }
    break;
  case ParserState::VALUE:
    if (c == '.' || std::isdigit(c)) {
      *buffer_ptr_ = c;
      buffer_ptr_++;
    } else if (c == ',') {
      state_ = ParserState::VALIDITY;
    } else {
      ResetParser(c);
    }
    break;
  case ParserState::VALIDITY:
    if (c == '0') {
      state_ = ParserState::END_MEASURE;
    } else if (c == '1' || c == '2') {
      OnInvalidMeasure();
      ResetParser();
    } else {
      ResetParser(c);
    }
    break;
  case ParserState::END_MEASURE:
    if (c == ',') {
      StoreMeasurement();
      if (value_count_ >= kValuesPerMeasure) {
        OnValidMeasure(values_, value_count_);
        ResetParser();
      } else {
        state_ = ParserState::BEGIN_MEASURE;
      }
    } else if (c == '\r') {
      StoreMeasurement();
      OnValidMeasure(values_, value_count_);
      ResetParser();
    } else {
      ResetParser(c);
    }
    break;
  default:
    ResetParser(c);
  }
}

void Parser::ResetParser() {
  for (int i = 0; i < kParserMaxValues; i++) {
    values_[i] = kMeasureInitialValue;
  }
  buffer_ptr_ = buffer_;
  value_count_ = 0;
  state_ = ParserState::READY;
}

void Parser::ResetParser(int c) {
  ResetParser();
  if (c == 'M') {
    state_ = ParserState::PREFIX;
  }
}

void Parser::StoreMeasurement() {
  *buffer_ptr_ = '\0';
  values_[value_count_] = std::atof(buffer_);
  buffer_ptr_ = buffer_;
  value_count_++;
}

void Store::OnInvalidMeasure() {
  error_count_++;
  if (error_count_ == kInvalidMeasureCountThreshold) {
    IgnoreLastValues();
    float averages[kValuesPerMeasure];
    ComputeAverages(averages);
    value_store_.clear();
    OnAveragesReady(averages, kValuesPerMeasure);
  }
}

void Store::IgnoreLastValues() {
  for (uint16_t i = 0; i < kLastValuesToDrop; i++) {
    value_store_.pop();
  }
}

void Store::ComputeAverages(float* averages) {
  uint16_t value_per_measure = value_store_.size() / kValuesPerMeasure;
  float totals[kValuesPerMeasure];
  for (uint8_t measure_idx = 0; measure_idx < kValuesPerMeasure; measure_idx++) {
    totals[measure_idx] = 0.0F;
  }
  for (uint16_t value_idx = 0; value_idx < value_per_measure; value_idx++) {
    for (uint8_t measure_idx = 0; measure_idx < kValuesPerMeasure; measure_idx++) {
      totals[measure_idx] += value_store_.shift();
    }
  }
  for (uint8_t i = 0; i < kValuesPerMeasure; i++) {
    averages[i] = totals[i] / value_per_measure;
  }
}

void Store::OnValidMeasure(float* values, std::size_t value_count) {
  if (value_count == kValuesPerMeasure) {
    for (uint8_t i = 0; i < kValuesPerMeasure; i++) {
      value_store_.push(values[i]);
    }
    error_count_ = 0;
  }
}
} // namespace measure
