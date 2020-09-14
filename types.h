#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <iostream>
#include <cassert>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cstdlib>

#if defined(_MSC_VER)
// Disable some silly and noisy warning from MSVC compiler
#pragma warning(disable: 4127) // Conditional expression is constant
#pragma warning(disable: 4146) // Unary minus operator applied to unsigned type
#pragma warning(disable: 4800) // Forcing value to bool 'true' or 'false'
#endif

// Predefined macros hell:
//
// __GNUC__           Compiler is gcc, Clang or Intel on Linux
// __INTEL_COMPILER   Compiler is Intel
// _MSC_VER           Compiler is MSVC or Intel on Windows
// _WIN32             Building on Windows (any)
// _WIN64             Building on Windows 64 bit

#if defined(_WIN64) && defined(_MSC_VER) // No Makefile used
#  include <intrin.h> // MSVC popcnt and bsfq instrinsics
#  define IS_64BIT
#  define USE_BSFQ
#  define USE_POPCNT // SSE4.2
//#  define USE_PEXT // AVX2
#endif

#if defined(USE_POPCNT) && defined(__INTEL_COMPILER) && defined(_MSC_VER)
#  include <nmmintrin.h> // Intel header for _mm_popcnt_u64() intrinsic
#endif

#if !defined(NO_PREFETCH) && (defined(__INTEL_COMPILER) || defined(_MSC_VER))
#  include <xmmintrin.h> // Intel and Microsoft header for _mm_prefetch()
#endif

#if defined(USE_PEXT)
#  include <immintrin.h> // Header for _pext_u64() intrinsic
#  define pext(b, m) _pext_u64(b, m)
#else
#  define pext(b, m) (0)
#endif

#ifdef USE_POPCNT
#  define POPCNT32(a) __popcnt32(a)
#  define POPCNT64(a) __popcnt64(a)
const bool HasPopCnt = true;
#else
const bool HasPopCnt = false;
#endif

#ifdef USE_PEXT
const bool HasPext = true;
#else
const bool HasPext = false;
#endif

#ifdef IS_64BIT
const bool Is64Bit = true;
#else
const bool Is64Bit = false;
#endif

// 1‚Å‚ ‚éÅ‰ºˆÊ‚Ìbit‚ÌbitˆÊ’u‚ð“¾‚é
inline int LSB32(uint32_t v) { assert(v != 0); unsigned long index; _BitScanForward(&index, v); return index; }
inline int LSB64(uint64_t v) { assert(v != 0); unsigned long index; _BitScanForward64(&index, v); return index; }

// 1‚Å‚ ‚éÅãˆÊ‚Ìbit‚ÌbitˆÊ’u‚ð“¾‚é
inline int MSB32(uint32_t v) { assert(v != 0); unsigned long index; _BitScanReverse(&index, v); return index; }
inline int MSB64(uint64_t v) { assert(v != 0); unsigned long index; _BitScanReverse64(&index, v); return index; }

enum Object {
	NO_OBJECT = 0,
	NINJA,
	DOG,
	SOUL,
	ROCK,
	WALL,

	OBJECT_NB = 6,
	MAX_NINJAS = 2,
	MAX_DOGS = 238,
	MAX_SOULS = 8,
	MAX_ROCKS = 238
};

enum Skill {
	HIGH_SPEED,
	MY_ROCKFALL,
	OPP_ROCKFALL,
	MY_LIGHTNING,
	OPP_LIGHTNING,
	MY_AVATAR,
	OPP_AVATAR,
	SWORD_ROTATION,

	NONE = 8,
	SKILL_NB = 8
};

