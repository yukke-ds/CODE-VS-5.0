#include <fstream>
#include <queue>
#include <vector>

#include "bitboard.h"
#include "think.h"

namespace Think {
	LimitsType Limits;
}

using namespace std;
using namespace Think;

void Think::start(Position& myPos, Position& oppPos)
{
	string walk[MAX_NINJAS];
	Skill type = NONE;

	// �E�p�̗��p
	check_skills(myPos, oppPos, type);

	for (int id = 0; id < myPos.num_of_ninjas(); ++id) {
		for (int i = 0; i < (type == HIGH_SPEED ? 3 : 2); ++i) {
			// �s���T��
			walk[id] += search(myPos, oppPos, id, i);
		}
	}

	// �o��
	for (int id = 0; id < myPos.num_of_ninjas(); ++id)
		cout << walk[id] << endl;
}

void Think::check_skills(Position& myPos, Position& oppPos, Skill& type)
{
	// ���w�ɔE�p���g���Ȃ�
	if (skills_to_me(myPos, oppPos))
		return;

	// ����w�ɔE�p���g���Ȃ�
	else if (skills_to_opp(myPos, oppPos))
		return;

	// �����ȊO�͒�����or�ʏ�ړ�
	else {
		if (myPos.get_skill_point() >= Limits.skillCost[HIGH_SPEED]
			&& Limits.skillCost[HIGH_SPEED] == 1
			&& myPos.num_of_dogs() < 10)
		{
			type = HIGH_SPEED;
			cout << 3 << endl;
			cout << HIGH_SPEED << endl;
		}
		else
			cout << 2 << endl;
	}
}

bool Think::skills_to_me(Position& myPos, Position& oppPos)
{
	Bitboard target;
	Skill type;
	int useSwordId = -1;
	bool canUseLightning = (myPos.get_skill_point() >= Limits.skillCost[MY_LIGHTNING]);
	bool canUseAvatar = (myPos.get_skill_point() >= Limits.skillCost[MY_AVATAR]);
	bool canUseSword = (myPos.get_skill_point() >= Limits.skillCost[SWORD_ROTATION]);

	// ���w�̔E�҂��ꂼ��̏�Ԃ�����
	for (int id = 0; id < myPos.num_of_ninjas(); ++id)
	{
		// 1. ��]�a�̃R�X�g��10�ȉ��Ȃ�
		target = ALL0BB;
		if (Limits.skillCost[SWORD_ROTATION] <= 10)
		{
			target = myPos.objects(DOG)
				& aroundBB(myPos.get_ninja(id).file | myPos.get_ninja(id).rank);

			if (more_than_two(target))
			{
				type = SWORD_ROTATION;
				useSwordId = id;
				break;
			}
		}

		// 2. ���̃^�[���ŋl�ނƂ�
		target = ALL0BB;
		if (lightning_square(myPos, target, id))
		{
			if (!target)
			{
				type = MY_AVATAR;
				avatar_square(myPos, target, id);
			}
			else
				type = MY_LIGHTNING;

			break;
		}

		// 3. �◎�Ƃ��ǂ�
		/*target = ALL0BB;
		if (rockfall_square(myPos, target, id))
		{
			type = MY_LIGHTNING;
			break;
		}*/


		// 4. �߂��Ɋ���󂹂Ύ���E�҃\�E��������Ƃ�
		target = ALL0BB;
		if (break_and_catch(myPos, target, id))
		{
			type = MY_LIGHTNING;
			break;
		}

		// 5. �E�͂ɗ]�T������Ƃ�
		target = ALL0BB;
		if (myPos.get_skill_point() >= 26)
		{
			type = MY_AVATAR;
			avatar_square(myPos, target, id);
			break;
		}
	}

	if (!target
		|| (type == MY_LIGHTNING && !canUseLightning)
		|| (type == MY_AVATAR && !canUseAvatar)
		|| (type == SWORD_ROTATION && !canUseSword))
		return false;

	// �����ŉ󂷊�������͕��g�����ʒu
	File targetFile;
	Rank targetRank;

	// ��]�a��Ȃ�
	if (type == SWORD_ROTATION)
	{
		for (Square s = SQ1_1; s <= SQ17_14; ++s)
		{
			if (target & squareBB(s)) {

				// ��]�a��͌�������
				Position::simulateSkill(myPos, type, s);
			}
		}

		cout << 3 << endl;
		cout << type << " " << useSwordId << endl;
	}

	// ����ȊO
	else {
		for (Square s = SQ1_1; s <= SQ17_14; ++s)
		{
			if (target & squareBB(s)) {
				targetFile = file_of(s);
				targetRank = rank_of(s);
				break;
			}
		}

		// �����̏ꍇ�͊�������A���g�̏ꍇ�͌��̈ړ�
		Position::simulateSkill(myPos, type, (targetFile | targetRank));

		cout << 3 << endl;
		cout << type << " " << targetRank << " " << targetFile << endl;
	}

	return true;
}

