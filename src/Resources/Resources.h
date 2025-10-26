#pragma once

#include "Resources/Image.h"
#include "Resources/Material.h"
#include "Resources/Model.h"
#include "Resources/Sound.h"

#include "Resources/HashMap.h"
#include "Resources/Resource.h"

#include "Resources/AnimatedMap.h"

using ImagePtr = ResourcePtr<Image>;
using MaterialPtr = ResourcePtr<Material>;
using ModelPtr = ResourcePtr<Model>;
using SoundPtr = ResourcePtr<Sound>;

class Lexer;

class ResourceManager
{
private:
    static HashMap<Image> m_images;
    static HashMap<Material> m_materials;
    static HashMap<Model> m_models;
    static HashMap<Sound> m_sounds;

    static Image* m_flatNormal;

    static std::vector<AnimatedMap> m_animatedMaps;

    static bool LoadMaterialMap(Lexer& lexer, Material* material, int mapn);
    static bool ReadParam(Lexer& lexer, float& param);
    static bool ReadString(Lexer& lexer, std::string& str);
    static bool ReadColor(Lexer& lexer, vec3& color);
    static bool ReadType(Lexer& lexer, Material::MaterialType& type);

    static void CleanupAnimatedMaps();

public:

    static void AnimateMaps(float dt);

    static Image* GetImage(const std::string& name);
    static Material* GetMaterial(const std::string& name);
    static Model* GetModel(const std::string& name);
    static Sound* GetSound(const std::string& name);

    static bool MaterialExist(const std::string& name);
    static bool ModelExist(const std::string& name);

    static void Cleanup();
};