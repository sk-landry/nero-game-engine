////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2019 SANOU A. K. Landry
////////////////////////////////////////////////////////////
///////////////////////////HEADERS///////////////////////////
//NERO
#include <Nero/core/resource/SoundHolder.h>
#include <Nero/core/utility/Utility.h>
//BOOST
#include <experimental/filesystem>
////////////////////////////////////////////////////////////
namespace nero
{
    SoundHolder::SoundHolder()
    {

    }

	SoundHolder::SoundHolder(const Setting& setting) : ResourceHolder (setting)
	{

	}


	SoundHolder::~SoundHolder()
	{
		destroy();
	};

	void SoundHolder::destroy()
	{

	}

    void SoundHolder::addSoundBuffer(std::string name, std::unique_ptr<sf::SoundBuffer> soundBuffer)
    {
        auto newSoundBuffer = m_SoundBufferMap.insert(std::make_pair(name, std::move(soundBuffer)));
        assert(newSoundBuffer.second);

        m_SoundBufferTable.push_back(name);
    }

    sf::SoundBuffer& SoundHolder::getSoundBuffer(std::string name)
    {
        auto found = m_SoundBufferMap.find(name);
        assert(found != m_SoundBufferMap.end());

        return *found->second;
    }

    const sf::SoundBuffer& SoundHolder::getSoundBuffer(std::string name) const
    {
        auto found = m_SoundBufferMap.find(name);
        assert(found != m_SoundBufferMap.end());

        return *found->second;
    }

    const std::vector<std::string>& SoundHolder::getSoundBufferTable() const
    {
        return m_SoundBufferTable;
    }

	void SoundHolder::loadFile(const std::string& file)
	{
		std::experimental::filesystem::path filePath(file);

		std::unique_ptr<sf::SoundBuffer> soundBuffer = std::make_unique<sf::SoundBuffer>();

		const std::string name = filePath.filename().stem().string();

		if (!soundBuffer->loadFromFile(filePath.string()))
		{
			nero_log("unable to load sound : " + name);
			return;
		}

		addSoundBuffer(name, std::move(soundBuffer));

		nero_log("loaded : " + name);
	}


	void SoundHolder::loadDirectory()
    {
		if(m_SelectedDirectory == StringPool.BLANK)
		{
			nero_log("failed to load directory");
			return;
		}

		nero_log("resource path : " + m_SelectedDirectory);

		std::experimental::filesystem::path folderPath(m_SelectedDirectory);

		if(!directoryExist(m_SelectedDirectory))
        {
            nero_log("unable to load sound resource");
			nero_log("folder not found : " + m_SelectedDirectory);
            assert(false);
        }

		std::experimental::filesystem::directory_iterator it{folderPath};
		while(it != std::experimental::filesystem::directory_iterator{})
        {
			if(checkExtention(it->path().extension().string(), m_Setting.getStringTable("extension")))
            {
				loadFile(it->path().string());
            }

            it++;
        }
    }
}