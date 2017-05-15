#pragma once

#include <locale>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
# define strtok_r strtok_s
#endif

#if defined(_WIN32) || defined(_WIN64)
# define LOCNAME "Russian_Russia.1251"
#else
# define LOCNAME "ru_RU.CP1251"
#endif
