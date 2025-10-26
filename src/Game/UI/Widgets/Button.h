#pragma once

#include "UI/Widget.h"
#include "Utils/EventHandler.h"

namespace GameLogic
{
namespace Widgets
{

class Button : public UI::Widget
{
public:
    using OnClickEvent = Event<void()>;

    Button(UI::Widget* parent, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const std::string& caption);

    void display() override;

    OnClickEvent OnClick;

private:
    void onMouseButtonUp(int button, short x, short y) override;
    void onMouseOver() override;
    void onMouseLeave() override;

private:

    std::vector<vec2> m_polygon;
    std::string m_caption;
    uint16_t m_captionWidth;
};

} // namespace Widgets
} // namespace GameLogic
