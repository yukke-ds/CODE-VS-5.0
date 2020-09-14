#include <iostream>
#include <fstream>
#include <queue>
#include <vector>

#include "position.h"
#include "think.h"

using namespace std;
using namespace Think;

namespace {
	int ascending(const void *c1, const void *c2)
	{
		Dog buf1 = *(Dog*)c1;
		Dog buf2 = *(Dog*)c2;

		//�����\�[�g
		if (buf1.distance < buf2.distance)
			return -1;
		else if (buf1.distance > buf2.distance)
			return 1;
		else if (buf1.distance == buf2.distance)
		{
			if (buf1.id < buf2.id)
				return -1;
			else
				return 1;
		}
	}
}

bool Position::input(Position& myPos, Position& oppPos) {

	Think::LimitsType limits;

	if (!(cin >> limits.time)) // ��������
		return false;

	cin >> limits.numOfSkills; // �E�p�̐�
	for (Skill s = HIGH_SPEED; s <= SWORD_ROTATION; ++s)
		cin >> limits.skillCost[s]; // �E�p�̃R�X�g

	Limits = limits;

	for (Object o = NO_OBJECT; o <= WALL; ++o) // �r�b�g�{�[�h��������
		myPos.byTypeBB[o] = oppPos.byTypeBB[o] = ALL0BB;

	set_position(myPos); // �����̋ǖ�
	set_position(oppPos); // ����̋ǖ�
	
	return true;
}

void Position::set_position(Position& pos) {

	// �E�́A�����A��
	cin >> pos.skillPoint;
	cin >> pos.height;
	cin >> pos.width;

	// �Ֆ�
	for (Rank r = RANK_1; r <= RANK_17; ++r)
	{
		string s;
		cin >> s;
		for (File f = FILE_1; f <= FILE_14; ++f)
		{
			switch (s[f]) {
			case '_':
				pos.board[f | r] = NO_OBJECT;
				break;

			case 'O':
				pos.board[f | r] = ROCK;
				pos.byTypeBB[ROCK] |= (f | r);
				break;

			case 'W':
				pos.board[f | r] = WALL;
				pos.byTypeBB[WALL] |= (f | r);
				break;

			default: assert(false);
			}
		}
	}

	// �E�Ҋ֘A
	cin >> pos.numOfNinjas;
	for (int i = 0; i < pos.numOfNinjas; ++i) {
		int id;
		cin >> id;
		cin >> pos.ninja[i].rank;
		cin >> pos.ninja[i].file;
		pos.board[pos.ninja[i].file | pos.ninja[i].rank] = NINJA;
		pos.byTypeBB[NINJA] |= (pos.ninja[i].file | pos.ninja[i].rank);
	}

	// �E���֘A
	cin >> pos.numOfDogs;
	for (int i = 0; i < pos.numOfDogs; ++i) {
		cin >> pos.dog[i].id;
		cin >> pos.dog[i].rank;
		cin >> pos.dog[i].file;
		pos.board[pos.dog[i].file | pos.dog[i].rank] = DOG;
		pos.byTypeBB[DOG] |= (pos.dog[i].file | pos.dog[i].rank);
	}

	// �E�҃\�E���֘A
	cin >> pos.numOfSouls;
	for (int i = 0; i < pos.numOfSouls; ++i) {
		cin >> pos.soul[i].rank;
		cin >> pos.soul[i].file;
		pos.board[pos.soul[i].file | pos.soul[i].rank] = SOUL;
		pos.byTypeBB[SOUL] |= (pos.soul[i].file | pos.soul[i].rank);
	}

	// �E�p�̎g�p��
	for (Skill s = HIGH_SPEED; s <= SWORD_ROTATION; ++s)
		cin >> pos.skillCount[s]; // �E�p�̃R�X�g
}

