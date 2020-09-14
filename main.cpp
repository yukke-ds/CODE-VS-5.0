#include <iostream>
#include <string>

#include "bitboard.h"
#include "position.h"
#include "think.h"

using namespace Think;

int main() {

	std::cout << "yukke" << std::endl;

	Bitboard::init();

	Position myPos, oppPos;
	while (Position::input(myPos, oppPos)) // ƒƒCƒ“ƒ‹[ƒv
		Think::start(myPos, oppPos);

	return 0;
}