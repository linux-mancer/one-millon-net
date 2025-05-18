#pragma once

#include <cstddef>
#include <memory>
#include <cassert>

class ByteStream {
 public:
  ByteStream(char* data, size_t size, bool take_ownership);
  explicit ByteStream(size_t size);
  ByteStream(const ByteStream&) = delete;
  ByteStream& operator=(const ByteStream&) = delete;
  ByteStream(ByteStream&&) noexcept = default;
  ByteStream& operator=(ByteStream&&) noexcept = default;

  ~ByteStream() = default;

  char* data() { return data_; }
  const char* data() const { return data_; }

  size_t length() const { return write_pos_; }

  bool CanRead(size_t n) { return (write_pos_ - read_pos_) >= n; }

  bool CanWrite(size_t n) { return (capacity_ - write_pos_) >= n; }

  void Push(size_t n) {
    if (CanWrite(n)) write_pos_ += n;
  }

  void Pop(size_t n) {
    if (CanRead(n)) read_pos_ += n;
  }

  void Reset() {
    read_pos_ = 0;
    write_pos_ = 0;
  }
 private:
  std::unique_ptr<char[]> owned_data_;
  char* data_ = nullptr;
  size_t capacity_ = 0;
  size_t write_pos_ = 0;
  size_t read_pos_ = 0;
};