void Position::simulateWalk(Position& pos, Direction dir, int id)
{
	assert(dir != STAY);

	// �i�ޗ\��̏ꏊ
	File nx = pos.get_ninja(id).file + (File)dx[dir];
	Rank ny = pos.get_ninja(id).rank + (Rank)dy[dir];

	// �E�҂̈ʒu+�����ɕǂ�����
	if (pos.on_wall(nx, ny))
		return;

	// �E�҂̈ʒu+�����Ɋ₪����
	if (pos.on_rock(nx, ny))
		if (cannot_move_rock(pos, dir, nx, ny)) // ���ꂪ�������Ȃ�
			return;

	// �Ֆʂ��X�V����
	// �E�҃\�E��������΁A������Ƃ݂Ȃ�
	if (pos.on_soul(nx, ny)) {
		pos.skillPoint += 2;
		pos.board[nx | ny] = NO_OBJECT;
		pos.byTypeBB[SOUL] ^= squareBB(nx | ny);
	}

	// �E��
	// board
	pos.board[pos.ninja[id].file | pos.ninja[id].rank] = NO_OBJECT;
	pos.board[nx | ny] = NINJA;
	// byTypeBB
	pos.byTypeBB[NINJA] ^= squareBB(pos.ninja[id].file | pos.ninja[id].rank);
	pos.byTypeBB[NINJA] |= squareBB(nx | ny);
	// file��rank
	pos.ninja[id].file = (File)nx;
	pos.ninja[id].rank = (Rank)ny;

	// �₪����΁A�������Ƃ݂Ȃ�
	if (pos.on_rock(nx, ny)) {
		pos.board[nx | ny] = NO_OBJECT;
		pos.byTypeBB[ROCK] ^= squareBB(nx | ny);

		switch (dir) {
		case UP:
			pos.board[(File)nx | (Rank)(ny - 1)] = ROCK;
			pos.byTypeBB[ROCK] |= squareBB((File)nx | (Rank)(ny - 1));
			break;

		case RIGHT:
			pos.board[(File)(nx + 1) | (Rank)ny] = ROCK;
			pos.byTypeBB[ROCK] |= squareBB((File)(nx + 1) | (Rank)ny);
			break;

		case DOWN:
			pos.board[(File)nx | (Rank)(ny + 1)] = ROCK;
			pos.byTypeBB[ROCK] |= squareBB((File)nx | (Rank)(ny + 1));
			break;

		case LEFT:
			pos.board[(File)(nx - 1) | (Rank)ny] = ROCK;
			pos.byTypeBB[ROCK] |= squareBB((File)(nx - 1) | (Rank)ny);
			break;

		default: assert(false);
		}
	}
}

void Position::simulateSkill(Position& pos, Skill use, Square target)
{
	switch (use)
	{
	case OPP_ROCKFALL:
		pos.skillPoint -= Limits.skillCost[OPP_ROCKFALL]; // �E�͂��R�X�g�����炷
		pos.board[target] = NO_OBJECT;
		pos.byTypeBB[ROCK] |= squareBB(target);
		break;

	case MY_LIGHTNING:
		pos.skillPoint -= Limits.skillCost[MY_LIGHTNING]; // �E�͂��R�X�g�����炷
		pos.board[target] = NO_OBJECT;
		pos.byTypeBB[ROCK] ^= squareBB(target);
		break;

	case MY_AVATAR:
		pos.skillPoint -= Limits.skillCost[MY_AVATAR]; // �E�͂��R�X�g�����炷
		//simulateAvatar(pos, target);
		break;

	case SWORD_ROTATION:
		pos.skillPoint -= Limits.skillCost[SWORD_ROTATION]; // �E�͂��R�X�g�����炷
		pos.board[target] = NO_OBJECT;
		pos.byTypeBB[DOG] ^= squareBB(target);

		break;

	default: 
		cerr << use << endl;
		assert(false);
	}
}

void Position::simulateNinja(Position& pos, File x, Rank y, int id)
{
	// �E��
	// board
	pos.board[pos.ninja[id].file | pos.ninja[id].rank] = NO_OBJECT;
	pos.board[x | y] = NINJA;
	// byTypeBB
	pos.byTypeBB[NINJA] ^= squareBB(pos.ninja[id].file | pos.ninja[id].rank);
	pos.byTypeBB[NINJA] |= squareBB(x | y);
	// file��rank
	pos.ninja[id].file = x;
	pos.ninja[id].rank = y;
}

