#include "ComboBox.h"
#include "Game/UI/Widgets/Style.h"
#include "UI/UiLayer.h"

#include <cmath>

namespace GameLogic
{
namespace Widgets
{

ComboDropDown::ComboDropDown()
: UI::Widget(nullptr, 0, 0, 20, 100, flag_visible | flag_enabled | flag_clip)
, m_highlight(-1)
, m_scrollAnimation(this, this, &ComboDropDown::onScrollUpdate)
, m_dropDownAnimation(nullptr, this, &ComboDropDown::onDropDown)
{
    m_canvas.setLineColor(Style::WidgetLineColor);
    m_canvas.setFillColor(Style::WidgetBgColor2);

    m_canvas.setFont(Style::CaptionFont);

    m_canvas.setLineSize(0.5f);
    m_canvas.setSmoothEdgeSize(1.0f);
}

void ComboDropDown::onResize()
{
    m_polygon = { {float(m_width), 0.0f },
                  {float(m_width), float(m_height) - 3.0f},
                  {float(m_width) - 3.0f, float(m_height) },
                  {3.0f, float(m_height) },
                  {0.0f, float(m_height) - 3.0f },
                  {0.0f, 0.0f} };

    m_bottomPoly = { {float(m_width) * 0.5f - 20.0f, float(m_height) - 3.0f},
                     {float(m_width) * 0.5f + 20.0f, float(m_height) - 3.0f}, 
                     {float(m_width) * 0.5f + 23.0f, float(m_height)}, 
                     {float(m_width) * 0.5f - 23.0f, float(m_height)} };
}

void ComboDropDown::onMouseButtonUp(int button, short x, short y)
{
    if (m_dropDownAnimation.isRunning()) return;

    if (button == mb_left)
    {
        m_owner->selectItem(m_highlight);
        Hide();
    }
}

void ComboDropDown::onMouseMove(short x, short y)
{
    if (m_dropDownAnimation.isRunning()) return;

    size_t item = (y + short(m_offset) - 2) / 18;
    
    if (m_highlight != item)
    {
        m_highlight = item;
        refresh();
    }
}

void ComboDropDown::onMouseWheel(short delta)
{
    if (m_dropDownAnimation.isRunning()) return;

    m_scrollSpeed += delta * ScrollAcceleration;
    m_scrollSpeed = std::min(MaxScrollSpeed, std::max(-MaxScrollSpeed, m_scrollSpeed));

    m_scrollAnimation.run();
}

void ComboDropDown::onKeyDn(int key)
{
    if (key == 0x1B) Hide();
}

void ComboDropDown::onWidgetClick(Widget* widget)
{
    if (widget != this && widget != m_owner) Hide();
}

void ComboDropDown::onScrollUpdate(float dt)
{
    m_offset -= m_scrollSpeed * dt;

    if (m_offset <= 0.0f)
    {
        m_offset = 0.0f;
        m_scrollSpeed = 0.0f;
        m_scrollAnimation.stop();
    }

    if (m_offset >= m_length)
    {
        m_offset = m_length;
        m_scrollSpeed = 0.0f;
        m_scrollAnimation.stop();
    }

    m_scrollSpeed *= ScrollDrag;
    if (fabs(m_scrollSpeed) < 0.1f) m_scrollAnimation.stop();
}

void ComboDropDown::onDropDown(float dt)
{
    m_vpos += DropDownSpeed * dt;

    if (m_vpos >= 0.0f)
    {
        m_vpos = 0.0f;
        m_dropDownAnimation.stop();

        refresh();
    }

    m_canvas.setTranslation(0.0f, m_vpos);
}

void ComboDropDown::display()
{
    m_canvas.setFillColor(Style::WidgetBgColor2);
    m_canvas.polygon(m_polygon);

    if (!m_dropDownAnimation.isRunning())
    {
        size_t first = (short(m_offset) - 2) / 18;
        size_t last = first + std::ceilf((m_height - 2) / 18.0f);
        last = std::min(m_owner->m_items.size() - 1, last);

        int16_t y = -short(m_offset) + first * 18;

        for (size_t i = first; i <= last; i++)
        {
            const std::string& item = m_owner->m_items[i];

            int16_t len = m_canvas.getFont()->textWidth(item);
            int16_t x = (m_width - len) / 2;

            if (i == m_highlight || i == m_owner->m_index)
                m_canvas.setTextColor(Style::WidgetTextColorHighlight);
            else
                m_canvas.setTextColor(Style::WidgetTextColor);

            m_canvas.text(x, y, item);

            y += 18;

            if (y > m_height) break;
        }
    }

    m_canvas.setFillColor(Style::WidgetLineColor);

    m_canvas.line(m_polygon);
    m_canvas.polygon(m_bottomPoly);
}

ComboDropDown& ComboDropDown::GetInstance()
{
    static ComboDropDown DropDownList;

    return DropDownList;
}

void ComboDropDown::Show(ComboBox* owner, int16_t x, int16_t y, uint16_t width, uint16_t height)
{
    ComboDropDown& DropDownList = GetInstance();

    DropDownList.resize(width, height);
    DropDownList.move(x, y);
    DropDownList.show();

    DropDownList.m_owner = owner;
    DropDownList.m_length = owner->m_items.size() * 18 - height + 4;

    DropDownList.m_scrollSpeed = 0.0f;
    DropDownList.m_offset = float(owner->m_index) * 18.0f + 10.0f - DropDownList.m_height * 0.5f;
    DropDownList.m_offset = std::max(0.0f, std::min(DropDownList.m_length, DropDownList.m_offset));

    DropDownList.m_vpos = -float(DropDownList.m_height);
    DropDownList.m_dropDownAnimation.run();

    DropDownList.m_clickSubscription = UI::UiLayer::GetInstance().OnWidgetClicked.subscribe(&DropDownList, &ComboDropDown::onWidgetClick);
    DropDownList.setFocus();

    DropDownList.refresh();
}

void ComboDropDown::Hide()
{
    ComboDropDown& DropDownList = GetInstance();

    DropDownList.hide();

    DropDownList.m_dropDownAnimation.stop();
    DropDownList.m_scrollAnimation.stop();

    UI::UiLayer::GetInstance().OnWidgetClicked.unsubscribe(DropDownList.m_clickSubscription);
    DropDownList.killFocus();

    if (DropDownList.m_owner) DropDownList.m_owner->onCloseDropDown();
}

ComboBox::ComboBox(UI::Widget* parent, uint16_t x, uint16_t y, uint16_t width, Alignment alignment)
: UI::Widget(parent, x, y, width, 14, flag_visible | flag_enabled, alignment)
, m_over(false)
, m_dropdown(false)
, m_index(0)
{
    m_polygon = { {float(m_left) + 5.0f, float(m_top)},
                  {float(m_right) - 5.0f, float(m_top)},
        
                  {float(m_right), float(m_top) + 5.0f},
                  {float(m_right), float(m_top) + 9.0f},

                  {float(m_right) - 5.0f, float(m_bottom)},
                  {float(m_left) + 5.0f, float(m_bottom)},

                  {float(m_left), m_top + 9.0f},
                  {float(m_left), m_top + 5.0f} };

    m_canvas.setFont(Style::CaptionFont);

    m_canvas.setLineColor(Style::WidgetLineColor);
    m_canvas.setFillColor(Style::WidgetBgColor);
    m_canvas.setTextColor(Style::WidgetTextColor);

    m_canvas.setLineSize(0.5f);
    m_canvas.setSmoothEdgeSize(1.0f);
}

void ComboBox::addItem(const std::string& item)
{
    m_items.push_back(item);
}

void ComboBox::selectItem(size_t index)
{
    m_index = index;
    OnChange(index);

    refresh();
}

void ComboBox::onMouseButtonUp(int button, short x, short y)
{
    if (button == mb_left)
    {
        int16_t screenx, screeny;
        
        if (!m_dropdown)
        {
            screenPos(screenx, screeny);
            ComboDropDown::Show(this, screenx + m_left + 5, screeny + m_bottom, m_width - 10, 100);

            m_dropdown = true;
        }
        else
            ComboDropDown::Hide();
    }
}

void ComboBox::onMouseOver()
{
    if (!isEnabled()) return;

    m_over = true;
    refresh();
}

void ComboBox::onMouseLeave()
{
    if (!isEnabled()) return;

    m_over = false;
    refresh();
}

void ComboBox::onCloseDropDown()
{
    m_dropdown = false;
    refresh();
}

void ComboBox::display()
{
    if (m_over || m_dropdown)
        m_canvas.setTextColor(Style::WidgetTextColorHighlight);
    else
        m_canvas.setTextColor(Style::WidgetTextColor);

    if (m_over || m_dropdown)
        m_canvas.setFillColor(Style::WidgetBgColorHighlight);
    else
        m_canvas.setFillColor(Style::WidgetBgColor);

    m_canvas.polygon(m_polygon, true);

    if (m_index < m_items.size())
    {
        const std::string& item = m_items[m_index];

        int16_t len = m_canvas.getFont()->textWidth(item);
        int16_t xpos = m_left + (m_width - len) / 2;
    
        m_canvas.text(xpos, m_top - 4, item);
    }
}

} // namespace Widgets
} // namespace GameLogic