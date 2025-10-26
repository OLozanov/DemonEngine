#pragma once

#include "Resources/Resources.h"

inline Material* LoadMaterial(const std::string& name)
{
    bool exist = ResourceManager::MaterialExist(name);

    Material* mat = ResourceManager::GetMaterial(name);

    if (mat && !exist) mat->name = name;

    return mat;
}
