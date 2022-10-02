#include "Assets.h"
#include <cassert>

Assets::Assets()
{

}

void Assets::loadFromFile(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cerr << "Could not load assets from file: " << path << std::endl;
		return;
	}

	std::string str;
	while (file >> str)
	{
		if (str == "Texture")
		{
			std::string name, path;
			file >> name >> path;
			addTexture(name, path);
		}
		else if (str == "Animation")
		{
			std::string name, texture;
			size_t frames, speed;
			file >> name >> texture >> frames >> speed;
			addAnimation(name, texture, frames, speed);
		}
		else if (str == "Sound")
		{
			std::string name, path;
			file >> name >> path;
			addSound(name, path);
		}
		else if (str == "Font")
		{
			std::string name, path;
			file >> name >> path;
			addFont(name, path);
		}
		else
		{
			std::cerr << "Unknown Asset Type: " << str << std::endl;
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
		std::cout << "Loaded Texture: " << path << std::endl;
	}
}

const sf::Texture& Assets::getTexture(const std::string& textureName) const
{
	auto it = m_textureMap.find(textureName);
	assert(it != m_textureMap.end());
	return it->second;
}

void Assets::addAnimation(const std::string& animationName, const std::string& textureName, size_t frameCount, size_t speed)
{
	m_animationMap[animationName] = Animation(animationName, getTexture(textureName), frameCount, speed);
	std::cout << "Added Animation: " << animationName << std::endl;
}

const Animation& Assets::getAnimation(const std::string& animationName) const
{
	auto it = m_animationMap.find(animationName);
	assert(it != m_animationMap.end());
	return it->second;
}

void Assets::addSound(const std::string& soundName, const std::string& path)
{
	m_soundBufferMap[soundName] = sf::SoundBuffer();
	if (!m_soundBufferMap[soundName].loadFromFile(path))
	{
		std::cerr << "Could not load sound file: " << path << std::endl;
		m_soundBufferMap.erase(soundName);
	}
	else
	{
		std::cout << "Loaded Sound: " << path << std::endl;
		m_soundMap[soundName] = sf::Sound(m_soundBufferMap[soundName]);
		m_soundMap[soundName].setVolume(25);
	}
}

sf::Sound& Assets::getSound(const std::string& soundName)
{
	auto it = m_soundMap.find(soundName);
	assert(it != m_soundMap.end());
	return it->second;
}

void Assets::addFont(const std::string& fontName, const std::string& path)
{
	m_fontMap[fontName] = sf::Font();
	if (!m_fontMap[fontName].loadFromFile(path))
	{
		std::cerr << "Could not load font file: " << path << std::endl;
		m_fontMap.erase(fontName);
	}
	else
	{
		std::cout << "Loaded Font: " << path << std::endl;
	}
}

const sf::Font& Assets::getFont(const std::string& fontName) const
{
	auto it = m_fontMap.find(fontName);
	assert(it != m_fontMap.end());
	return it->second;
}