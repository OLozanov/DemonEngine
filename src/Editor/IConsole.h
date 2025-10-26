#pragma once

#include "Resources/Resources.h"

class IConsole
{
public:
    ~IConsole() = default;

    virtual void addMaterial(const std::string& name, Material* mat) = 0;
    virtual void addModel(const std::string& name, Model* mat) {}
};