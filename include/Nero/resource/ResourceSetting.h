////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2019 SANOU A. K. Landry
/////////////////////////////////////////////////////////////
#ifndef RESOURCESETTING_H
#define RESOURCESETTING_HCopyright (c) 2016-2021 Sanou A. K. Landry
///////////////////////////HEADERS//////////////////////////
//JSON
#include <json/json.hpp>
////////////////////////////////////////////////////////////
namespace nero
{
    struct ResourceSetting
    {
        static  nlohmann::json toJson();
    };
}
#endif // RESOURCESETTING_H
