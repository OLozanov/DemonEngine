#include "AnimatedMap.h"
#include "Resources/Resources.h"

AnimatedMap::AnimatedMap(Material * material, int mapnum, std::string name)
: m_material(material)
, m_map(mapnum)
, m_time(0.0)
, m_frame(0)
{
    size_t pos = name.find('$');

    std::string body(name, 0, pos);
    std::string suffix(name, pos+1);
    std::string fname;

    for(int i = 1; ; i++)
    {
        fname = body;
        fname += std::to_string(i);
        fname += suffix;

        Image * img = ResourceManager::GetImage(fname.c_str());

        if(img) m_images.emplace_back(img);
        else break;
    }

    m_duration = m_images.size()*AnimRate;

    m_material->maps[m_map] = m_images[0]->handle;
}

size_t AnimatedMap::referenceCount()
{
    return m_material->referenceCount();
}

void AnimatedMap::update(float dt)
{
    m_time += dt;

    if(m_time >= m_duration) m_time -= m_duration;
    int frame = m_time/AnimRate;

    if(frame >= m_images.size())
    {
        m_time -= ((int)(m_time/m_duration))*m_duration;
        frame = m_time/AnimRate;
    }

    if(frame != m_frame)
    {
        m_frame = frame;
        m_material->maps[m_map] = m_images[m_frame]->handle;
    }
}
