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

#if !defined(JSON_WRITER_H)
#define JSON_WRITER_H

#include <cstddef>

namespace jsonwriter {

struct Dimension {
  const char* name;
  float value;
  bool valid;
};

struct WriterOutput {
  char buffer[1000];
  std::size_t length = 0;
};

class Writer {
public:
  Writer() {}
  struct WriterOutput Write(unsigned long time, Dimension* dimensions, std::size_t nb_dimensions);
private:
  WriterOutput _output;
  void OpenObject();
  void OpenNestedObject(const char* name);
  void CloseObject();
  void NextAttribute();
  void WriteBool(const char* name, bool value);
  void WriteCharArray(const char* name, const char* value);
  void WriteFloat(const char* name, float value);
  void WriteULong(const char* name, unsigned long value);
};

} // namespace jsonwriter


#endif // JSON_WRITER_H
