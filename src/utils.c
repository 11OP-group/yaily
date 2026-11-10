#include "yaily/utils.h"

bool cmp_string(const char *lhs, int lhs_len, const char *rhs, int rhs_len) {
  if (lhs_len != rhs_len) {
    return false;
  }
  for (int i = 0; i < lhs_len; i++) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }

  return true;
}
