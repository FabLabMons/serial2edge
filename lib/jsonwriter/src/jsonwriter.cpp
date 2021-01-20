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

#define TRUE "true"
#define FALSE "false"

#include "jsonwriter.h"

#include <cstddef>
#include <cstdio>
#include <cstring>

namespace jsonwriter {

WriterOutput Writer::Write(unsigned long time, Dimension* dimensions, std::size_t nb_dimensions) {
  _output.buffer[0] = '\0';
  _output.length = 0;
  OpenObject();
  WriteULong("time", time);
  NextAttribute();
  WriteCharArray("sensor_id", "coupole_profile");
  NextAttribute();
  OpenNestedObject("dimensions");
  for (unsigned int i = 0; i < nb_dimensions; i++) {
    Dimension dimension = dimensions[i];
    OpenNestedObject(dimension.name);
    WriteFloat("value", dimension.value);
    NextAttribute();
    WriteBool("valid", dimension.valid);
    CloseObject();
    if (i < nb_dimensions - 1) {
      NextAttribute();
    }
  }
  CloseObject();
  CloseObject();
  return _output;
}

void Writer::OpenObject() {
  strcat(_output.buffer, "{");
  _output.length++;
}

void Writer::OpenNestedObject(const char* name) {
  char* target = _output.buffer + _output.length;
  std::size_t actual_size = sprintf(target, "\"%s\":{", name);
  _output.length += actual_size;
}

void Writer::CloseObject() {
  strcat(_output.buffer, "}");
  _output.length++;
}

void Writer::NextAttribute() {
  strcat(_output.buffer, ",");
  _output.length++;
}

void Writer::WriteBool(const char* name, bool value) {
  char* target = _output.buffer + _output.length;
  std::size_t actual_size = sprintf(target, "\"%s\":%s", name, (value ? TRUE : FALSE));
  _output.length += actual_size;
}

void Writer::WriteCharArray(const char* name, const char* value) {
  char* target = _output.buffer + _output.length;
  std::size_t actual_size = sprintf(target, "\"%s\":\"%s\"", name, value);
  _output.length += actual_size;
}

void Writer::WriteFloat(const char* name, float value) {
  char* target = _output.buffer + _output.length;
  std::size_t actual_size = sprintf(target, "\"%s\":%.3f", name, value);
  _output.length += actual_size;
}

void Writer::WriteULong(const char* name, unsigned long value) {
  char* target = _output.buffer + _output.length;
  std::size_t actual_size = sprintf(target, "\"%s\":%lu", name, value);
  _output.length += actual_size;
}

} // namespace jsonwriter
