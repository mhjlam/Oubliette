#include "game.h"
#include "tiles/tile.h"
#include "tiles/room.h"

Map* map_ = Map::Instance();

uint Game::current_map = 0;
std::vector<std::string> Game::map_files;

player* Game::player1 = nullptr;
player* Game::player2 = nullptr;

std::vector<monster*> Game::monsters;

void Game::exit()
{
	std::cout << "Closing..." << std::endl;

	reset_map();

	std::exit(0);
}

void Game::set_color(color_t color, bool dim)
{ 
	if (dim)
		glColor3f(color.r / 4, color.g / 4, color.b / 4);
	else
		glColor3f(color.r, color.g, color.b);
}

void Game::process_movement(player* pl)
{
	if (!pl) return;
	if (!pl->alive) return;

	bool validated = true;

	uint nx = pl->x;
	uint ny = pl->y;

	switch (pl->direction)
	{
		case DIR_UP:		ny -= 1; break;
		case DIR_DOWN:	ny += 1; break;
		case DIR_LEFT:	nx -= 1; break;
		case DIR_RIGHT:	nx += 1; break;
		default: return;
	}

	if (!map_->isWalkable(nx, ny))
	{
		validated = false; // but there may be something more going on

		// check for collision with monsters
		for (uint i = 0; i < monsters.size(); i++)
		{
			monster* m = monsters.at(i);

			if (m->x == nx && m->y == ny && m->alive)
			{
				pl->kill();
				map_->getTileAt(pl->x, pl->y)->solid = false;

				std::cout << pl->name << " was killed by a monster!" << std::endl;

				return;
			}
		}
	}

	// check for collision with (dead) player (otherwise it wouldn't be walkable)
	if (pl == player1)
	{
		if (player2) // if player two exists
			if (player2->x == nx && player2->y == ny) // same location
				if (!player2->alive) // but he's dead
					transfer_inventory(player2, player1);      
				
	}
	else if (player1->x == nx && player1->y == ny) // same location
		if (!player1->alive) // but dead
		   transfer_inventory(player1, player2);
	
	// approve movement or not
	if (validated)
	{
		map_->getTileAt(pl->x, pl->y)->solid = false;
		map_->getTileAt(nx, ny)->solid = true;
		pl->x = nx;
		pl->y = ny;
	}

	// from here on, handle special tile cases:

	if (map_->isType(TILE_GOLD, nx, ny))
	{
		// add key or gold to player
		// replace tile with Floor
		uint value = ((Gold*)(map_->getTileAt(nx, ny)))->value_;
		pl->gold += value;
		map_->putFloorAt(nx, ny);

		std::cout << pl->name << " took " << value << " gold" << std::endl;
		std::cout << pl->name << " now has " << pl->gold << " gold" << std::endl;
	}
	else if (map_->isType(TILE_KEY, nx, ny))
	{
		pl->keys++;
		map_->putFloorAt(nx, ny);

		std::cout << pl->name << " found a key" << std::endl;
		std::cout << pl->name << " now has " << pl->keys << " key(s)" << std::endl;
	}
	else if (map_->isType(TILE_DOOR, nx, ny))
	{
		// if player has normal key, remove it from inventory
		// and 'open' door (replace tile with Floor)
		if (pl->keys > 0)
		{
			pl->keys--;
			map_->putFloorAt(nx, ny);

			std::cout << pl->name << " unlocked a door" << std::endl;
		}
	}
	else if (map_->isType(TILE_GATE, nx, ny))
	{
		if (pl->gate_key)
		{
			std::cout << pl->name << " opened the Gate and has exited the oubliette!" << std::endl;
			std::cout << pl->name << " escaped with " << pl->gold << " gold!" << std::endl;
			std::cout << "Congratulations!" << std::endl;

			end_game();
		}
		else
		{
			std::cout << pl->name << " does not possess the gate key! " << std::endl;
		}
	}
	else if (map_->isType(TILE_ROOM, nx, ny))
	{
		Room* room = (Room*)map_->getTileAt(nx, ny);
		room->enter(*pl);
	}
}

void Game::transfer_inventory(player* giver, player* taker)
{
	std::cout << taker->name << " took the valuables off of " << giver->name << "'s corpse" << std::endl;

	// take belongings off of the dead guy (this could be considered trading, right?)
	if (giver->gold > 0)
	{
		std::cout << taker->name << " took " << giver->gold << " gold from " << giver->name << std::endl;

		taker->gold += giver->gold;
		giver->gold = 0;
	}

	if (giver->keys > 0)
	{
		std::cout << taker->name << " took " << giver->keys << " keys from " << giver->name << std::endl;

		taker->keys += giver->keys;
		giver->keys = 0;
	}

	if (giver->gate_key)
	{
		std::cout << taker->name << " took the gate key from " << giver->name << std::endl;

		taker->gate_key = true;
		giver->gate_key = false;
	}
}

