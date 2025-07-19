#include "util.hpp"
using namespace util;

#include <sstream>


namespace util {

    PositionCmd::PositionCmd(std::istringstream& ss) : ss(ss) {}
    GoCmd::GoCmd(std::istringstream& ss) : ss(ss) {}

} // namespace util