enum Square {
	// SQs_—ñ¨17*14=238
	SQ1_1, SQ2_1, SQ3_1, SQ4_1, SQ5_1, SQ6_1, SQ7_1, SQ8_1, SQ9_1, SQ10_1, SQ11_1, SQ12_1, SQ13_1, SQ14_1, SQ15_1, SQ16_1, SQ17_1,
	SQ1_2, SQ2_2, SQ3_2, SQ4_2, SQ5_2, SQ6_2, SQ7_2, SQ8_2, SQ9_2, SQ10_2, SQ11_2, SQ12_2, SQ13_2, SQ14_2, SQ15_2, SQ16_2, SQ17_2,
	SQ1_3, SQ2_3, SQ3_3, SQ4_3, SQ5_3, SQ6_3, SQ7_3, SQ8_3, SQ9_3, SQ10_3, SQ11_3, SQ12_3, SQ13_3, SQ14_3, SQ15_3, SQ16_3, SQ17_3,
	SQ1_4, SQ2_4, SQ3_4, SQ4_4, SQ5_4, SQ6_4, SQ7_4, SQ8_4, SQ9_4, SQ10_4, SQ11_4, SQ12_4, SQ13_4, SQ14_4, SQ15_4, SQ16_4, SQ17_4,
	SQ1_5, SQ2_5, SQ3_5, SQ4_5, SQ5_5, SQ6_5, SQ7_5, SQ8_5, SQ9_5, SQ10_5, SQ11_5, SQ12_5, SQ13_5, SQ14_5, SQ15_5, SQ16_5, SQ17_5,
	SQ1_6, SQ2_6, SQ3_6, SQ4_6, SQ5_6, SQ6_6, SQ7_6, SQ8_6, SQ9_6, SQ10_6, SQ11_6, SQ12_6, SQ13_6, SQ14_6, SQ15_6, SQ16_6, SQ17_6,
	SQ1_7, SQ2_7, SQ3_7, SQ4_7, SQ5_7, SQ6_7, SQ7_7, SQ8_7, SQ9_7, SQ10_7, SQ11_7, SQ12_7, SQ13_7, SQ14_7, SQ15_7, SQ16_7, SQ17_7,
	SQ1_8, SQ2_8, SQ3_8, SQ4_8, SQ5_8, SQ6_8, SQ7_8, SQ8_8, SQ9_8, SQ10_8, SQ11_8, SQ12_8, SQ13_8, SQ14_8, SQ15_8, SQ16_8, SQ17_8,
	SQ1_9, SQ2_9, SQ3_9, SQ4_9, SQ5_9, SQ6_9, SQ7_9, SQ8_9, SQ9_9, SQ10_9, SQ11_9, SQ12_9, SQ13_9, SQ14_9, SQ15_9, SQ16_9, SQ17_9,
	SQ1_10, SQ2_10, SQ3_10, SQ4_10, SQ5_10, SQ6_10, SQ7_10, SQ8_10, SQ9_10, SQ10_10, SQ11_10, SQ12_10, SQ13_10, SQ14_10, SQ15_10, SQ16_10, SQ17_10,
	SQ1_11, SQ2_11, SQ3_11, SQ4_11, SQ5_11, SQ6_11, SQ7_11, SQ8_11, SQ9_11, SQ10_11, SQ11_11, SQ12_11, SQ13_11, SQ14_11, SQ15_11, SQ16_11, SQ17_11,
	SQ1_12, SQ2_12, SQ3_12, SQ4_12, SQ5_12, SQ6_12, SQ7_12, SQ8_12, SQ9_12, SQ10_12, SQ11_12, SQ12_12, SQ13_12, SQ14_12, SQ15_12, SQ16_12, SQ17_12,
	SQ1_13, SQ2_13, SQ3_13, SQ4_13, SQ5_13, SQ6_13, SQ7_13, SQ8_13, SQ9_13, SQ10_13, SQ11_13, SQ12_13, SQ13_13, SQ14_13, SQ15_13, SQ16_13, SQ17_13,
	SQ1_14, SQ2_14, SQ3_14, SQ4_14, SQ5_14, SQ6_14, SQ7_14, SQ8_14, SQ9_14, SQ10_14, SQ11_14, SQ12_14, SQ13_14, SQ14_14, SQ15_14, SQ16_14, SQ17_14,
	SQ_NONE,

	SQUARE_NB = 238
};

enum File {
	FILE_1, FILE_2, FILE_3, FILE_4, FILE_5, FILE_6, FILE_7,
	FILE_8, FILE_9, FILE_10, FILE_11, FILE_12, FILE_13, FILE_14,

