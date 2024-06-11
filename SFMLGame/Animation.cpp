#include "Animation.h"
#include <cmath>
#include <iostream>

Animation::Animation() {};
Animation::Animation(const std::string& name, const sf::Texture& t) 
	: Animation(name, t, 1, 0)
{

}

Animation::Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
	: m_name(name)
	, m_sprite(t)
	, m_frameCount(frameCount)
	, m_currentFrame(0)
	, m_speed(speed)
{
	m_size = Vec2((float)t.getSize().x / frameCount, (float)t.getSize().y);
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
	m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_size.x, 0, m_size.x, m_size.y));
}

//Updates the animation to show the next frame, depending on its speed
//Animation loops until it reaches the end
void Animation::update()
{
	//Add speed variable to current frame
	m_currentFrame++;
	if (m_currentFrame == (m_speed * m_frameCount))
		m_currentFrame = 0;

	if(m_speed != 0)
		m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame / m_speed) * m_size.x, 0, m_size.x, m_size.y));
	//TODO: 1) Calculate the correct frame of animation to play based on currentFrame and speed
	//		2) set the texture rectangle properly (see constructor for sample)
	
}

const Vec2& Animation::getSize() const
{
	return m_size;
}

const std::string& Animation::getName() const
{
	return m_name;
}

sf::Sprite& Animation::getSprite()
{
	return m_sprite;
}

bool Animation::hasEnded() const
{
	//TODO : Detect when animation has ended and return true
	return false;
}