bool Think::skills_to_opp(Position& myPos, Position& oppPos)
{
	bool canUseRockfall = (myPos.get_skill_point() >= Limits.skillCost[MY_ROCKFALL]);

	// �E�͂�����Ȃ�
	if (!canUseRockfall)
		return false;

	// ����w�̔E�҂̈ʒu���댯�����m�F����
	for (int id = 0; id < oppPos.num_of_ninjas(); ++id)
	{
		Bitboard target = ALL0BB;

		// �댯�ȏ�Ԃł���Ί�𗎂Ƃ�
		if (rockfall_square(oppPos, target, id))
		{
			// �ق��Ƃ��Ă��l��
			if (!target) break;

			File targetFile;
			Rank targetRank;
			for (Square s = SQ1_1; s <= SQ17_14; ++s)
			{
				if (target & squareBB(s)) { // ���Ƃ��ꏊ�Ȃ�
					targetFile = file_of(s);
					targetRank = rank_of(s);
					break;
				}
			}

			// ���΂̏ꍇ�͊�𑝂₷
			Position::simulateSkill(oppPos, OPP_ROCKFALL, (targetFile | targetRank));

			cout << 3 << endl;
			cout << OPP_ROCKFALL << " " << targetRank << " " << targetFile << endl;
			return true;
		}
	}

	return false;
}

string Think::search(Position& myPos, Position& oppPos, int id, int actNum)
{
	int shortest = INFINITE;
	Direction bestDir = STAY;
	Direction secondDir = STAY;
	queue<Search> open;
	vector<vector<bool>> closed(myPos.get_height(), vector<bool>(myPos.get_width(), false));

	File ninjaFile = myPos.get_ninja(id).file;
	Rank ninjaRank = myPos.get_ninja(id).rank;
	closed[ninjaRank][ninjaFile] = true;
	open.push(Search(myPos, STAY, ninjaFile, ninjaRank, 0));

	// queue�̒��g���Ȃ��Ȃ�܂�
	while (!open.empty())
	{
		// �擪�̗v�f�����o��
		Search sc = open.front(); open.pop();

		// ���̏�Ɍ�������Ȃ�
		if (sc.pos.on_dog(sc.x, sc.y))
			continue;

		// ���̂��Ȃ��Ƃ��͈ꉞ�����ɂ��Ă���
		if (!(squareBB(sc.x | sc.y) & sc.pos.objects(DOG)))
			secondDir = sc.dir;

		// (sc.x, sc.y)�Ƀ\�E�������� && toSoul�������l�ł����
		if (sc.pos.on_soul(sc.x, sc.y) && shortest == INFINITE) {
			shortest = sc.distance;
			bestDir = sc.dir;
		}

		// �����l�łȂ���΍ŏ��������������Ă���
		if (shortest != INFINITE)
			break;

		// ���̔Ֆʂ��c���Ă���
		Position temp = sc.pos;

		// (sc.x, sc.y)�ɂ͉����Ȃ�������Ɉړ�������
		for (Direction d = UP; d <= DOWN; ++d) {
			File nx = sc.x + (File)dx[d];
			Rank ny = sc.y + (Rank)dy[d];

			// �����Ō��̔Ֆʂɂ���
			sc.pos = temp;

			// �T���ς݂̏ꏊ�ł����
			if (closed[ny][nx])
				continue;

			// �E�҂̈ʒu+�����Ɍ�������
			if (sc.pos.on_dog(nx, ny))
				continue;

			// �E�҂̈ʒu+�����ɕǂ�����
			if (sc.pos.on_wall(nx, ny))
				continue;

			// (nx, ny)�ɍs���Ǝ��̃^�[���l��
			if (checkmate_square(sc.pos, nx, ny, id))
				continue;

			// �E�҂̈ʒu+�����Ɋ₪����
			if (sc.pos.on_rock(nx, ny)) {
				if (cannot_move_rock(sc.pos, d, nx, ny)) // ���ꂪ�������Ȃ�
					continue;
				else {
					Position::simulateRock(sc.pos, d, nx, ny);
				}
			}

			// (nx, ny)�͒T�����܂��� = true
			closed[ny][nx] = true;

			// �E�҂𓮂���
			Position::simulateNinja(sc.pos, nx, ny, id);

			// ��������Γ�����
			if (myPos.num_of_dogs() > 0
				&& (actNum == 0 ? sc.distance % 2 == 1 : sc.distance % 2 == 0))
			{
				Bitboard dogAppears = dog_appears(oppPos);
				Position::simulateDog(sc.pos, dogAppears, id);
			}

			// �����͐i�ނ��Ƃ��ł��邽��queue�֒ǉ�����
			if (sc.x == myPos.get_ninja(id).file
				&& sc.y == myPos.get_ninja(id).rank)
				open.push(Search(sc.pos, d, nx, ny, sc.distance + 1));

			else
				open.push(Search(sc.pos, sc.dir, nx, ny, sc.distance + 1));
		}
	}

	if (bestDir == STAY)
		bestDir = secondDir;

	if (bestDir != STAY)
		Position::simulateWalk(myPos, bestDir, id);

	return action[bestDir];
}

