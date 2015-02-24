
#pragma once

#ifdef _MSC_VER
#define THREAD_LOCAL __declspec(thread)
#else
#define THREAD_LOCAL thread_local
#endif
