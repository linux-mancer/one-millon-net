#include "byte_stream.h"
#include <cassert>
#include <glog/logging.h>

ByteStream::ByteStream(char* data, size_t size, bool take_ownership)
    : data_(data), capacity_(size) {
  assert(data != nullptr);
  if (take_ownership) {
    owned_data_.reset(data_);
  }
  LOG(INFO) << "Glog is working!";
}

ByteStream::ByteStream(size_t size)
    : owned_data_(new char[size]), data_(owned_data_.get()), capacity_(size) {
  assert(size > 0);

}