	FILE_ONE = 1,
	FILE_NB
};

enum Rank {
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9,
	RANK_10, RANK_11, RANK_12, RANK_13, RANK_14, RANK_15, RANK_16, RANK_17,

	RANK_ONE = 1,
	RANK_NB
};

const File SquareToFile[SQUARE_NB] = {
	FILE_1, FILE_1, FILE_1, FILE_1, FILE_1, FILE_1, FILE_1, FILE_1, FILE_1, FILE_1, FILE_1, FILE_1, FILE_1, FILE_1, FILE_1, FILE_1, FILE_1,
	FILE_2, FILE_2, FILE_2, FILE_2, FILE_2, FILE_2, FILE_2, FILE_2, FILE_2, FILE_2, FILE_2, FILE_2, FILE_2, FILE_2, FILE_2, FILE_2, FILE_2,
	FILE_3, FILE_3, FILE_3, FILE_3, FILE_3, FILE_3, FILE_3, FILE_3, FILE_3, FILE_3, FILE_3, FILE_3, FILE_3, FILE_3, FILE_3, FILE_3, FILE_3,
	FILE_4, FILE_4, FILE_4, FILE_4, FILE_4, FILE_4, FILE_4, FILE_4, FILE_4, FILE_4, FILE_4, FILE_4, FILE_4, FILE_4, FILE_4, FILE_4, FILE_4,
	FILE_5, FILE_5, FILE_5, FILE_5, FILE_5, FILE_5, FILE_5, FILE_5, FILE_5, FILE_5, FILE_5, FILE_5, FILE_5, FILE_5, FILE_5, FILE_5, FILE_5,
	FILE_6, FILE_6, FILE_6, FILE_6, FILE_6, FILE_6, FILE_6, FILE_6, FILE_6, FILE_6, FILE_6, FILE_6, FILE_6, FILE_6, FILE_6, FILE_6, FILE_6,
	FILE_7, FILE_7, FILE_7, FILE_7, FILE_7, FILE_7, FILE_7, FILE_7, FILE_7, FILE_7, FILE_7, FILE_7, FILE_7, FILE_7, FILE_7, FILE_7, FILE_7,
	FILE_8, FILE_8, FILE_8, FILE_8, FILE_8, FILE_8, FILE_8, FILE_8, FILE_8, FILE_8, FILE_8, FILE_8, FILE_8, FILE_8, FILE_8, FILE_8, FILE_8,
	FILE_9, FILE_9, FILE_9, FILE_9, FILE_9, FILE_9, FILE_9, FILE_9, FILE_9, FILE_9, FILE_9, FILE_9, FILE_9, FILE_9, FILE_9, FILE_9, FILE_9,
	FILE_10, FILE_10, FILE_10, FILE_10, FILE_10, FILE_10, FILE_10, FILE_10, FILE_10, FILE_10, FILE_10, FILE_10, FILE_10, FILE_10, FILE_10, FILE_10, FILE_10,
	FILE_11, FILE_11, FILE_11, FILE_11, FILE_11, FILE_11, FILE_11, FILE_11, FILE_11, FILE_11, FILE_11, FILE_11, FILE_11, FILE_11, FILE_11, FILE_11, FILE_11,
	FILE_12, FILE_12, FILE_12, FILE_12, FILE_12, FILE_12, FILE_12, FILE_12, FILE_12, FILE_12, FILE_12, FILE_12, FILE_12, FILE_12, FILE_12, FILE_12, FILE_12,
	FILE_13, FILE_13, FILE_13, FILE_13, FILE_13, FILE_13, FILE_13, FILE_13, FILE_13, FILE_13, FILE_13, FILE_13, FILE_13, FILE_13, FILE_13, FILE_13, FILE_13,
	FILE_14, FILE_14, FILE_14, FILE_14, FILE_14, FILE_14, FILE_14, FILE_14, FILE_14, FILE_14, FILE_14, FILE_14, FILE_14, FILE_14, FILE_14, FILE_14, FILE_14
};

