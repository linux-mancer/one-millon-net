/**
 * @file fd_set_wrapper.h
 *
 * @brief This message displayed in Doxygen Files index
 *
 * @ingroup PackageName
 * (note: this needs exactly one @defgroup somewhere)
 *
 * @author Nemausa
 */

#ifndef FD_SET_WRAPPER_H_
#define FD_SET_WRAPPER_H_

#include <sys/select.h>
#include <vector>
#include <cstddef>

class FdSetWrapper {
 public:
  explicit FdSetWrapper(int max_fds);

  void Clear();
  bool Add(int fd);
  void Remove(int fd);
  bool Contains(int fd) const;
  fd_set* Raw();
  int max_fds() const { return max_fds_; }

 private:
  int max_fds_{0};
  std::vector<char> buffer_;
};

#endif  // FD_SET_WRAPPER_H_
