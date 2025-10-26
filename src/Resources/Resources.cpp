#include "Resources.h"

#include "Resources/Formats/Formats.h"

#include "Utils/Lexer.h"
#include "Render/Render.h"

#include <iostream>

HashMap<Image> ResourceManager::m_images;
HashMap<Material> ResourceManager::m_materials;
HashMap<Model> ResourceManager::m_models;
HashMap<Sound> ResourceManager::m_sounds;

Image* ResourceManager::m_flatNormal = nullptr;

std::vector<AnimatedMap> ResourceManager::m_animatedMaps;

Material::~Material()
{
}

inline std::string GetFullPath()
{
    constexpr DWORD BufferSize = 512;

    CHAR pathBuffer[BufferSize];

    DWORD size = GetModuleFileNameA(nullptr, pathBuffer, BufferSize);

    if (size == 0 || size == BufferSize)
    {
        return {};
    }

    CHAR* lastSlash = strrchr(pathBuffer, L'\\');

    if (lastSlash)
    {
        *(lastSlash + 1) = L'\0';
    }

    return std::string(pathBuffer);
}

void ResourceManager::AnimateMaps(float dt)
{
    for (auto& animatedMap : m_animatedMaps) animatedMap.update(dt);
}

Image* ResourceManager::GetImage(const std::string& name)
{
    Image* image = m_images[name];

    if (!image)
    {
        std::string path = GetFullPath() + "Textures\\" + name;

        size_t extpos = path.find_last_of('.');
        std::string extension = extpos != path.npos ? path.substr(++extpos) : "";

        image = nullptr;

        if (extension == "bmp") image = LoadBmp(path.c_str());
        if (extension == "tga") image = LoadTga(path.c_str());
        if (extension == "dds") image = LoadDds(path.c_str());

        if (image)
        {
            Render::GpuInstance& gpuInstance = Render::GpuInstance::GetInstance();

            gpuInstance.createTexture(image);
            m_images.add(name, image);
        }
	}

    return image;
}

bool ResourceManager::LoadMaterialMap(Lexer& lexer, Material* material, int mapn)
{
    if (!lexer.match(Lexer::lex_assign)) return false;

    Lexer::Token tk = lexer.read();

    if (tk == Lexer::lex_string)
    {
        const std::string& tname = lexer.tokenValue();

        size_t pos = tname.find('$');

        if (pos != std::string::npos)
        {
            m_animatedMaps.emplace_back(material, mapn, tname);
        }
        else
        {
            Image* image = GetImage(tname);
            material->img[mapn] = image;
            material->maps[mapn] = image->handle;
        }
    }

    return true;
}

bool ResourceManager::ReadParam(Lexer& lexer, float& param)
{
    if (!lexer.match(Lexer::lex_assign)) return false;
    Lexer::Token tk = lexer.read();

    if (tk == Lexer::lex_num) param = atof(lexer.tokenValue().c_str());

    return true;
}

bool ResourceManager::ReadString(Lexer& lexer, std::string& str)
{
    if (!lexer.match(Lexer::lex_assign)) return false;
    Lexer::Token tk = lexer.read();

    if (tk == Lexer::lex_string) str = lexer.tokenValue();

    return true;
}

bool ResourceManager::ReadColor(Lexer& lexer, vec3& color)
{
    if (!lexer.match(Lexer::lex_assign)) return false;
    if (!lexer.match(Lexer::lex_bropen)) return false;

    Lexer::Token tk = lexer.read();
    if (tk == Lexer::lex_num) color.x = atof(lexer.tokenValue().c_str());
    else return false;

    if (!lexer.match(Lexer::lex_coma)) return false;

    tk = lexer.read();
    if (tk == Lexer::lex_num) color.y = atof(lexer.tokenValue().c_str());
    else return false;

    if (!lexer.match(Lexer::lex_coma)) return false;

    tk = lexer.read();
    if (tk == Lexer::lex_num) color.z = atof(lexer.tokenValue().c_str());
    else return false;

    if (!lexer.match(Lexer::lex_brclose)) return false;

    return true;
}

bool ResourceManager::ReadType(Lexer& lexer, Material::MaterialType& type)
{
    if (!lexer.match(Lexer::lex_colon)) return false;
    
    Lexer::Token tk = lexer.read();

    if (tk == Lexer::lex_id)
    {
        const std::string& value = lexer.tokenValue();

        type = Material::material_regular;

        if (value == "transparent") type = Material::material_transparent;
        if (value == "emissive") type = Material::material_emissive;
    }

    return true;
}

/*bool ResourceManager::ReadDetail(Lexer& lexer, Material::MaterialDetail& detail)
{
    if (!lexer.match(Lexer::lex_blopen)) return false;

    while(true)
    {  
        Lexer::Token tk = lexer.read();

        if (tk == Lexer::lex_id)
        {
            const std::string value = lexer.tokenValue();

            if (value == "model")
            {
                std::string model;

                if (!ReadString(lexer, model)) return false;

                detail.model = GetModel(model);
            }
                
            if (value == "density") if (!ReadParam(lexer, detail.density)) return false;

            continue;
        }

        if (tk == Lexer::lex_blclose) break;

        return false;
    }

    return true;
}*/