void Position::simulateDog(Position& pos, Bitboard dogAppears, int scId)
{
	if (dogAppears)
	{
		Square target = longest_square(pos, pos.get_ninja(scId).file, pos.get_ninja(scId).rank);

		pos.dog[pos.numOfDogs].id = pos.numOfDogs;
		pos.dog[pos.numOfDogs].rank = rank_of(target);
		pos.dog[pos.numOfDogs].file = file_of(target);
		pos.board[target] = DOG;
		pos.byTypeBB[DOG] |= squareBB(target);
		pos.numOfDogs++;
	}

	for (int dogId = 0; dogId < pos.numOfDogs; ++dogId)
	{
		int shortest = INFINITE;
		int shortestId = -1;

		for (int ninId = 0; ninId < pos.numOfNinjas; ++ninId)
		{
			int shortestToNinja = shortest_to_ninja(pos, pos.get_dog(dogId).file, pos.get_dog(dogId).rank, ninId);

			// �ŒZ�o�H��
			if (shortestToNinja < shortest) {
				shortest = shortestToNinja;
				shortestId = ninId;
			}
		}

		pos.dog[dogId].distance = shortest;
		pos.dog[dogId].targetId = shortestId;
	}

	// �߂������璲�ׂ邽�߂Ƀ\�[�g
	qsort(pos.dog, pos.numOfDogs, sizeof(Dog), ascending);

	Bitboard dog = ALL0BB;
	for (int id = 0; id < pos.numOfDogs; ++id)
	{
		// �ŒZ�o�H���Ȃ���΂��̂܂��
		if (pos.dog[id].distance == INFINITE) {
			dog |= (pos.get_dog(id).file | pos.get_dog(id).rank);
			continue;
		}

		int targetId = pos.get_dog(id).targetId;
		for (Direction d = UP; d <= DOWN; ++d)
		{
			File nx = pos.get_dog(id).file + (File)dx[d];
			Rank ny = pos.get_dog(id).rank + (Rank)dy[d];

			if (   !pos.on_rock(nx, ny) // ��łȂ�
				&& !pos.on_wall(nx, ny) // �ǂłȂ�
				&& !(squareBB(nx | ny) & dog) // ���łȂ�
				&& (shortest_to_ninja(pos, pos.get_dog(id).file, pos.get_dog(id).rank, targetId)
					- shortest_to_ninja(pos, nx, ny, targetId)) == 1)
			{
				dog |= (nx | ny);
				pos.dog[id].file = nx;
				pos.dog[id].rank = ny;
				break;
			}

			// �����𖞂����Ȃ���Γ����ꏊ
			if (d == DOWN)
				dog |= ((File)pos.get_dog(id).file | (Rank)pos.get_dog(id).rank);
		}
	}

	pos.byTypeBB[DOG] = dog;
}

void Position::simulateAvatar(Position& pos, Square target)
{
	for (int id = 0; id < pos.numOfDogs; ++id)
	{
		// �ŒZ�o�H��
		pos.dog[id].distance = shortest_to_avatar(pos, pos.get_dog(id).file, pos.get_dog(id).rank, target);
	}

	// �߂������璲�ׂ邽�߂Ƀ\�[�g
	qsort(pos.dog, pos.numOfDogs, sizeof(Dog), ascending);

	Bitboard dog = ALL0BB;
	for (int id = 0; id < pos.numOfDogs; ++id)
	{
		// �ŒZ�o�H���Ȃ���΂��̂܂��
		if (pos.dog[id].distance == INFINITE) {
			dog |= (pos.get_dog(id).file | pos.get_dog(id).rank);
			continue;
		}

		for (Direction d = UP; d <= DOWN; ++d)
		{
			File nx = pos.get_dog(id).file + (File)dx[d];
			Rank ny = pos.get_dog(id).rank + (Rank)dy[d];

			if (!pos.on_rock(nx, ny) // ��łȂ�
				&& !pos.on_wall(nx, ny) // �ǂłȂ�
				&& !(squareBB((File)nx | (Rank)ny) & dog) // ���łȂ�
				&& (shortest_to_avatar(pos, pos.get_dog(id).file, pos.get_dog(id).rank, target)
					- shortest_to_avatar(pos, nx, ny, target)) == 1)
			{
				dog |= ((File)nx | (Rank)ny);
				pos.dog[id].file = (File)nx;
				pos.dog[id].rank = (Rank)ny;
				break;
			}

			// �����𖞂����Ȃ���Γ����ꏊ
			if (d == DOWN)
				dog |= ((File)pos.get_dog(id).file | (Rank)pos.get_dog(id).rank);
		}
	}

	pos.byTypeBB[DOG] = dog;
}

void Position::simulateRock(Position& pos, Direction dir, File x, Rank y)
{
	pos.byTypeBB[ROCK] ^= squareBB(x | y);

	switch (dir) {
	case UP:
		pos.board[x | (y - RANK_ONE)] = ROCK;
		pos.byTypeBB[ROCK] |= squareBB(x | (y - RANK_ONE));
		break;

	case LEFT:
		pos.board[(x - FILE_ONE) | y] = ROCK;
		pos.byTypeBB[ROCK] |= squareBB((x - FILE_ONE) | y);
		break;

	case RIGHT:
		pos.board[(x + FILE_ONE) | y] = ROCK;
		pos.byTypeBB[ROCK] |= squareBB((x + FILE_ONE) | y);
		break;

	case DOWN:
		pos.board[x | (y + RANK_ONE)] = ROCK;
		pos.byTypeBB[ROCK] |= squareBB(x | (y + RANK_ONE));
		break;

	default: assert(false);
	}
}

