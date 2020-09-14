#ifndef BITBOARD_H_INCLUDED
#define BITBOARD_H_INCLUDED

#include "types.h"

class Bitboard {
public:
	static void init();

	Bitboard() {}
	Bitboard(const uint64_t v0, const uint64_t v1, const uint64_t v2, const uint64_t v3, const uint64_t v4) {
		this->p[0] = v0;
		this->p[1] = v1;
		this->p[2] = v2;
		this->p[3] = v3;
		this->p[4] = v4;
	}
	Bitboard(Square s);

	operator bool() const {
		return ((this->p[0] & 0x7FFFFFFFFFFFF)
			|| (this->p[1] & 0x7FFFFFFFFFFFF)
			|| (this->p[2] & 0x7FFFFFFFFFFFF)
			|| (this->p[3] & 0x7FFFFFFFFFFFF)
			|| (this->p[4] & 0x3FFFFFFFF)) ? true : false;
	}

	uint64_t merge() const { return (this->p[0] | this->p[1] | this->p[2] | this->p[3] | this->p[4]); }
	uint64_t cross() const { return (this->p[0] & this->p[1] & this->p[2] & this->p[3] & this->p[4]); }

	Bitboard& operator |= (const Bitboard& b) {
		this->p[0] |= b.p[0];
		this->p[1] |= b.p[1];
		this->p[2] |= b.p[2];
		this->p[3] |= b.p[3];
		this->p[4] |= b.p[4];
		return *this;
	}

	Bitboard& operator &= (const Bitboard& b) {
		this->p[0] &= b.p[0];
		this->p[1] &= b.p[1];
		this->p[2] &= b.p[2];
		this->p[3] &= b.p[3];
		this->p[4] &= b.p[4];
		return *this;
	}

	Bitboard& operator ^= (const Bitboard& b) {
		this->p[0] ^= b.p[0];
		this->p[1] ^= b.p[1];
		this->p[2] ^= b.p[2];
		this->p[3] ^= b.p[3];
		this->p[4] ^= b.p[4];
		return *this;
	}

	Bitboard operator ~ () const {
		return Bitboard(~this->p[0], ~this->p[1], ~this->p[2], ~this->p[3], ~this->p[4]);
	}
	Bitboard operator & (const Bitboard& rhs) const { return Bitboard(*this) &= rhs; }
	Bitboard operator | (const Bitboard& rhs) const { return Bitboard(*this) |= rhs; }
	Bitboard operator ^ (const Bitboard& rhs) const { return Bitboard(*this) ^= rhs; }

private:
	uint64_t p[5];
};

// Bitboardの1の升を'*'、0の升を'.'として表示する。デバッグ用。
std::ostream& operator<<(std::ostream& os, const Bitboard& board);

const Bitboard ALL1BB = Bitboard(UINT64_C(0x7FFFFFFFFFFFF), UINT64_C(0x7FFFFFFFFFFFF), UINT64_C(0x7FFFFFFFFFFFF), UINT64_C(0x7FFFFFFFFFFFF), UINT64_C(0x3FFFFFFFF));
const Bitboard ALL0BB = Bitboard(0, 0, 0, 0, 0);

extern Bitboard SquareBB[SQUARE_NB];
extern Bitboard CrossBB[SQUARE_NB];
extern Bitboard AroundBB[SQUARE_NB];
extern Bitboard LargeBB[SQUARE_NB];

inline Bitboard::Bitboard(Square s) {
	*this = SquareBB[s];
}

inline Bitboard squareBB(Square s) {
	return SquareBB[s];
}

inline Bitboard crossBB(Square s) {
	return CrossBB[s];
}

inline Bitboard aroundBB(Square s) {
	return AroundBB[s];
}

inline Bitboard largeBB(Square s) {
	return LargeBB[s];
}

inline Bitboard operator|(const Bitboard& b, Square s) {
	return b | SquareBB[s];
}

inline Bitboard operator&(const Bitboard& b, Square s) {
	return b & SquareBB[s];
}

inline Bitboard operator^(const Bitboard& b, Square s) {
	return b ^ SquareBB[s];
}

inline bool more_than_one(const Bitboard& b) {
	assert(!b.cross());
	return POPCNT64(b.merge()) > 1;
}

inline bool more_than_two(const Bitboard& b) {
	assert(!b.cross());
	return POPCNT64(b.merge()) > 2;
}

inline bool more_than_three(const Bitboard& b) {
	assert(!b.cross());
	return POPCNT64(b.merge()) > 3;
}

#endif // ifndef BITBOARD_H_INCLUDED