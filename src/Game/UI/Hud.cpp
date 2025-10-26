#include "Hud.h"
#include <string>

namespace GameLogic
{

Hud::Hud()
: UI::Widget(nullptr, 0, 0, 800, 600, flag_visible)
, m_crosshair(this, 800, 600, ResourceManager::GetImage("ui/crosshair.dds"), Alignment::Center)
, m_healthIcon(this, 800, 600, ResourceManager::GetImage("ui/health_icon.dds"))
, m_health(this, 0, 0, 100, 70, "digits_large")
, m_highlightColor(0.0f, 0.0f, 0.0f, 0.0f)
, m_highlightAnim(this, m_highlightColor.w, HighlightFadeSpeed)
, m_fadeAnim(this, m_highlightColor.w, ScreeFadeSpeed)
{
    m_health.setText("100");
    m_health.setColor(0.0f, 0.8f, 0.0f);
}

void Hud::reset()
{
    m_highlightColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_highlightAnim.stop();
    m_fadeAnim.stop();

    refresh();
}

void Hud::setHighlight(const vec3& color, float intensity)
{
    m_highlightColor.xyz = color;
    m_canvas.setFillColor(m_highlightColor);

    m_highlightAnim.run(intensity, 0.0f);

    refresh();
}

void Hud::fade()
{
    m_highlightColor.xyz = {};
    m_canvas.setFillColor(m_highlightColor);

    m_fadeAnim.run(0.0f, 1.0f);

    refresh();
}

void Hud::setHealth(uint16_t health)
{
    if (health > 50)
        m_health.setColor(0.0f, 0.8f, 0.0f);
    else if (health > 25)
        m_health.setColor(0.8f, 0.8f, 0.0f);
    else
        m_health.setColor(0.8f, 0.0f, 0.0f);

    m_health.setText(std::to_string(health).c_str());
}

void Hud::onResize()
{
    m_crosshair.move(m_width / 2, m_height / 2);
    
    m_healthIcon.move(0, m_height - 80);
    m_health.move(88, m_height - 65);
}

void Hud::display()
{
    if (m_highlightColor.w < math::eps) return;

    m_canvas.setFillColor(m_highlightColor);
    m_canvas.rectangle(0, 0, m_width, m_height);
}

} // namespace gamelogic