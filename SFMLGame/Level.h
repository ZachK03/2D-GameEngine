#pragma once
#include <string>
class Level
{
protected:
	std::string& levelPath;
	std::string& levelName;
public:
	Level(std::string& path, std::string& name);

	void setName(std::string& name);
};

