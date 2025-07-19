#pragma once
#include <iostream>

#ifndef ENABLE_LOGGING
#define ENABLE_LOGGING 0
#endif

#if ENABLE_LOGGING
#define LOG(x) do { std::cerr << x; } while(0)
#else
#define LOG(x) do {} while(0)
#endif
