#ifndef UNUSED_H_
#define UNUSED_H_

#include <string>

template<typename T>
void PARAMETER_UNUSED(T const& /*parameter*/, std::string const& /*reason*/) {}

#endif /* end of include guard */
