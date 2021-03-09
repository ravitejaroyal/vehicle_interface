#pragma once

#include <g3log/g3log.hpp>

//Adds the ERROR logging level to the levels supported by G3.
const LEVELS ERROR {WARNING.value +1, {"ERROR"}};
