#include "buffer.h"
#include "cell.h"
#include "message_header.h"

Buffer::Buffer(size_t capacity) {
  size_ = capacity;
  data_ = new char[size_+1];
}

Buffer::~Buffer() {
  if (data_) {
    delete[] data_;
    data_ = nullptr;
  }
}

bool Buffer::Push(const char* data, size_t length) noexcept {
  if (head_ + length <= size_) {
    memcpy(data_ + head_, data, length);
    head_ += length;
    if (head_ == SEND_BUFF_SIZE) {
      ++full_count_;
    }
    return true;
  } else {
    ++full_count_;
  }
  return false;
}

void Buffer::Consume(size_t bytes) {
  int n = head_ - bytes;
  if (n > 0) {
    memcpy(data_, data_ + bytes, n);
  }
  head_ = n;
  if (full_count_ > 0) {
    --full_count_;
  }
}

int Buffer::WriteToSocket(int sockfd) {
  int ret = 0;
  if (head_ > 0 && INVALID_SOCKET != sockfd) {
    ret = send(sockfd, data_, head_, 0);
    if (ret <= 0) {
      LOG(ERROR) << "WriteToSocket: sockf<" << sockfd << "> size<" << size_
                 << "> head<" << head_ << "> ret<" << ret;
      return SOCKET_ERROR;
    }
    if (ret == head_) {
      head_ = 0;
    } else {
      head_ -= ret;
      memcpy(data_, data_ + ret, head_);
    }
    full_count_ = 0;
  }
  return ret;
}

int Buffer::ReadFromSocket(int sockfd) {
  if (size_ - head_ > 0) {
    char* sz_recv = data_ + head_;
    int len = static_cast<int>(recv(sockfd, sz_recv, size_ - head_, 0));
    if (len <= 0) {
      // LOG(ERROR) << "ReadFromSocket:sockfd<" << sockfd << "> size<" << size_
      //            << "> head<" << head_ << "> len<" << len << ">";
      return SOCKET_ERROR;
    }
    head_ += len;
    data_[head_] = 0;
    return len;
  }
  return 0;
}

bool Buffer::HasData() const {
  if (head_ >= sizeof(DataHeader)) {
    DataHeader* header = (DataHeader*)data_;
    return head_ >= header->data_length;
  }
  return false;
}
