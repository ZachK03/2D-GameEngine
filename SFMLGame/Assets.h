#pragma once

#include "Common.h"
#include "Animation.h"

class Assets
{
	std::map<std::string, sf::Texture> m_textureMap;
	std::map<std::string, Animation> m_animationMap;
	std::map<std::string, sf::Font> m_fontMap;

	void addTexture(const std::string& textureName, const std::string& path, bool smooth = true);
	void addAnimation(const std::string& animationName, const std::string& textureName, size_t frameCount, size_t speed);
	void addFont(const std::string& fontName, const std::string& path);

	template <typename K, typename V>
	V GetWithDefault(const std::map<K, V>& m, const K& key, const V& defaultValue)
	{
		typename std::map<K, V>::const_iterator it = m.find(key);
		if (it == m.end())
		{
			return defaultValue;
		}
		else {
			return it->second;
		}
	}

public:
	Assets();
	void loadFromFile(const std::string& path);
	const sf::Texture& getTexture(const std::string& textureName) const;
	const Animation& getAnimation(const std::string& animationName) const;
	const sf::Font& getFont(const std::string& fontName) const;
};

