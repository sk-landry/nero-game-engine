////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2023 Sanou A. K. Landry
////////////////////////////////////////////////////////////
#ifndef UICOMPONENT_H
#define UICOMPONENT_H
///////////////////////////HEADERS//////////////////////////
//Imgui
#include <imgui/imgui.h>
#include <imgui/imgui-SFML.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
//Nero
#include <Nero/editor/ui/EditorContext.h>
////////////////////////////////////////////////////////////
namespace nero
{
    class UIComponent
    {
        public:
                                    UIComponent(EditorContext::Ptr editorContext);
            virtual                ~UIComponent();

            virtual void 			destroy()   = 0;
            virtual void            render()    = 0;

        protected:
            EditorContext::Ptr      m_EditorContext;
	};
}

#endif // UICOMPONENT_H
