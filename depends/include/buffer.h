/**
 * @file buffer.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @date 2025-05-15-06-39
 * @author Nemausa
 *
 */
#ifndef BUFFER_H_
#define BUFFER_H_

#include "cell.h"

class Buffer {
 public:
  explicit Buffer(size_t capacity);
  ~Buffer();

  /**
   * @brief Writes data into the buffer
   * @param data the byye to write
   * @param length the length
   * @return True if the data was written successfully; false if there was not
   * enough free space.
   */
  bool Push(const char* data, size_t length) noexcept;

  /**
   * @brief Consumes (removes) bytes from the front of the buffer.
   * @param bytes Number of the bytes to remove (must not exceed the current
   * readable size).
   */
  void Consume(size_t bytes);

  int WriteToSocket(int sockfd);
  int ReadFromSocket(int sockfd);

  bool HasData() const;

  const char* data() const noexcept { return data_; }
  inline bool NeedWrite() const { return head_ > 0; }
  size_t Size() const noexcept { return size_; }
  size_t Capacity() const noexcept { return head_; }

 private:
  char* data_ = nullptr;
  size_t head_ = 0;
  size_t size_ = 0;
  int full_count_ = 0;
};

#endif  // BUFFER_H_