const Rank SquareToRank[SQUARE_NB] = {
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_10, RANK_11, RANK_12, RANK_13, RANK_14, RANK_15, RANK_16, RANK_17,
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_10, RANK_11, RANK_12, RANK_13, RANK_14, RANK_15, RANK_16, RANK_17,
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_10, RANK_11, RANK_12, RANK_13, RANK_14, RANK_15, RANK_16, RANK_17,
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_10, RANK_11, RANK_12, RANK_13, RANK_14, RANK_15, RANK_16, RANK_17,
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_10, RANK_11, RANK_12, RANK_13, RANK_14, RANK_15, RANK_16, RANK_17,
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_10, RANK_11, RANK_12, RANK_13, RANK_14, RANK_15, RANK_16, RANK_17,
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_10, RANK_11, RANK_12, RANK_13, RANK_14, RANK_15, RANK_16, RANK_17,
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_10, RANK_11, RANK_12, RANK_13, RANK_14, RANK_15, RANK_16, RANK_17,
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_10, RANK_11, RANK_12, RANK_13, RANK_14, RANK_15, RANK_16, RANK_17,
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_10, RANK_11, RANK_12, RANK_13, RANK_14, RANK_15, RANK_16, RANK_17,
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_10, RANK_11, RANK_12, RANK_13, RANK_14, RANK_15, RANK_16, RANK_17,
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_10, RANK_11, RANK_12, RANK_13, RANK_14, RANK_15, RANK_16, RANK_17,
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_10, RANK_11, RANK_12, RANK_13, RANK_14, RANK_15, RANK_16, RANK_17,
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_9, RANK_10, RANK_11, RANK_12, RANK_13, RANK_14, RANK_15, RANK_16, RANK_17
};

enum Direction {
	UP,
	LEFT,
	RIGHT,
	DOWN,
	STAY,

	DIREC_NB = 5
};

enum Distance : int {
	INFINITE = 10000
};

#define ENABLE_BASE_OPERATORS_ON(T)                             \
inline T operator+(T d1, T d2) { return T(int(d1) + int(d2)); } \
inline T operator-(T d1, T d2) { return T(int(d1) - int(d2)); } \
inline T operator*(int i, T d) { return T(i * int(d)); }        \
inline T operator*(T d, int i) { return T(int(d) * i); }        \
inline T operator-(T d) { return T(-int(d)); }                  \
inline T& operator+=(T& d1, T d2) { return d1 = d1 + d2; }      \
inline T& operator-=(T& d1, T d2) { return d1 = d1 - d2; }      \
inline T& operator*=(T& d, int i) { return d = T(int(d) * i); }

#define ENABLE_FULL_OPERATORS_ON(T)                             \
ENABLE_BASE_OPERATORS_ON(T)                                     \
inline T& operator++(T& d) { return d = T(int(d) + 1); }        \
inline T& operator--(T& d) { return d = T(int(d) - 1); }        \
inline T operator/(T d, int i) { return T(int(d) / i); }        \
inline int operator/(T d1, T d2) { return int(d1) / int(d2); }  \
inline T& operator/=(T& d, int i) { return d = T(int(d) / i); }

ENABLE_FULL_OPERATORS_ON(Object);
ENABLE_FULL_OPERATORS_ON(Skill);
ENABLE_FULL_OPERATORS_ON(Square);
ENABLE_FULL_OPERATORS_ON(File);
ENABLE_FULL_OPERATORS_ON(Rank);
ENABLE_FULL_OPERATORS_ON(Direction);

#undef ENABLE_FULL_OPERATORS_ON
#undef ENABLE_BASE_OPERATORS_ON

inline Square operator | (File f, Rank r) {
	Square sq = (Square)(f * 17 + r);
	return sq;
}

inline Rank operator>>(std::istream& is, Rank& r) {
	int i;
	is >> i;
	r = (Rank)i;
	return r;
}

inline File operator>>(std::istream& is, File& f) {
	int i;
	is >> i;
	f = (File)i;
	return f;
}

inline File file_of(Square s) {
	return SquareToFile[s];
}

inline Rank rank_of(Square s) {
	return SquareToRank[s];
}

#endif // ifndef TYPES_H_INCLUDED