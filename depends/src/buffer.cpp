#include "buffer.h"
#include <cstddef>
#include <memory>
#include "cell.h"
#include "message_header.h"
#include <algorithm>

Buffer::Buffer(size_t capacity)
    : data_(std::make_unique<char[]>(capacity + 1)),
      capacity_(capacity),
      write_pos_(0),
      full_count_(0) {}

Buffer::~Buffer() {}

bool Buffer::Push(const char* data, size_t length) noexcept {
  if (!data || length == 0) return false;
  if (write_pos_ + length > capacity_) {
    ++full_count_;
    return false;
  }
  memcpy(data_.get() + write_pos_, data, length);
  write_pos_ += length;
  if (write_pos_ == full_count_) {
    ++full_count_;
  }
  return true;
}

void Buffer::Consume(size_t bytes) {
  if (bytes >= write_pos_) {
    write_pos_ = 0;
  } else {
    size_t remain = write_pos_ - bytes;
    memmove(data_.get(), data_.get() + bytes, remain);
    write_pos_ = remain;
  }
  if (full_count_ > 0) --full_count_;
}

int Buffer::WriteToSocket(int sockfd) {
  if (write_pos_ == 0 || INVALID_SOCKET == sockfd) return 0;
  size_t sent = send(sockfd, data_.get(), write_pos_, 0);
  if (sent <= 0) {
    PLOG(ERROR) << "WriteToSocket failed on sockfd=" << sockfd
                << " write_pos=" << write_pos_;
    return SOCKET_ERROR;
  }
  if (static_cast<size_t>(sent) == write_pos_) {
    write_pos_ = 0;
  } else {
    write_pos_ -= sent;
    memmove(data_.get(), data_.get() + sent, write_pos_);
  }
  full_count_ = 0;
  return static_cast<int>(sent);
}

int Buffer::ReadFromSocket(int sockfd) {
  if (capacity_ - write_pos_ == 0) return 0;
  char* dst = data_.get() + write_pos_;
  ssize_t len = recv(sockfd, dst, capacity_ - write_pos_, 0);
  if (len <= 0) {
    // PLOG(ERROR) << "ReadFromSocket failed";
    return SOCKET_ERROR;
  }
  write_pos_ += len;
  data_[write_pos_] = 0;
  return static_cast<int>(len);
}

bool Buffer::HasData() const {
  if (write_pos_ >= sizeof(DataHeader)) {
    const DataHeader* header = reinterpret_cast<const DataHeader*>(data_.get());
    return write_pos_ >= header->data_length;
  }
  return false;
}
