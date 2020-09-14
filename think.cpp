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

	// 忍術の利用
	check_skills(myPos, oppPos, type);

	for (int id = 0; id < myPos.num_of_ninjas(); ++id) {
		for (int i = 0; i < (type == HIGH_SPEED ? 3 : 2); ++i) {
			// 行動探索
			walk[id] += search(myPos, oppPos, id, i);
		}
	}

	// 出力
	for (int id = 0; id < myPos.num_of_ninjas(); ++id)
		cout << walk[id] << endl;
}

void Think::check_skills(Position& myPos, Position& oppPos, Skill& type)
{
	// 自陣に忍術を使うなら
	if (skills_to_me(myPos, oppPos))
		return;

	// 相手陣に忍術を使うなら
	else if (skills_to_opp(myPos, oppPos))
		return;

	// これら以外は超高速or通常移動
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

	// 自陣の忍者それぞれの状態を見る
	for (int id = 0; id < myPos.num_of_ninjas(); ++id)
	{
		// 1. 回転斬のコストが10以下なら
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

		// 2. 次のターンで詰むとき
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

		// 3. 岩落とし読み
		/*target = ALL0BB;
		if (rockfall_square(myPos, target, id))
		{
			type = MY_LIGHTNING;
			break;
		}*/


		// 4. 近くに岩を壊せば取れる忍者ソウルがあるとき
		target = ALL0BB;
		if (break_and_catch(myPos, target, id))
		{
			type = MY_LIGHTNING;
			break;
		}

		// 5. 忍力に余裕があるとき
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

	// 雷撃で壊す岩もしくは分身を作る位置
	File targetFile;
	Rank targetRank;

	// 回転斬りなら
	if (type == SWORD_ROTATION)
	{
		for (Square s = SQ1_1; s <= SQ17_14; ++s)
		{
			if (target & squareBB(s)) {

				// 回転斬りは犬を消す
				Position::simulateSkill(myPos, type, s);
			}
		}

		cout << 3 << endl;
		cout << type << " " << useSwordId << endl;
	}

	// それ以外
	else {
		for (Square s = SQ1_1; s <= SQ17_14; ++s)
		{
			if (target & squareBB(s)) {
				targetFile = file_of(s);
				targetRank = rank_of(s);
				break;
			}
		}

		// 雷撃の場合は岩を消す、分身の場合は犬の移動
		Position::simulateSkill(myPos, type, (targetFile | targetRank));

		cout << 3 << endl;
		cout << type << " " << targetRank << " " << targetFile << endl;
	}

	return true;
}

bool Think::skills_to_opp(Position& myPos, Position& oppPos)
{
	bool canUseRockfall = (myPos.get_skill_point() >= Limits.skillCost[MY_ROCKFALL]);

	// 忍力が足りない
	if (!canUseRockfall)
		return false;

	// 相手陣の忍者の位置が危険かを確認する
	for (int id = 0; id < oppPos.num_of_ninjas(); ++id)
	{
		Bitboard target = ALL0BB;

		// 危険な状態であれば岩を落とす
		if (rockfall_square(oppPos, target, id))
		{
			// ほっといても詰み
			if (!target) break;

			File targetFile;
			Rank targetRank;
			for (Square s = SQ1_1; s <= SQ17_14; ++s)
			{
				if (target & squareBB(s)) { // 落とす場所なら
					targetFile = file_of(s);
					targetRank = rank_of(s);
					break;
				}
			}

			// 落石の場合は岩を増やす
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

	// queueの中身がなくなるまで
	while (!open.empty())
	{
		// 先頭の要素を取り出す
		Search sc = open.front(); open.pop();

		// その場に犬がいるなら
		if (sc.pos.on_dog(sc.x, sc.y))
			continue;

		// 犬のこないとこは一応第二候補にしておく
		if (!(squareBB(sc.x | sc.y) & sc.pos.objects(DOG)))
			secondDir = sc.dir;

		// (sc.x, sc.y)にソウルがある && toSoulが初期値であれば
		if (sc.pos.on_soul(sc.x, sc.y) && shortest == INFINITE) {
			shortest = sc.distance;
			bestDir = sc.dir;
		}

		// 初期値でなければ最小距離が代入されている
		if (shortest != INFINITE)
			break;

		// 今の盤面を残しておく
		Position temp = sc.pos;

		// (sc.x, sc.y)には何もない→さらに移動させる
		for (Direction d = UP; d <= DOWN; ++d) {
			File nx = sc.x + (File)dx[d];
			Rank ny = sc.y + (Rank)dy[d];

			// ここで元の盤面にする
			sc.pos = temp;

			// 探索済みの場所であれば
			if (closed[ny][nx])
				continue;

			// 忍者の位置+方向に犬がいる
			if (sc.pos.on_dog(nx, ny))
				continue;

			// 忍者の位置+方向に壁がある
			if (sc.pos.on_wall(nx, ny))
				continue;

			// (nx, ny)に行くと次のターン詰み
			if (checkmate_square(sc.pos, nx, ny, id))
				continue;

			// 忍者の位置+方向に岩がある
			if (sc.pos.on_rock(nx, ny)) {
				if (cannot_move_rock(sc.pos, d, nx, ny)) // それが動かせない
					continue;
				else {
					Position::simulateRock(sc.pos, d, nx, ny);
				}
			}

			// (nx, ny)は探索しました = true
			closed[ny][nx] = true;

			// 忍者を動かす
			Position::simulateNinja(sc.pos, nx, ny, id);

			// 犬がいれば動かす
			if (myPos.num_of_dogs() > 0
				&& (actNum == 0 ? sc.distance % 2 == 1 : sc.distance % 2 == 0))
			{
				Bitboard dogAppears = dog_appears(oppPos);
				Position::simulateDog(sc.pos, dogAppears, id);
			}

			// ここは進むことができるためqueueへ追加する
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

	// queueの中身がなくなるまで
	while (!open.empty())
	{
		// 先頭の要素を取り出す
		Search sc = open.front(); open.pop();

		if (sc.distance > longest) {
			longest = sc.distance;
			longestSquare = sc.x | sc.y;
		}

		// (sc.x, sc.y)には何もない→さらに移動させる
		for (Direction d = UP; d <= DOWN; ++d) {
			File nx = sc.x + (File)dx[d];
			Rank ny = sc.y + (Rank)dy[d];

			// 忍者の位置+方向に壁がある
			if (pos.on_wall(nx, ny))
				continue;

			// 忍者の位置+方向に岩がある
			if (pos.on_rock(nx, ny))
				continue;

			// 探索済みの場所であれば
			if (closed[ny][nx]) continue;

			// (nx, ny)は探索しました = true
			closed[ny][nx] = true;

			// ここは進むことができるためqueueへ追加する
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

		// (nx, ny)に犬がいる、または捕まるとき
		if (pos.on_dog(nx, ny) || pos.catch_by_dog(nx, ny)) {
			++mateCount;
			continue;
		}

		// (nx, ny)に壁がある
		if (pos.on_wall(nx, ny)) {
			++mateCount;
			continue;
		}

		// (nx, ny)に岩がある
		if (pos.on_rock(nx, ny) && d != STAY)
		{
			// それが動かせない
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

		// (nx, ny)に犬がいる、または捕まるとき
		if (pos.on_dog(nx, ny)/* || pos.catch_by_dog(nx, ny)*/) {
			++mateCount;
			continue;
		}

		// (nx, ny)に壁がある
		if (pos.on_wall(nx, ny)) {
			++mateCount;
			continue;
		}

		// (nx, ny)に岩がある
		if (pos.on_rock(nx, ny))
		{
			// それが動かせない
			if (cannot_move_rock(pos, d, nx, ny))
			{
				// 動かせないが、壊せば犬に捕まらず行けるとき
				if (!pos.catch_by_dog(nx, ny))
					target |= squareBB(nx | ny); // そこが雷撃のターゲット

				++mateCount;
				continue;
			}
		}

		// (nx, ny)に行くと次のターン詰み
		if (checkmate_square(pos, nx, ny, id)) {
			++mateCount;
			continue;
		}
	}

	return mateCount == 4 ? true : false;
}

void Think::avatar_square(Position& pos, Bitboard& target, int id)
{
	// 2. 忍術を使う忍者から最も遠い場所
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

		// (nx, ny)に犬がいる、または捕まる
		if (pos.on_dog(nx, ny) || pos.catch_by_dog(nx, ny)) {
			++dangerCount;
			continue;
		}

		// (nx, ny)に壁がある
		if (pos.on_wall(nx, ny)) {
			++dangerCount;
			continue;
		}

		// (nx, ny)に岩がある
		if (pos.on_rock(nx, ny))
		{
			// それが動かせない
			if (cannot_move_rock(pos, d, nx, ny)) {
				++dangerCount;
				continue;
			}

			// それが動かせる、動かせない理由が壁でない
			if (!cannot_move_rock(pos, d, nx, ny))
			{
				target |= squareBB((nx + (File)dx[d]) | (ny + (Rank)dy[d]));

				++dangerCount;
				continue;
			}
		}

		// (nx, ny)が床で奥が壁のとき→忍床壁
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

		// 壁がある
		if (myPos.on_wall(nx, ny))
			continue;

		// 犬がいる
		if (myPos.on_dog(nx, ny))
			continue;

		// 岩がある
		if (myPos.on_rock(nx, ny))
		{
			// それが動かせなくてソウルがあるなら
			if (cannot_move_rock(myPos, d, nx, ny) && myPos.on_soul(nx, ny)) {
				if (d == UP || d == DOWN
					? (myPos.on_rock(nx + (File)dx[RIGHT], ny + (Rank)dy[RIGHT])
						|| myPos.on_rock(nx + (File)dx[LEFT], ny + (Rank)dy[LEFT]))
					: (myPos.on_rock(nx + (File)dx[UP], ny + (Rank)dy[UP])
						|| myPos.on_rock(nx + (File)dx[DOWN], ny + (Rank)dy[DOWN])))
					target |= squareBB(nx | ny);
				return true;
			}

			// それが動かせてソウルがない
			if (!cannot_move_rock(myPos, d, nx, ny) && !myPos.on_soul(nx, ny))
			{
				// ソウルの場所
				File sx = nx + (File)dx[d];
				Rank sy = ny + (Rank)dy[d];

				// 奥の岩の場所
				File tx = sx + (File)dx[d];
				Rank ty = sy + (Rank)dy[d];

				// 岩魂岩忍←のとき、岩を動かして魂が取れなくなるなら
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

		// 隣にはないが近くにある岩にソウルがあるとき
		else {
			for (Direction d2 = UP; d2 <= DOWN; ++d2)
			{
				File sx = nx + (File)dx[d2];
				Rank sy = ny + (Rank)dy[d2];

				// 岩がある
				if (myPos.on_rock(sx, sy))
				{
					// それが動かせなくてソウルがあるなら
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