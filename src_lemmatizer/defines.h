#pragma once

#include <locale>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
# define strtok_r strtok_s
#endif

#if defined(_WIN32) || defined(_WIN64)
# define LOCNAME "be_BY.UTF-8" // TODO: Test
#else
# define LOCNAME "be_BY.UTF-8"
#endif
