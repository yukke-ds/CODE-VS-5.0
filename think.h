#ifndef THINK_H_INCLUDED
#define THINK_H_INCLUDED

#include "position.h"
#include "types.h"

using namespace std;

const int dx[] = { 0, -1, 1, 0, 0 };
const int dy[] = { -1, 0, 0, 1, 0 };
const string action[] = { "U", "L", "R", "D", "N" };

namespace Think {

	struct LimitsType {
		int time;
		int numOfSkills;
		int skillCost[SKILL_NB];
	};

	struct BestRoute {
		BestRoute(int toDog, int toSoul) : toDog(toDog), toSoul(toSoul) {}
		int toDog;
		int toSoul;
	};

	struct Point {
		Point() { x = (File)-1; y = (Rank)-1; }
		Point(File x, Rank y) : x(x), y(y) {}
		bool operator== (const Point &p) const { return x == p.x && y == p.y; }

		File x;
		Rank y;
	};

	struct Search : public Point {
		Search() {}
		Search(Position pos, Direction dir, File x, Rank y, int distance)
			: pos(pos), dir(dir), Point(x, y), distance(distance) {}

		Position pos;
		Direction dir;
		int distance;
	};

	extern LimitsType Limits;

	void start(Position& myPos, Position& oppPos);
	void check_skills(Position& myPos, Position& oppPos, Skill& type);
	string search(Position& myPos, Position& oppPos, int id, int actNum);
	Bitboard dog_appears(Position& oppPos);
	Square longest_square(Position& pos, File x, Rank y);
	bool skills_to_me(Position& myPos, Position& oppPos);
	bool skills_to_opp(Position& myPos, Position& oppPos);
	bool lightning_square(Position& pos, Bitboard& target, int id);
	bool rockfall_square(Position& pos, Bitboard& target, int id);
	void avatar_square(Position& pos, Bitboard& target, int id);
	bool checkmate_square(Position& pos, File x, Rank y, int id);
	bool break_and_catch(Position& myPos, Bitboard& target, int id);
	bool cannot_move_rock(Position& pos, Direction dir, File nx, Rank ny);

} // namespace Search

#endif // ifndef SEARCH_H_INCLUDED