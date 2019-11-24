////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2020 SANOU A. K. Landry
/////////////////////////////////////////////////////////////
#ifndef SCENE_H
#define SCENE_H
///////////////////////////HEADERS///////////////////////////
//NERO
#include <Nero/core/object/GameObject.h>
//SFML
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
//BOX2D
#include <Box2D/Dynamics/b2World.h>
//STD
#include <memory>
#include <vector>
/////////////////////////////////////////////////////////////
namespace nero
{
	class  Scene
    {
        public: //Types Definiton
			typedef std::shared_ptr<Scene> Ptr;
			typedef std::vector<GameObject::Ptr> WorldChunkTab;

        public: //Utility Class
            struct Context
            {
                Context();
            };


		public: //Constructor
											Scene(Context context);
			virtual                        ~Scene();

			virtual void                    handleEvent(const sf::Event& event);
			virtual void                    update(const sf::Time& timeStep);
			virtual void                    render();

			void    renderScene();

            sf::RenderTexture&              getRenderTexture();
			virtual std::string             getName();


        private:
            void                            destroyScene();

        protected:
            //Base Attributs
            Context                             m_SceneContext;
            b2World*                            m_PhysicWorld;
            GameObject                          m_GameWorld;
            //Containers
            std::vector<GameObject::Ptr>        m_WorldChunkTable;
            //
            sf::RenderTexture                   m_RenderTexture;
			Context								m_Context;

    };
}

#endif // SCENE_H