int Position::shortest_to_ninja(Position& pos, File x, Rank y, int targetId)
{
	int shortest = INFINITE;
	Square target = pos.get_ninja(targetId).file | pos.get_ninja(targetId).rank;
	queue<Search> open;
	vector<vector<bool>> closed(pos.get_height(), vector<bool>(pos.get_width(), false));

	closed[y][x] = true;
	open.push(Search(pos, STAY, x, y, 0));

	// queue�̒��g���Ȃ��Ȃ�܂�
	while (!open.empty())
	{
		// �擪�̗v�f�����o��
		Search sc = open.front(); open.pop();

		// target�̏ꏊ�ŁA���Z���o�H�ł����
		if ((sc.x | sc.y) == target && sc.distance < shortest) {
			shortest = sc.distance;
		}

		// (sc.x, sc.y)�ɂ͉����Ȃ�������Ɉړ�������
		for (Direction d = UP; d <= DOWN; ++d) {
			File nx = sc.x + (File)dx[d];
			Rank ny = sc.y + (Rank)dy[d];

			// �E���̈ʒu+�����ɕǂ�����
			if (pos.on_wall(nx, ny))
				continue;

			// �E���̈ʒu+�����Ɋ₪����
			if (pos.on_rock(nx, ny))
				continue;

			// �T���ς݂̏ꏊ�ł����
			if (closed[ny][nx]) continue;

			// (nx, ny)�͒T�����܂��� = true
			closed[ny][nx] = true;

			// �����͐i�ނ��Ƃ��ł��邽��queue�֒ǉ�����
			open.push(Search(pos, d, nx, ny, sc.distance + 1));
		}
	}

	return shortest;
}

int Position::shortest_to_avatar(Position& pos, File x, Rank y, Square target)
{
	int shortest = INFINITE;
	queue<Search> open;
	vector<vector<bool>> closed(pos.get_height(), vector<bool>(pos.get_width(), false));

	closed[y][x] = true;
	open.push(Search(pos, STAY, x, y, 0));

	// queue�̒��g���Ȃ��Ȃ�܂�
	while (!open.empty())
	{
		// �擪�̗v�f�����o��
		Search sc = open.front(); open.pop();

		// target�̏ꏊ�ŁA���Z���o�H�ł����
		if ((sc.x | sc.y) == target && sc.distance < shortest) {
			shortest = sc.distance;
		}

		// (sc.x, sc.y)�ɂ͉����Ȃ�������Ɉړ�������
		for (Direction d = UP; d <= DOWN; ++d) {
			File nx = sc.x + (File)dx[d];
			Rank ny = sc.y + (Rank)dy[d];

			// �E���̈ʒu+�����ɕǂ�����
			if (pos.on_wall(nx, ny))
				continue;

			// �E���̈ʒu+�����Ɋ₪����
			if (pos.on_rock(nx, ny))
				continue;

			// �T���ς݂̏ꏊ�ł����
			if (closed[ny][nx]) continue;

			// (nx, ny)�͒T�����܂��� = true
			closed[ny][nx] = true;

			// �����͐i�ނ��Ƃ��ł��邽��queue�֒ǉ�����
			open.push(Search(pos, d, nx, ny, sc.distance + 1));
		}
	}

	return shortest;
}

void Position::eraseDangerSouls(Position& pos)
{
	// �e�E�҃\�E���ɑ΂���
	for (int id = 0; id < pos.numOfSouls; ++id)
	{
		Bitboard dangerSouls =
			(crossBB((File)pos.get_soul(id).file | (Rank)pos.get_soul(id).rank))
			| (squareBB((File)pos.get_soul(id).file | (Rank)pos.get_soul(id).rank));
		dangerSouls &= pos.objects(DOG);

		// �E�����߂���2�C�ȏア��Ȃ�ڎw���Ȃ�
		if (more_than_one(dangerSouls))
			pos.byTypeBB[SOUL] ^= squareBB((File)pos.get_soul(id).file | (Rank)pos.get_soul(id).rank);
	}
}