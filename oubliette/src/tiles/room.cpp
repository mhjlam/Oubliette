#include "room.h"
#include <iostream>

Tile* Room::roomWithGateKey_ = nullptr;

Room::Room(uint x, uint y) : Tile(x, y, TILE_ROOM), numMonsters_(0), hasKey_(false), hasGateKey_(false), initialized_(false), gold_(0)
{
	solid = false;
	color = color_t(0.8f, 0.8f, 0.8f); // light grey
}

void Room::generateContent()
{
	if (!initialized_)
	{
		// make some monsters
		numMonsters_ = rand() % 3; // 0 to 2 (=3) dwellers

		// add some gold
		gold_ = rand() % 150 + 25;

		// generate keys
		bool generateGateKey = (this == roomWithGateKey_);

		if (rand() % 3 == 1 || generateGateKey)
			hasKey_ = true;

		if (generateGateKey)
			hasGateKey_ = true;

		initialized_ = true;
	}
}

void Room::enter(player& player)
{
	std::cout << player.name << " entered a room" << std::endl;

	generateContent();
	player.in_room = true;

	// used to read the player's choices
	std::string input;

	while (input != "exit")
	{
		// tell the status of the room
		if (empty())
		{
			std::cout << "This room is empty" << std::endl;
		}
		else
		{
			if (monsterCount() > 0)
			{
				std::cout << monsterCount() << " monster(s) can be found in this room" << std::endl;
			}
			else
			{
				std::cout << "In this room, you find:" << std::endl;

				if (goldAmount() != 0)
					std::cout << "- A bag of gold" << std::endl;

				if (hasKey())
				{
					if (hasGateKey())
						std::cout << "- A large gate key!" << std::endl;
					else
						std::cout << "- A rusty old key" << std::endl;
				}
			}
		}

		// ask the player what to do
		std::cout << "What will you do? (attack, take, exit)" << std::endl;
		std::cin >> input;
		std::transform(input.begin(), input.end(), input.begin(), ::tolower);

		// evaluate
		if (input == "attack")
		{
			if (monsterCount() > 0)
			{
				std::cout << player.name << " started to attack one of the monsters" << std::endl;
				attack(player);

				std::cout << player.name << " killed a monster!" << std::endl;

				if (monsterCount() == 0)
					std::cout << "The room has been cleared!" << std::endl;
			}
			else
			{
				std::cout << player.name << " assaulted the furniture in a blind rage" << std::endl;
			}

		}
		else if (input == "take")
		{
			if (monsterCount() == 0) // no taking stuff when there are monsters around
			{
				if (hasKey() || goldAmount() > 0)
				{
					if (goldAmount() > 0)
					{
						std::cout << player.name << " took " << goldAmount() << " gold from the room" << std::endl;
						takeGold(player);
					}

					if (hasKey())
					{
						std::cout << player.name << " took the key from the room" << std::endl;
						
						if (hasGateKey_)
						{
							player.gate_key = true;
							hasGateKey_ = false;  
							hasKey_ = false;
						}
						else takeKey(player);

						if (player.gate_key)
							std::cout << player.name << " now has the Gate Key!" << std::endl;
					}
				}
				else
				{
					std::cout << "Nothing to take!" << std::endl;
				}
			}
			else
			{
				// you tried to take without killing the monsters
				if (rand() % 5 == 1)
					getAttacked(player);	
				else
					std::cout << "You cannot search for valuables with monsters still in the room!" << std::endl;
			}
		}
		else if (input != "exit")
		{
			std::cout << "Come again?" << std::endl;
		}

		if (monsterCount() > 0)
		{
			if (rand() % 5 == 1)
				getAttacked(player);
		}

		std::cout << std::endl;
	}

	player.in_room = false;

	if (player.alive)
		std::cout << player.name << " left the room" << std::endl;
	else
		std::cout << player.name << "'s corpse was thrown out of the room" << std::endl;

	std::cout << std::endl;

}

void Room::takeKey(player& player) // call from procedure that checks Room text input
{
	if (hasKey_)
	{
		if (hasGateKey_)
			player.gate_key = true;
		else
			player.keys++;

		hasKey_ = false;
	}
}

void Room::takeGold(player& player)
{
	player.gold += gold_;
	gold_ = 0;
}

void Room::attack(player& player)
{
	numMonsters_--; // kill a monster at a time
}

void Room::getAttacked(player& player)
{
	std::cout << player.name << " is attacked!" << std::endl;

	if (rand() % 3 == 1)
	{
		std::cout << player.name << " dies by hands of the monster!" << std::endl;
		player.kill();
	}
	else
	{
		std::cout << "But " << player.name << " defends himself and slays the assailant!" << std::endl;
		--numMonsters_;
	}
}

bool Room::empty() const
{
	return (monsterCount() == 0 && goldAmount() == 0 && !hasKey());
}
