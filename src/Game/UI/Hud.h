#pragma once

#include "UI/Animation.h"
#include "Game/UI/Widgets/Label.h"

namespace GameLogic
{

class Hud : public UI::Widget
{
public:
    Hud();

    void reset();

    void setHighlight(const vec3& color, float intensity);
    void fade();

    void setHealth(uint16_t health);

    void onResize() override;
    void display() override;

    auto& onScreenFaded() { return m_fadeAnim.OnFinished; }

private:
    Widgets::IconLabel m_crosshair;
    
    Widgets::IconLabel m_healthIcon;
    Widgets::Label m_health;

    vec4 m_highlightColor;

    UI::FadeOutAnimation m_highlightAnim;
    UI::FadeInAnimation m_fadeAnim;

    static constexpr float HighlightFadeSpeed = 0.3f;
    static constexpr float ScreeFadeSpeed = 0.8f;
};

} // namespace gamelogic