void Game::keyboard(unsigned char key, int x, int y)
{
	if (key == 27) exit(); // escape key

	if (!player1) return;
	if (!player1->alive) return;

	switch (key)
	{
		// player1
		case 'w':
			player1->direction = DIR_UP;
			process_movement(player1);
		break;

		case 's':
			player1->direction = DIR_DOWN;
			process_movement(player1);
		break;

		case 'a':
			player1->direction = DIR_LEFT;
			process_movement(player1);
		break;

		case 'd':
			player1->direction = DIR_RIGHT;
			process_movement(player1);
		break;

		case 'f':
			player1->shoot();
		break;
	}
}

void Game::special(int key, int x, int y)
{
	if (!player2) return;
	if (!player2->alive) return;

	switch (key)
	{
		// player2 keys
		case GLUT_KEY_UP:
			player2->direction = DIR_UP;
			process_movement(player2);
		break;

		case GLUT_KEY_DOWN:
			player2->direction = DIR_DOWN;
			process_movement(player2);
		break;

		case GLUT_KEY_LEFT:
			player2->direction = DIR_LEFT;
			process_movement(player2);
		break;

		case GLUT_KEY_RIGHT:
			player2->direction = DIR_RIGHT;
			process_movement(player2);
		break;

		case GLUT_KEY_END:
			player2->shoot();
		break;
	}
}

void Game::draw_triangle(uint x, uint y, dir_t direction)
{
	glBegin(GL_TRIANGLES);

	switch (direction)
	{
		case DIR_UP:
			glVertex2f(x, y + 0.5f);
			glVertex2f(x + 1.0f, y + 0.5f);
			glVertex2f(x + 0.5f, y);
		break;

		case DIR_DOWN:
			glVertex2f(x, y);
			glVertex2f(x + 1.0f, y);
			glVertex2f(x + 0.5f, y + 0.5f);
		break;

		case DIR_LEFT:
			glVertex2f(x + 1.0f, y);
			glVertex2f(x + 1.0f, y + 1.0f);
			glVertex2f(x + 0.5f, y + 0.5f);
		break;

		case DIR_RIGHT:
			glVertex2f(x, y);
			glVertex2f(x + 0.5f, y + 0.5f);
			glVertex2f(x, y + 1.0f);
		break;

		default:
		break;
	}

	glEnd();
}

void Game::draw_square(uint x, uint y, float scale)
{
	float centerX = x + 0.5f;
	float centerY = y + 0.5f;
	float half = scale / 2;

	glBegin(GL_QUADS);
		glVertex2f(centerX - half, centerY - half);
		glVertex2f(centerX + half, centerY - half);
		glVertex2f(centerX + half, centerY + half);
		glVertex2f(centerX - half, centerY + half);
	glEnd();
}

void Game::draw_entity(entity* e)
{
	if (!e) return;
	set_color(e->color, !e->alive);
	draw_triangle(e->x, e->y, e->direction);
}

void Game::draw_tile(Tile* t)
{
	if (!t) return;
	set_color(t->color);
	draw_square(t->x, t->y);
}

void Game::draw_bullet(bullet_t* b)
{
	if (!b)	return;

	set_color(b->color);
	draw_square(b->x, b->y, 0.3f); // smaller
}

void Game::update_bullet(player* p)
{
	if (!p)	return;

	bullet_t* b = p->bullet;

	if (!b) return;

	switch (b->direction)
	{
		case DIR_UP: 	b->y--;		break;
		case DIR_DOWN:	b->y++;		break;
		case DIR_LEFT:	b->x--;		break;
		case DIR_RIGHT:	b->x++;		break;

		default:
		return;
	}

	Tile* t = map_->getTileAt(b->x, b->y); // check if there is obstructing tile at the bullet's position
	tile_t et  = t->type;

	if (et == TILE_WALL || et == TILE_DOOR || et == TILE_GATE)
	{
		p->destroy_bullet();
	}

	// kill monster & destroy bullet
	for (uint i = 0; i < monsters.size(); i++)
	{
		if (!monsters.at(i)->alive)
			continue;

		uint x = monsters.at(i)->x;
		uint y = monsters.at(i)->y;

		if (b->x == x && b->y == y)
		{
			// kill monster
			std::cout << p->name << " killed a monster!" << std::endl;
			monsters.at(i)->kill();
			map_->getTileAt(monsters.at(i)->x, monsters.at(i)->y)->solid = false;

			/*
			// remove monster from vector
			std::vector<Monster*>::iterator it = monsters_.begin();
			std::advance(it, i);
			monsters_.erase(it);
			*/

			p->destroy_bullet();

			// some gold perchance?
			if (rand() % 2 == 1) // 50%
			{
				std::cout << "The monster dropped some gold!" << std::endl;
				map_->putGoldAt(x, y, rand() % 250);
			}
		}
	}
}

// update bullet positions for players
void Game::update_bullets(int value)
{
	update_bullet(player1);
	update_bullet(player2);

	glutPostRedisplay();
	glutTimerFunc(50, update_bullets, 0); // reset timer
}

