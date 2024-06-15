#pragma once
//#define DEBUG_SEED 1
#ifdef DEBUG_SEED
#pragma GCC optimize ("O0")
#define D_PRINT(A,...) hw.Print(A , ##__VA_ARGS__);
#else
#define D_PRINT(A,...)
#endif
