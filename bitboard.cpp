#include <iostream>
#include <fstream>

#include "bitboard.h"

using namespace std;

Bitboard SquareBB[SQUARE_NB];
Bitboard CrossBB[SQUARE_NB];
Bitboard AroundBB[SQUARE_NB];
Bitboard LargeBB[SQUARE_NB];

void Bitboard::init() {

	// SquareBB
	for (Square s = SQ1_1; s <= SQ17_14; ++s)
	{
		Rank r = rank_of(s);
		File f = file_of(s);
		SquareBB[s].p[0] = (f <= FILE_3) ? ((uint64_t)1 << ((f * 17) + r)) : 0;
		SquareBB[s].p[1] = (f >= FILE_4 && f <= FILE_6) ? ((uint64_t)1 << ((f - FILE_4) * 17 + r)) : 0;
		SquareBB[s].p[2] = (f >= FILE_7 && f <= FILE_9) ? ((uint64_t)1 << ((f - FILE_7) * 17 + r)) : 0;
		SquareBB[s].p[3] = (f >= FILE_10 && f <= FILE_12) ? ((uint64_t)1 << ((f - FILE_10) * 17 + r)) : 0;
		SquareBB[s].p[4] = (f >= FILE_13) ? ((uint64_t)1 << ((f - FILE_13) * 17 + r)) : 0;
	}

	// CrossBB
	for (Square s = SQ1_1; s <= SQ17_14; ++s)
	{
		Rank r = rank_of(s);
		File f = file_of(s);
		if (r == RANK_1 && f == FILE_1) // 下右
			CrossBB[s] = (SquareBB[s + 1] | SquareBB[s + 17]);
		else if (r == RANK_1 && f == FILE_14) // 下左
			CrossBB[s] = (SquareBB[s + 1] | SquareBB[s - 17]);
		else if (r == RANK_17 && f == FILE_1) // 上右
			CrossBB[s] = (SquareBB[s - 1] | SquareBB[s + 17]);
		else if (r == RANK_17 && f == FILE_14) // 上左
			CrossBB[s] = (SquareBB[s - 1] | SquareBB[s - 17]);
		else if (r == RANK_1) // 下左右
			CrossBB[s] = (SquareBB[s + 1] | SquareBB[s - 17] | SquareBB[s + 17]);
		else if (r == RANK_17) // 上左右
			CrossBB[s] = (SquareBB[s - 1] | SquareBB[s - 17] | SquareBB[s + 17]);
		else if (f == FILE_1) // 上下右
			CrossBB[s] = (SquareBB[s - 1] | SquareBB[s + 1] | SquareBB[s + 17]);
		else if (f == FILE_14) // 上下左
			CrossBB[s] = (SquareBB[s - 1] | SquareBB[s + 1] | SquareBB[s - 17]);
		else // 上下左右
			CrossBB[s] = (SquareBB[s - 1] | SquareBB[s + 1] | SquareBB[s - 17] | SquareBB[s + 17]);
	}

	// AroundBB
	for (Square s = SQ1_1; s <= SQ17_14; ++s)
	{
		Rank r = rank_of(s);
		File f = file_of(s);
		if (r == RANK_1 && f == FILE_1)
			AroundBB[s] = CrossBB[s] | SquareBB[s + 18];
		else if (r == RANK_1 && f == FILE_14)
			AroundBB[s] = CrossBB[s] | SquareBB[s - 16];
		else if (r == RANK_17 && f == FILE_1)
			AroundBB[s] = CrossBB[s] | SquareBB[s + 16];
		else if (r == RANK_17 && f == FILE_14)
			AroundBB[s] = CrossBB[s] | SquareBB[s - 18];
		else if (r == RANK_1)
			AroundBB[s] = CrossBB[s] | SquareBB[s - 16] | SquareBB[s + 18];
		else if (r == RANK_17)
			AroundBB[s] = CrossBB[s] | SquareBB[s - 18] | SquareBB[s + 16];
		else if (f == FILE_1)
			AroundBB[s] = CrossBB[s] | SquareBB[s + 16] | SquareBB[s + 18];
		else if (f == FILE_14)
			AroundBB[s] = CrossBB[s] | SquareBB[s - 16] | SquareBB[s - 18];
		else
			AroundBB[s] = CrossBB[s]
			| SquareBB[s - 16] | SquareBB[s - 18] | SquareBB[s + 16] | SquareBB[s + 18];
	}

	// LargeBB
	for (Square s = SQ1_1; s <= SQ17_14; ++s)
	{
		Rank r = rank_of(s);
		File f = file_of(s);
		if ((r == RANK_1 && (f == FILE_1 || f == FILE_2))
			|| (r == RANK_2 && (f == FILE_1 || f == FILE_2)))
			LargeBB[s] = AroundBB[s] | SquareBB[s + 2] | SquareBB[s + 34];
		else if ((r == RANK_1 && (f == FILE_14 || f == FILE_13))
			|| (r == RANK_2 && (f == FILE_14 || f == FILE_13)))
			LargeBB[s] = AroundBB[s] | SquareBB[s + 2] | SquareBB[s - 34];
		else if ((r == RANK_17 && (f == FILE_1 || f == FILE_2))
			|| (r == RANK_16 && (f == FILE_1 || f == FILE_2)))
			LargeBB[s] = AroundBB[s] | SquareBB[s - 2] | SquareBB[s + 34];
		else if ((r == RANK_17 && (f == FILE_14 || f == FILE_13))
			|| (r == RANK_16 && (f == FILE_14 || f == FILE_13)))
			LargeBB[s] = AroundBB[s] | SquareBB[s - 2] | SquareBB[s - 34];
		else if (r == RANK_1 || r == RANK_2)
			LargeBB[s] = AroundBB[s] | SquareBB[s + 2] | SquareBB[s - 34] | SquareBB[s + 34];
		else if (r == RANK_17 || r == RANK_16)
			LargeBB[s] = AroundBB[s] | SquareBB[s - 2] | SquareBB[s - 34] | SquareBB[s + 34];
		else if (f == FILE_1 || f == FILE_2)
			LargeBB[s] = AroundBB[s] | SquareBB[s - 2] | SquareBB[s + 2] | SquareBB[s + 34];
		else if (f == FILE_14 || f == FILE_13)
			LargeBB[s] = AroundBB[s] | SquareBB[s - 2] | SquareBB[s + 2] | SquareBB[s - 34];
		else
			LargeBB[s] = AroundBB[s] | SquareBB[s - 2] | SquareBB[s + 2] | SquareBB[s - 34] | SquareBB[s + 34];
	}
}

ostream& operator<<(ostream& os, const Bitboard& board)
{
	for (Rank r = RANK_1; r <= RANK_17; ++r)
	{
		for (File f = FILE_1; f <= FILE_14; ++f)
			os << ((board & (f | r)) ? " *" : " .");
		os << endl;
	}
	os << endl;
	return os;
}