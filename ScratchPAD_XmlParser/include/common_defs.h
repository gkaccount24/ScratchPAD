#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#define LOGGING_ENABLED

#ifdef LOGGING_ENABLED

#include <iostream>

using std::cout;
using std::endl;

#endif

#include <vector>
#include <string>
#include <tuple>

#include <cstddef>
#include <cstdint>
#include <cassert>

using std::size_t;
using std::string;
using std::vector;
using std::pair;
using std::tuple;

using std::make_tuple;
using std::make_pair;

using std::move;
using std::begin;
using std::end;

#endif