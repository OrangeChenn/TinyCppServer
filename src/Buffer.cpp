#include "Buffer.h"

std::string &Buffer::Buf() { return buf_; }

void Buffer::Append(const char *str, int size) {
  for (int i = 0; i < size; i++) {
    char ch = str[i];
    if (ch == '\0') {
      break;
    }
    buf_.push_back(ch);
  }
}

ssize_t Buffer::Size() { return buf_.size(); }

const char *Buffer::ToStr() const { return buf_.c_str(); }

void Buffer::Clear() { buf_.clear(); }

void Buffer::SetBuf(const char *buf) {
  buf_.clear();
  buf_.append(buf);
}
