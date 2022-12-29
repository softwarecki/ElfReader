#pragma once

#include <stdio.h>

#include <string>
#include <exception>

#define _T(x) L ## x
typedef std::wstring String;
#define Exception(x) std::runtime_error(x)