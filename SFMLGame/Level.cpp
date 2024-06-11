#include "Level.h"

Level::Level(std::string& path, std::string& name)
	: levelPath(path), levelName(name) {}
void Level::setName(std::string& name)
{
	levelName = name;
}