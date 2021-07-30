////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2021 Sanou A. K. Landry
////////////////////////////////////////////////////////////
///////////////////////////HEADERS//////////////////////////
//Nero
#include <Nero/editor/level/LevelBuilder.h>
////////////////////////////////////////////////////////////
namespace nero
{
	LevelBuilder::LevelBuilder():
		 m_RenderContext(nullptr)
		,m_RenderTexture(nullptr)
		,m_LevelSetting(std::make_shared<Setting>())
		,m_EditorSetting(nullptr)
		,m_ResourceManager(nullptr)
		,m_SelectedChunk(nullptr)
		,m_ChunkTable()
		,m_ChunkCount(0)
		,m_Opened(false)
	{

	}

	LevelBuilder::~LevelBuilder()
	{

	}

	void LevelBuilder::loadResource()
	{
		m_ResourceManager = std::make_shared<ResourceManager>(m_EditorSetting->getSetting("resource"));
		m_ResourceManager->loadDirectory(m_LevelSetting->getString("resource_directory"));
	}

	ResourceManager::Ptr LevelBuilder::getResourceManager()
	{
		return m_ResourceManager;
	}

	void LevelBuilder::setEditorSetting(const Setting::Ptr &setting)
	{
		m_EditorSetting = setting;
	}

	Setting::Ptr LevelBuilder::getLevelSetting()
	{
		return m_LevelSetting;
	}

	std::string LevelBuilder::getLevelName()
	{
		return m_LevelSetting->getString("level_name");
	}

	std::string LevelBuilder::getResourceFoler()
	{
		return m_LevelSetting->getString("resource_directory");
	}

	ChunkBuilder::Ptr LevelBuilder::addChunk()
	{
		//create
		ChunkBuilder::Ptr chunkBuilder = std::make_shared<ChunkBuilder>();
		chunkBuilder->setChunkId(++m_ChunkCount);
		chunkBuilder->setChunkName(std::string("world chunk ") + toString(chunkBuilder->getChunkId()));
		chunkBuilder->setSelected(false);
		chunkBuilder->setVisible(true);
		chunkBuilder->setAutoLoad(false);

		WorldBuilder::Ptr worldBuilder = chunkBuilder->getWorldBuilder();
		worldBuilder->setRenderContext(m_RenderContext);
		worldBuilder->setRenderTexture(m_RenderTexture);
		worldBuilder->setResourceManager(m_ResourceManager);
		worldBuilder->init();

		//register
		m_ChunkTable.push_back(chunkBuilder);

		//select
		setSelectedChunk(chunkBuilder);

		return chunkBuilder;
	}

	void LevelBuilder::removeChunk()
	{
		//TODO remove chunk
	}

	std::vector<ChunkBuilder::Ptr>& LevelBuilder::getChunkTable()
	{
		return m_ChunkTable;
	}

	ChunkBuilder::Ptr LevelBuilder::getSelectedChunk()
	{
		return m_SelectedChunk;
	}

	void LevelBuilder::setSelectedChunk(ChunkBuilder::Ptr worldChunk)
	{
		m_SelectedChunk = worldChunk;
	}

	void LevelBuilder::setRenderContext(const RenderContext::Ptr& renderContext)
	{
		m_RenderContext = renderContext;
	}

	void LevelBuilder::setRenderTexture(const std::shared_ptr<sf::RenderTexture>& renderTexture)
	{
		m_RenderTexture = renderTexture;
	}

	void LevelBuilder::saveGameLevel()
	{
		//chunk
		for(auto chunk : m_ChunkTable)
		{
			//update is_seleted
			chunk->getChunkId() == m_SelectedChunk->getChunkId() ? chunk->setSelected(true) : chunk->setSelected(false);

			//save chunk
			file::saveFile(
				file::getPath({m_LevelSetting->getString("chunk_directory"), chunk->getChunkName()}, StringPool.EXT_NERO),
				chunk->saveChunk().dump(3), true);
		}

		//setting
		m_LevelSetting->setInt("chunk_count", m_ChunkCount);
		m_LevelSetting->setBool("opened", m_Opened);

		file::saveFile(
			file::getPath({m_LevelSetting->getString("level_directory"), "setting"}, StringPool.EXT_NERO),
			m_LevelSetting->toString(), true);
	}

	void LevelBuilder::loadGameLevel()
	{
		std::experimental::filesystem::path chunkDirectory(m_LevelSetting->getString("chunk_directory"));

		std::experimental::filesystem::directory_iterator it{chunkDirectory};
		while(it != std::experimental::filesystem::directory_iterator{})
		{
			loadChunk(it->path().string());

			it++;
		}

		m_Opened		= m_LevelSetting->getBool("opened");
		m_ChunkCount	= m_LevelSetting->getInt("chunk_count");
	}

	void LevelBuilder::loadChunk(const std::string& fileName)
	{
		ChunkBuilder::Ptr chunkBuilder = std::make_shared<ChunkBuilder>();
		WorldBuilder::Ptr worldBuilder = chunkBuilder->getWorldBuilder();

		worldBuilder->setRenderContext(m_RenderContext);
		worldBuilder->setRenderTexture(m_RenderTexture);
		worldBuilder->setResourceManager(m_ResourceManager);

		chunkBuilder->loadChunk(file::loadJson(fileName, true));

		if(chunkBuilder->isSelected())
		{
			setSelectedChunk(chunkBuilder);
		}

		m_ChunkTable.push_back(chunkBuilder);
	}

	void LevelBuilder::setOpened(const bool& opened)
	{
		m_Opened = opened;
	}

	bool LevelBuilder::isOpened() const
	{
		return m_Opened;
	}
}
