////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2023 Sanou A. K. Landry
/////////////////////////////////////////////////////////////
#ifndef MESHEDITOR_H
#define MESHEDITOR_H
///////////////////////////HEADERS//////////////////////////
// Nero
#include <Nero/core/cpp/model/PointMesh.h>
#include <Nero/core/cpp/model/PolygonMesh.h>
#include <Nero/core/cpp/model/CircleMesh.h>
#include <Nero/core/cpp/scene/SceneUtility.h>
#include <SFML/Graphics/RenderTexture.hpp>
// Std
#include <vector>
////////////////////////////////////////////////////////////
namespace nero
{
    class MeshEditor
    {
      public:
        typedef std::shared_ptr<MeshEditor>        Ptr;
        typedef std::shared_ptr<sf::RenderTexture> RenderTexturePtr;
        typedef sf::RectangleShape                 Vertex;
        typedef std::vector<PointMesh::Ptr>        MeshTable;

        MeshEditor();
        virtual ~MeshEditor();

        void handleEvent(const sf::Event& event);

        void addMesh(PointMesh::Ptr mesh);
        void deleteMesh(const int& id);
        void clearMeshTable();

        void setUpdateUndo(std::function<void()> fn);
        void setUpdateLog(std::function<void(const std::string&, int)> fn);
        void setUpdateLogIf(std::function<void(const std::string&, bool, int)> fn);
        void setRenderContext(const RenderContext::Ptr& renderContext);
        void setRenderTexture(const RenderTexturePtr& renderTexture);

      private: // Methods
        void         handleKeyboardInput(const sf::Keyboard::Key& key, const bool& isPressed);
        void         handleMouseButtonsInput(const sf::Event::MouseButtonEvent& mouse,
                                             const bool&                        isPressed);
        void         handleMouseMoveInput(const sf::Event::MouseMoveEvent& mouse);
        void         rotateMesh(PointMesh::Ptr mesh, float speed = 0.1f);
        void         scaleMesh(PointMesh::Ptr mesh, float scale = 0.1f);
        void         selectMesh(PointMesh::Ptr pointMesh);
        void         unselectMesh(PointMesh::Ptr pointMesh);
        sf::Vector2f getMouseWorldPosition() const;
        //
        bool         handleLeftClickPressOnVertex(const PointMesh::Ptr& pointMesh);
        bool         handleLeftClickPressOnLine(const PointMesh::Ptr& pointMesh);
        bool         handleLeftClickPressOnPolygon(const PolygonMesh::Ptr& polygonMesh);
        bool         handleLeftClickPressOnCircle(const CircleMesh::Ptr& circleMesh);
        bool         handleReleaseLeftClick(const sf::Event::MouseButtonEvent&);
        bool         handleRightClickPressOnVertex(const PointMesh::Ptr& pointMesh);
        bool         handleRightClickPressOnLine(const PointMesh::Ptr& pointMesh);
        void         handleLeftClickRelease();
        void         handleRightClickRelease();

      private:
        MeshTable                                          m_MeshTable;
        PointMesh::Ptr                                     m_SelectedMesh;
        std::vector<sf::RectangleShape*>                   m_SelectedVertexTable;
        int                                                m_MeshCount;
        RenderTexturePtr                                   m_RenderTexture;
        RenderContext::Ptr                                 m_RenderContext;
        sf::Vector2f                                       m_LastMousePosition;
        std::function<void()>                              m_UpdateUndo;
        std::function<void(const std::string&, int)>       m_UpdateLog;
        std::function<void(const std::string&, bool, int)> m_UpdateLogIf;
        sf::Vector2f                                       m_Epsilon;
        bool                                               m_LeftSelection;
    };
} // namespace nero
#endif // MESHEDITOR_H