void Game::update_monsters(int value)
{
	// for each monster: move, based on provided strategy (AI)
	for (uint i = 0; i < monsters.size(); i++)
	{	
		if (!monsters.at(i)->alive)
			continue;

		monsters.at(i)->update(); // AI routine
		
		// check if it collided with a player
		if (player1->alive)
			if (player1->x == monsters.at(i)->x && player1->y == monsters.at(i)->y)
			{   
				std::cout << player1->name << " was killed by a roaming monster!" << std::endl;
				player1->kill();
				map_->getTileAt(player1->x, player1->y)->solid = false; // no longer forms an obstruction for other entities
			}
		
		if (player2)
			if (player2->alive)
				if (player2->x == monsters.at(i)->x && player2->y == monsters.at(i)->y)
				{
					std::cout << player2->name << " was killed by a roaming monster!" << std::endl;
					player2->kill();
					map_->getTileAt(player2->x, player2->y)->solid = false;
				}
	}

	glutPostRedisplay();
	glutTimerFunc(500, update_monsters, 0); // reset timer
}

void Game::update()
{
	if (!map_)
		return;

	bool endGame = false;

	// check if the players are no longer with us
	if (!player1->alive) // player one is dead
	{
		if (player2)
		{
			if (!player2->alive) // player two is dead as well
				endGame = true;
		}
		else // player one is dead and player two doesn't exist
		{
			endGame = true;
		}
	}

	// ending the game when a player opens the exit gate is handled in processMovement

	// Check if game over
	if (endGame)
	{
		end_game();
		return;
	}

	glutPostRedisplay();
}

void Game::draw(void)
{
	// Draw the scene
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	// Draw the objects (excluding monsters / players)
	const std::vector<Tile*>& tiles = map_->getTiles();

	for (uint i = 0; i < tiles.size(); i++)
		if (tiles.at(i)) draw_tile(tiles.at(i));

	// draw monsters
	for (uint i = 0; i < monsters.size(); i++)
		draw_entity(monsters.at(i));
	
	// draw players
	if (player1) draw_entity(player1);

	if (player1->alive)
	{
		bullet_t* p1b = player1->bullet;
		if (p1b) draw_bullet(p1b);
	}

	if (player2)
	{
		draw_entity(player2);
			
		if (player2->alive)
		{
			bullet_t* p2b = player2->bullet;
			if (p2b) draw_bullet(p2b);
		}
	}

	glutSwapBuffers();
}

void Game::init(int argc, char* argv[])
{
	// Initialization of the window
	glutInit(&argc, argv);

	// Size and position
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(200, 200);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	// Name
	glutCreateWindow("Dungeon Adventure");

	// Load map file names from arguments
	int i = 1;

	while(argv[i])
	{
		map_files.push_back(std::string(argv[i]));
		++i;
	}

	// Initialize randomizer
	srand((uint)time(0));

	// Initialize map
	map_ = Map::Instance();

	if (!map_files.empty()) // start the first map
	{
		if (!load_map(map_files.at(current_map))) return; // map failed to load
	}
	else
	{
		std::cout << "Fatal Error: No Map files passed as parameters" << std::endl;
		exit();

		return;
	}

	// Managers
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutDisplayFunc(draw);
	glutIdleFunc(update);

	// timers
	glutTimerFunc(50, update_bullets, 0);
	glutTimerFunc(500, update_monsters, 0);
}

bool Game::load_map(const std::string& file)
{
	// prepare container
	std::vector<entity*> entities;

	// reset entities
	reset_map();

	// create map
	map_->createMap(file, entities);
	++current_map;

	if (entities.empty()) // should never be empty
	{
		std::cout << "Fatal Error: Map construction failed" << std::endl;
		end_game(); // end this game, allowing the game to open any other map files provided

		return false; // loadmap failed
	}

	// the first two items should be players
	player1 = dynamic_cast<player*>(entities.at(0)); // should this cast fail, playGame will then end the game prematurely

	if (entities.size() > 1)
		player2 = dynamic_cast<player*>(entities.at(1));

	// load the other entities (which should be monsters) into our monsters_ vector
	for (uint i = 2; i < entities.size(); i++)
		monsters.push_back(dynamic_cast<monster*>(entities.at(i)));

	std::cout << "Loading map " << file << std::endl;

	// Point of view
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLdouble limitX = map_->getMapWidth() / 2.0;
	GLdouble limitY = map_->getMapHeight() / 2.0;
	glOrtho(-limitX, limitX, -limitY, limitY, -1.0, 1.0);

	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	glTranslated(-limitX, -limitY, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	return true;
}

void Game::play()
{
	if (!map_)
	{
		std::cout << "Fatal Error: Map could not be created";
		return; // will not enter glutMainLoop and hence terminate the program
	}

	if (!player1)
	{
		std::cout << "Fatal Error: Player could not be created";
		return;
	}

	// Game loop is managed by the window
	player1->name = "Player1";
	player2->name = "Player2";

	glutMainLoop();
}

void Game::end_game()
{
	std::cout << "The Game has ended!" << std::endl;

	if (map_files.size() <= current_map)
		exit();

	// more maps to play
	load_map(map_files.at(current_map));
}

void Game::reset_map()
{
	for (uint i = 0; i < monsters.size(); i++)
	{
		delete monsters.at(i);
		monsters.at(i) = nullptr;
	}

	monsters.clear();

	delete player1;
	delete player2;

	player1 = nullptr;
	player2 = nullptr;
}