Bitboard Think::dog_appears(Position& oppPos)
{
	Bitboard canCatchSouls = ALL0BB;

	for (int id = 0; id < oppPos.num_of_ninjas(); ++id)
	{
		Square ninjaSquare = oppPos.get_ninja(id).file | oppPos.get_ninja(id).rank;

		canCatchSouls |= (largeBB(ninjaSquare) & oppPos.objects(SOUL));
	}

	return canCatchSouls;
}

Square Think::longest_square(Position& pos, File x, Rank y)
{
	int longest = -INFINITE;
	Square longestSquare;
	queue<Search> open;
	vector<vector<bool>> closed(pos.get_height(), vector<bool>(pos.get_width(), false));

	closed[y][x] = true;
	open.push(Search(pos, STAY, x, y, 0));

	// queue�̒��g���Ȃ��Ȃ�܂�
	while (!open.empty())
	{
		// �擪�̗v�f�����o��
		Search sc = open.front(); open.pop();

		if (sc.distance > longest) {
			longest = sc.distance;
			longestSquare = sc.x | sc.y;
		}

		// (sc.x, sc.y)�ɂ͉����Ȃ�������Ɉړ�������
		for (Direction d = UP; d <= DOWN; ++d) {
			File nx = sc.x + (File)dx[d];
			Rank ny = sc.y + (Rank)dy[d];

			// �E�҂̈ʒu+�����ɕǂ�����
			if (pos.on_wall(nx, ny))
				continue;

			// �E�҂̈ʒu+�����Ɋ₪����
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

	return longestSquare;
}

bool Think::checkmate_square(Position& pos, File x, Rank y, int id)
{
	int mateCount = 0;

	for (Direction d = UP; d <= STAY; ++d)
	{
		File nx = x + (File)dx[d];
		Rank ny = y + (Rank)dy[d];

		// (nx, ny)�Ɍ�������A�܂��͕߂܂�Ƃ�
		if (pos.on_dog(nx, ny) || pos.catch_by_dog(nx, ny)) {
			++mateCount;
			continue;
		}

		// (nx, ny)�ɕǂ�����
		if (pos.on_wall(nx, ny)) {
			++mateCount;
			continue;
		}

		// (nx, ny)�Ɋ₪����
		if (pos.on_rock(nx, ny) && d != STAY)
		{
			// ���ꂪ�������Ȃ�
			if (cannot_move_rock(pos, d, nx, ny))
			{
				++mateCount;
				continue;
			}
		}
	}

	return mateCount == 5 ? true : false;
}

bool Think::lightning_square(Position& pos, Bitboard& target, int id)
{
	int mateCount = 0;

	for (Direction d = UP; d <= DOWN; ++d)
	{
		File nx = pos.get_ninja(id).file + (File)dx[d];
		Rank ny = pos.get_ninja(id).rank + (Rank)dy[d];

		// (nx, ny)�Ɍ�������A�܂��͕߂܂�Ƃ�
		if (pos.on_dog(nx, ny)/* || pos.catch_by_dog(nx, ny)*/) {
			++mateCount;
			continue;
		}

		// (nx, ny)�ɕǂ�����
		if (pos.on_wall(nx, ny)) {
			++mateCount;
			continue;
		}

		// (nx, ny)�Ɋ₪����
		if (pos.on_rock(nx, ny))
		{
			// ���ꂪ�������Ȃ�
			if (cannot_move_rock(pos, d, nx, ny))
			{
				// �������Ȃ����A�󂹂Ό��ɕ߂܂炸�s����Ƃ�
				if (!pos.catch_by_dog(nx, ny))
					target |= squareBB(nx | ny); // �����������̃^�[�Q�b�g

				++mateCount;
				continue;
			}
		}

		// (nx, ny)�ɍs���Ǝ��̃^�[���l��
		if (checkmate_square(pos, nx, ny, id)) {
			++mateCount;
			continue;
		}
	}

	return mateCount == 4 ? true : false;
}

void Think::avatar_square(Position& pos, Bitboard& target, int id)
{
	// 2. �E�p���g���E�҂���ł������ꏊ
	File file = pos.get_ninja(id).file;
	Rank rank = pos.get_ninja(id).rank;
	target |= squareBB(longest_square(pos, file, rank));
}

bool Think::rockfall_square(Position& pos, Bitboard& target, int id)
{
	int dangerCount = 0;

	for (Direction d = UP; d <= DOWN; ++d)
	{
		File nx = pos.get_ninja(id).file + (File)dx[d];
		Rank ny = pos.get_ninja(id).rank + (Rank)dy[d];

		// (nx, ny)�Ɍ�������A�܂��͕߂܂�
		if (pos.on_dog(nx, ny) || pos.catch_by_dog(nx, ny)) {
			++dangerCount;
			continue;
		}

		// (nx, ny)�ɕǂ�����
		if (pos.on_wall(nx, ny)) {
			++dangerCount;
			continue;
		}

		// (nx, ny)�Ɋ₪����
		if (pos.on_rock(nx, ny))
		{
			// ���ꂪ�������Ȃ�
			if (cannot_move_rock(pos, d, nx, ny)) {
				++dangerCount;
				continue;
			}

			// ���ꂪ��������A�������Ȃ����R���ǂłȂ�
			if (!cannot_move_rock(pos, d, nx, ny))
			{
				target |= squareBB((nx + (File)dx[d]) | (ny + (Rank)dy[d]));

				++dangerCount;
				continue;
			}
		}

		// (nx, ny)�����ŉ����ǂ̂Ƃ����E����
		if (pos.empty(nx, ny)) {
			if (pos.on_wall(nx + (File)dx[d], ny + (Rank)dy[d]) && dangerCount >= 3)
			{
				target |= squareBB((File)nx | (Rank)ny);
				++dangerCount;
				continue;
			}
		}
	}

	return dangerCount == 4 ? true : false;
}

bool Think::break_and_catch(Position& myPos, Bitboard& target, int id)
{
	for (Direction d = UP; d <= DOWN; ++d)
	{
		File nx = myPos.get_ninja(id).file + (File)dx[d];
		Rank ny = myPos.get_ninja(id).rank + (Rank)dy[d];

		// �ǂ�����
		if (myPos.on_wall(nx, ny))
			continue;

		// ��������
		if (myPos.on_dog(nx, ny))
			continue;

		// �₪����
		if (myPos.on_rock(nx, ny))
		{
			// ���ꂪ�������Ȃ��ă\�E��������Ȃ�
			if (cannot_move_rock(myPos, d, nx, ny) && myPos.on_soul(nx, ny)) {
				if (d == UP || d == DOWN
					? (myPos.on_rock(nx + (File)dx[RIGHT], ny + (Rank)dy[RIGHT])
						|| myPos.on_rock(nx + (File)dx[LEFT], ny + (Rank)dy[LEFT]))
					: (myPos.on_rock(nx + (File)dx[UP], ny + (Rank)dy[UP])
						|| myPos.on_rock(nx + (File)dx[DOWN], ny + (Rank)dy[DOWN])))
					target |= squareBB(nx | ny);
				return true;
			}

			// ���ꂪ�������ă\�E�����Ȃ�
			if (!cannot_move_rock(myPos, d, nx, ny) && !myPos.on_soul(nx, ny))
			{
				// �\�E���̏ꏊ
				File sx = nx + (File)dx[d];
				Rank sy = ny + (Rank)dy[d];

				// ���̊�̏ꏊ
				File tx = sx + (File)dx[d];
				Rank ty = sy + (Rank)dy[d];

				// �⍰��E���̂Ƃ��A��𓮂����č������Ȃ��Ȃ�Ȃ�
				if (myPos.on_soul(sx, sy) && myPos.on_rock(tx, ty))
					if (d == UP || d == DOWN
						? (myPos.on_rock(sx + (File)dx[RIGHT], sy + (Rank)dy[RIGHT])
							|| myPos.on_rock(sx + (File)dx[LEFT], sy + (Rank)dy[LEFT]))
						: (myPos.on_rock(sx + (File)dx[UP], sy + (Rank)dy[UP])
							|| myPos.on_rock(sx + (File)dx[DOWN], sy + (Rank)dy[DOWN]))) {
						target |= squareBB(nx | ny);
						return true;
					}
			}
		}

		// �ׂɂ͂Ȃ����߂��ɂ����Ƀ\�E��������Ƃ�
		else {
			for (Direction d2 = UP; d2 <= DOWN; ++d2)
			{
				File sx = nx + (File)dx[d2];
				Rank sy = ny + (Rank)dy[d2];

				// �₪����
				if (myPos.on_rock(sx, sy))
				{
					// ���ꂪ�������Ȃ��ă\�E��������Ȃ�
					if (cannot_move_rock(myPos, d2, sx, sy) && myPos.on_soul(sx, sy)) {
						if (d2 == UP || d2 == DOWN
							? (myPos.on_rock(sx + (File)dx[RIGHT], sy + (Rank)dy[RIGHT])
								|| myPos.on_rock(sx + (File)dx[LEFT], sy + (Rank)dy[LEFT]))
							: (myPos.on_rock(sx + (File)dx[UP], sy + (Rank)dy[UP])
								|| myPos.on_rock(sx + (File)dx[DOWN], sy + (Rank)dy[DOWN])))
							target |= squareBB(sx | sy);
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Think::cannot_move_rock(Position& pos, Direction dir, File nx, Rank ny)
{
	switch (dir) {
	case UP:
		return pos.on_ninja(nx, ny - RANK_ONE)
			|| pos.on_dog(nx, ny - RANK_ONE)
			|| pos.on_rock(nx, ny - RANK_ONE)
			|| pos.on_wall(nx, ny - RANK_ONE);

	case RIGHT:
		return pos.on_ninja(nx + FILE_ONE, ny)
			|| pos.on_dog(nx + FILE_ONE, ny)
			|| pos.on_rock(nx + FILE_ONE, ny)
			|| pos.on_wall(nx + FILE_ONE, ny);

	case DOWN:
		return pos.on_ninja(nx, ny + RANK_ONE)
			|| pos.on_dog(nx, ny + RANK_ONE)
			|| pos.on_rock(nx, ny + RANK_ONE)
			|| pos.on_wall(nx, ny + RANK_ONE);

	case LEFT:
		return pos.on_ninja(nx - FILE_ONE, ny)
			|| pos.on_dog(nx - FILE_ONE, ny)
			|| pos.on_rock(nx - FILE_ONE, ny)
			|| pos.on_wall(nx - FILE_ONE, ny);

	default: assert(false); return false;
	}
}