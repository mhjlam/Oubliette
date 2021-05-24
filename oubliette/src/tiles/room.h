#pragma once

#include "tile.h"
#include "../entities/player.h"
#include "../entities/monster.h"

// Room has special procedures for the console dialogue that player writes in.
class Room : public Tile
{
private:
	static Tile* roomWithGateKey_;

	uint numMonsters_;
	bool hasKey_;
	bool hasGateKey_;
	bool initialized_;
	uint gold_;

public:
	Room(uint x, uint y);

	void generateContent();

	void enter(player& player);
	void takeKey(player& player);
	void takeGold(player& player);
	void attack(player& player);
	void getAttacked(player& player);

	uint monsterCount() const { return numMonsters_; }
	uint goldAmount() const { return gold_; };
	bool hasKey() const { return hasGateKey_; };
	bool hasGateKey() const { return hasGateKey_; };
	bool empty() const;

	static void setRoomWithGateKey(Tile* room) { roomWithGateKey_ = room; }
};
