#pragma once

#include <string>
#include "Common.h"

class Buffer {
 public:
  Buffer() = default;
  ~Buffer() = default;
  DISALLOW_COPY_AND_MOVE(Buffer);

  std::string &Buf();
  void Append(const char *str, int size);
  ssize_t Size();
  const char *ToStr() const;
  void Clear();
  void SetBuf(const char *buf);

 private:
  std::string buf_;
};
