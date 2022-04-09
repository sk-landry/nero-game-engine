////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2022 sk-landry
/////////////////////////////////////////////////////////////
#ifndef FRONTSCREEN_H
#define FRONTSCREEN_H
///////////////////////////HEADERS//////////////////////////
//NERO
#include <Nero/object/Object.h>
#include <Nero/object/UIObject.h>
/////////////////////////////////////////////////////////////
namespace nero
{
    struct Screen
    {
        typedef std::shared_ptr<Screen>     Ptr;

        Object::Ptr     screen;
        UIObject::Ptr   screenUI;
        std::string     name;
        bool            hide;
        sf::Color       canvasColor;
    };
}
#endif // FRONTSCREEN_H