Material* ResourceManager::GetMaterial(const std::string& name)
{
    Material* material = m_materials[name];

    if (!material)
    {
        material = new Material();

        std::string filename = "Textures\\";
        filename += name;
        filename += ".mtl";

        try
        {
            Lexer lexer(filename);

            if (!lexer.match(Lexer::lex_id, "material")) return nullptr;
            if (!lexer.match(Lexer::lex_blopen)) return nullptr;

            material->maps[Material::map_diffuse] = 1;
            material->maps[Material::map_roughness] = 1;
            material->maps[Material::map_metalness] = 1;
            material->maps[Material::map_luminosity] = 1;
            material->maps[Material::map_height] = 1;

            while (1)
            {
                Lexer::Token tk = lexer.read();

                if (tk == Lexer::lex_id)
                {
                    // Maps
                    if (lexer.tokenValue() == "diffuse")
                    {
                        LoadMaterialMap(lexer, material, Material::map_diffuse);
                        material->flags |= Material::mat_diffuse_map;
                        continue;
                    }

                    if (lexer.tokenValue() == "normal")
                    {
                        LoadMaterialMap(lexer, material, Material::map_normal);
                        material->flags |= Material::mat_normal_map;
                        continue;
                    }

                    if (lexer.tokenValue() == "roughness_map")
                    {
                        LoadMaterialMap(lexer, material, Material::map_roughness);
                        material->flags |= Material::mat_roughness_map;
                        continue;
                    }

                    if (lexer.tokenValue() == "metalness_map")
                    {
                        LoadMaterialMap(lexer, material, Material::map_metalness);
                        material->flags |= Material::mat_metalness_map;
                        continue;
                    }

                    if (lexer.tokenValue() == "luminosity_map")
                    {
                        LoadMaterialMap(lexer, material, Material::map_luminosity);
                        material->flags |= Material::mat_luminosity_map;
                        continue;
                    }

                    if (lexer.tokenValue() == "height_map")
                    {
                        LoadMaterialMap(lexer, material, Material::map_height);
                        material->flags |= Material::mat_height_map;
                        continue;
                    }

                    // Parameters
                    if (lexer.tokenValue() == "color") ReadColor(lexer, material->color);
                    if (lexer.tokenValue() == "alpha") ReadParam(lexer, material->alpha);
                    if (lexer.tokenValue() == "metalness") ReadParam(lexer, material->metalness);
                    if (lexer.tokenValue() == "roughness") ReadParam(lexer, material->roughness);
                    if (lexer.tokenValue() == "luminosity") ReadParam(lexer, material->luminosity);

                    // Type
                    if (lexer.tokenValue() == "type") ReadType(lexer, material->type);
                }

                if (tk == Lexer::lex_blclose)
                {
                    if (!lexer.match(Lexer::lex_eof)) return nullptr;
                    break;
                }
            }

        }
        catch (...)
        {
            std::cout << "Fail to parse material file " << filename << std::endl;
        }

        if (!material->img[Material::map_normal])
        {
            if (!m_flatNormal)
            {
                m_flatNormal = GetImage("flat_n.dds");
                m_flatNormal->addReference();
            }

            material->maps[Material::map_normal] = m_flatNormal->handle;
        }

        m_materials.add(name, material);
    }

    return material;
}

Model* ResourceManager::GetModel(const std::string& name)
{
    Model* model = m_models[name];

    if (!model)
    {
        std::string path = std::string("Models\\") + name;
        model = Model::LoadModel(path.c_str());
        m_models.add(name, model);
    }

    return model;
}

Sound* ResourceManager::GetSound(const std::string& name)
{
    Sound* sound = m_sounds[name];

    if (!sound)
    {
        std::string path = GetFullPath() + "Sounds\\" + name;

        size_t extpos = path.find_last_of('.');
        std::string extension = extpos != path.npos ? path.substr(++extpos) : "";

        if (extension == "wav") sound = LoadWav(path.c_str());
    }

    return sound;
}

bool ResourceManager::MaterialExist(const std::string& name)
{
    return m_materials.exist(name);
}

bool ResourceManager::ModelExist(const std::string& name)
{
    return m_models.exist(name);
}

void ResourceManager::CleanupAnimatedMaps()
{
    std::vector<AnimatedMap> animatedMaps;

    for (AnimatedMap& map : m_animatedMaps)
    {
        if (map.referenceCount() != 0) animatedMaps.push_back(map);
    }

    m_animatedMaps.swap(animatedMaps);
}

void ResourceManager::Cleanup()
{
    m_models.cleanup();
    CleanupAnimatedMaps();
    m_materials.cleanup();
    m_images.cleanup();
    m_sounds.cleanup();
}