#ifndef POSITION_H_INCLUDED
#define POSITION_H_INCLUDED

#include <string>

#include "bitboard.h"
#include "types.h"

struct Ninja {
	Rank rank;
	File file;
};

struct Dog {
	int id;
	int targetId;
	int distance;
	Rank rank;
	File file;
};

struct Soul {
	Rank rank;
	File file;
};

class Position {
public:
	Position() = default;

	static bool input(Position& myPos, Position& oppPos);
	static void set_position(Position& pos);
	static int shortest_to_ninja(Position& pos, File x, Rank y, int targetId);
	static int shortest_to_avatar(Position& pos, File x, Rank y, Square target);
	static void simulateWalk(Position& pos, Direction dir, int id);
	static void simulateSkill(Position& pos, Skill use, Square target);
	static void simulateNinja(Position& pos, File x, Rank y, int id);
	static void simulateDog(Position& pos, Bitboard dogAppears, int scId);
	static void simulateAvatar(Position& pos, Square target);
	static void simulateRock(Position& pos, Direction dir, File x, Rank y);
	static void eraseDangerSouls(Position& myPos);

	bool empty(int file, int rank) const;
	Bitboard objects(Object o) const;
	Bitboard objects(Object o1, Object o2) const;
	Bitboard objects(Object o1, Object o2, Object o3) const;
	bool on_ninja(File x, Rank y) const;
	bool on_dog(File x, Rank y) const;
	bool on_soul(File x, Rank y) const;
	bool on_rock(File x, Rank y) const;
	bool on_wall(File x, Rank y) const;
	bool catch_by_dog(File x, Rank y) const;

	int get_skill_point() const;
	int get_height() const;
	int get_width() const;
	Ninja get_ninja(int id) const;
	Dog get_dog(int id) const;
	Soul get_soul(int id) const;
	int num_of_ninjas() const;
	int num_of_dogs() const;
	int num_of_souls() const;

private:
	Object board[SQUARE_NB]; // î’ñ 
	Bitboard byTypeBB[OBJECT_NB]; // objectñàÇÃÉrÉbÉgÉ{Å[Éh
	int skillPoint; // îEóÕ
	int height; // çÇÇ≥
	int width; // ïù
	Ninja ninja[MAX_NINJAS]; // îEé“ä÷òA
	int numOfNinjas; // îEé“ÇÃêî
	Dog dog[MAX_DOGS]; // îEå¢ä÷òA
	int numOfDogs; // îEå¢ÇÃêî
	Soul soul[MAX_SOULS]; // îEé“É\ÉEÉãä÷òA
	int numOfSouls; // îEé“É\ÉEÉãÇÃêî
	int skillCount[SKILL_NB]; // îEèpÇÃégópâÒêî
};

inline bool Position::empty(int file, int rank) const {
	return board[(File)file | (Rank)rank] == NO_OBJECT;
}

inline Bitboard Position::objects(Object o) const {
	return byTypeBB[o];
}

inline Bitboard Position::objects(Object o1, Object o2) const {
	return (byTypeBB[o1] | byTypeBB[o2]);
}

inline Bitboard Position::objects(Object o1, Object o2, Object o3) const {
	return (byTypeBB[o1] | byTypeBB[o2] | byTypeBB[o3]);
}

inline bool Position::on_ninja(File x, Rank y) const {
	return (byTypeBB[NINJA] & SquareBB[x | y]);
}

inline bool Position::on_dog(File x, Rank y) const {
	return (byTypeBB[DOG] & SquareBB[x | y]);
}

inline bool Position::on_soul(File x, Rank y) const {
	return (byTypeBB[SOUL] & SquareBB[x | y]);
}

inline bool Position::on_rock(File x, Rank y) const {
	return (byTypeBB[ROCK] & SquareBB[x | y]);
}

inline bool Position::on_wall(File x, Rank y) const {
	return (byTypeBB[WALL] & SquareBB[x | y]);
}

inline bool Position::catch_by_dog(File x, Rank y) const {
	return (crossBB(x | y) & byTypeBB[DOG]);
}

inline int Position::get_skill_point() const {
	return skillPoint;
}

inline int Position::get_height() const {
	return height;
}

inline int Position::get_width() const {
	return width;
}

inline Ninja Position::get_ninja(int id) const {
	return ninja[id];
}

inline Dog Position::get_dog(int id) const {
	return dog[id];
}

inline Soul Position::get_soul(int id) const {
	return soul[id];
}

inline int Position::num_of_ninjas() const {
	return numOfNinjas;
}

inline 	int Position::num_of_dogs() const {
	return numOfDogs;
}

inline int Position::num_of_souls() const {
	return numOfSouls;
}

#endif // ifndef POSITION_H_INCLUDED