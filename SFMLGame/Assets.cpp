#include "Assets.h"
#include <cassert>
#include <iostream>
#include <fstream>

Assets::Assets() {}

void Assets::loadFromFile(const std::string& path)
{
	std::ifstream file(path);
	std::string str;
	while (file.good())
	{
		file >> str;
		if (str == "Texture")
		{
			std::string name, path;
			file >> name >> path;
			addTexture(name, path, true);
		}
		else if (str == "Animation")
		{
			std::string name, texture;
			size_t frames, speed;
			file >> name >> texture >> frames >> speed;
			addAnimation(name, texture, frames, speed);
		}
		else if (str == "Font")
		{
			std::string name, path;
			file >> name >> path;
			addFont(name, path);
		}
		else
		{
			std::cerr << "Unknown asset type: " << str << std::endl;
		}
	}
}

void Assets::addTexture(const std::string& textureName, const std::string& path, bool smooth)
{
	m_textureMap[textureName] = sf::Texture();
	if (!m_textureMap[textureName].loadFromFile(path))
	{
		std::cerr << "Could not load texture file: " << path << std::endl;
		m_textureMap.erase(textureName);
	}
	else
	{
		m_textureMap[textureName].setSmooth(smooth);
		std::cout << "Loaded texture: " << path << std::endl;
	}
}

void Assets::addAnimation(const std::string& animationName, const std::string& textureName, size_t frameCount, size_t speed)
{
	if (auto search = m_textureMap.find(textureName); search == m_textureMap.end())
	{
		std::cerr << "Could not find texture for animation: " << animationName << std::endl;
		return;
	}
	m_animationMap[animationName] = Animation(animationName, m_textureMap[textureName], frameCount, speed);
	std::cout << "Loaded animation: " << animationName << std::endl;
}

void Assets::addFont(const std::string& fontName, const std::string& path)
{
	m_fontMap[fontName] = sf::Font();
	if (!m_fontMap[fontName].loadFromFile(path))
	{
		std::cerr << "Could not load font: " << path << std::endl;
		m_fontMap.erase(fontName);
	}
	else
	{
		std::cout << "Font loaded: " << path << std::endl;
	}
}

const sf::Texture& Assets::getTexture(const std::string& textureName) const
{
	const sf::Texture& tex = m_textureMap.at(textureName);
	return tex;
}

const Animation& Assets::getAnimation(const std::string& animName) const
{
	const Animation& anim = m_animationMap.at(animName);
	return anim;
}

const sf::Font& Assets::getFont(const std::string& fontName) const
{
	const sf::Font& font = m_fontMap.at(fontName);
	return font;
}