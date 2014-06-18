#ifndef UNUSED_H_
#define UNUSED_H_

#include <string>

template<typename T>
void PARAMETER_UNUSED(T& parameter, std::string const& reason) {
  (void) parameter;
  (void) reason;
}

#endif /* end of include guard */
