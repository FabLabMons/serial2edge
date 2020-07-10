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
