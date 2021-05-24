/*
 * 	Main class for the program. Integrates into GLUT and provides basic functions such as handling player input,
 *  drawing basic shapes that represent Tiles & Entities.
 */

#pragma once

#include <ctime>
#include <iostream>

#include "map.h"
#include "entities/player.h"
#include "entities/monster.h"

#include <GL/freeglut.h>

class Game 
{
private:
	static uint current_map;
	static std::vector<std::string> map_files;

	static player* player1;
	static player* player2;

	static std::vector<monster*> monsters;

private:
	static bool load_map(const std::string& file);

	static void set_color(color_t color, bool dim = false);
	static void keyboard(unsigned char key, int x, int y);
	static void special(int key, int x, int y);

	static void draw_triangle(uint x, uint y, dir_t orientation);
	static void draw_square(uint x, uint y, float scale = 1.0f);

	static void draw_entity(entity* e);
	static void draw_tile(Tile* t);
	static void draw_bullet(bullet_t* b);

	static void update_bullet(player* p);

	static void update_bullets(int value);
	static void update_monsters(int value);

	static void process_movement(player* pl);
	static void transfer_inventory(player* giver, player* taker);

	static void update();
	static void draw();
	static void end_game();
	static void reset_map();

public:
	static void init(int argc, char* argv[]);
	static void play();
	static void exit();
};
