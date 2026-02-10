#include "UI/UiLayer.h"

#include <iostream>

namespace UI
{

UiLayer& UiLayer::GetInstance()
{
    static UiLayer instance;

    return instance;
}

UiLayer::UiLayer()
: m_updateThread(&UiLayer::updateLoop, this)
, m_focus(nullptr)
, m_cursorVisible(false)
{
}

UiLayer::~UiLayer()
{
    m_updateThread.detach();
}

Font* UiLayer::getFont(const std::string& name)
{
    auto it = m_fonts.find(name);
    if (it != m_fonts.end()) return it->second;

    Font* font = nullptr;

    try
    {
        font = new Font(name);
    }
    catch (...)
    {
        std::cout << "Fail to load font " << name << std::endl;
        font = new Font();
    }

    m_fonts[name] = font;
    return font;
}

void UiLayer::init(int width, int height)
{
    m_width = width;
    m_height = height;

    m_cx = width / 2;
    m_cy = height / 2;

    m_renderer.init();
    m_renderer.setScreenSize(width, height);
}

void UiLayer::resize(int width, int height)
{
    m_width = width;
    m_height = height;

    m_cx = width / 2;
    m_cy = height / 2;

    m_renderer.setScreenSize(width, height);
}

Widget* UiLayer::findWidget(LinkedList<Widget>& widgets, short& x, short& y)
{
    for (Widget* widget : widgets.reverse())
    {
        if (widget->m_flags & Widget::flag_visible)
        {           
            if (widget->testBorder(x, y))
            {
                x -= widget->m_x + widget->m_left;
                y -= widget->m_y + widget->m_top;

                Widget* childWidget = findWidget(widget->m_children, x, y);
                if (childWidget) return childWidget;

                return widget;
            }
        }
    }

    return nullptr;
}

void UiLayer::onCharInput(char chr)
{
    if (!m_focus) return;
    if (!(m_focus->m_flags & Widget::flag_visible)) return;

    m_focus->onCharInput(chr);
}

void UiLayer::onKeyPress(int key, bool keyDown)
{
    if (!m_focus) return;
    if (!(m_focus->m_flags & Widget::flag_visible)) return;

    if (keyDown) m_focus->onKeyDn(key);
    else m_focus->onKeyUp(key);
}

void UiLayer::onMouseMove(int x, int y)
{
    m_cx += x;
    m_cy += y;

    if (m_cx < 0) m_cx = 0;
    if (m_cx > m_width) m_cx = m_width;

    if (m_cy < 0) m_cy = 0;
    if (m_cy > m_height) m_cy = m_height;

    OnCursorMoved(m_cx, m_cy);

    /*if (dragwnd)
    {
        dragwnd->x = cx + ddx;
        dragwnd->y = cy + ddy;

        return;
    }*/

    short wx = m_cx;
    short wy = m_cy;

    Widget* old_overWidget = m_overWidget;

    Widget* receiver = findWidget(m_widgets, wx, wy);
    m_overWidget = receiver;

    if (receiver) receiver->onMouseMove(wx, wy);
    if (m_overWidget != old_overWidget)
    {
        if (m_overWidget) m_overWidget->onMouseOver();
        if (old_overWidget) old_overWidget->onMouseLeave();
    }
}

void UiLayer::onMouseButton(int button, bool down)
{
    OnMouseClick(button, down);

    short wx = m_cx;
    short wy = m_cy;

    Widget* receiver = findWidget(m_widgets, wx, wy);
    if (receiver && receiver->isEnabled())
    {
        if (down)
        {
            receiver->onMouseButtonDown(button, wx, wy);
            OnWidgetClicked(receiver);
        }
        else 
            receiver->onMouseButtonUp(button, wx, wy);
    }
    else
        OnWidgetClicked(nullptr);
}

void UiLayer::onMouseWheel(short delta)
{
    short wx = m_cx;
    short wy = m_cy;

    Widget* receiver = findWidget(m_widgets, wx, wy);
    if (receiver && receiver->isEnabled())
    {
        receiver->onMouseWheel(delta);
    }
}

void UiLayer::setCursor(Image* cursor)
{ 
    m_cursor = cursor;
    m_renderer.setCursorSize(cursor->height, cursor->width);
}

void UiLayer::displayWidgets(LinkedList<Widget>& widgets, int16_t x, int16_t y)
{
    bool clip = false;

    for (Widget* widget : widgets)
    {
        if (widget->m_flags & Widget::flag_visible)
        {
            if (widget->m_redraw)
            {
                widget->m_canvas.reset();
                widget->display();
            
                widget->m_redraw = false;
            }

            m_renderer.setDrawOffset({ (float)widget->m_x + widget->m_canvas.m_offset.x + x, 
                                       (float)widget->m_y + widget->m_canvas.m_offset.y + y });

            if (widget->m_flags & Widget::flag_clip)
            {
                m_renderer.setClipArea({ widget->m_x + widget->m_left + x - 1, widget->m_y + widget->m_top + y + 1,
                                         widget->m_x + widget->m_right + x + 1, widget->m_y + widget->m_bottom + y + 1 });
                clip = true;
            }
            else if (clip)
            {
                m_renderer.resetClipArea();
                clip = false;
            }

            const CommandList& drawList = widget->m_canvas.commandList();
            const VertexBuffer& buffer = widget->m_canvas.vertexBuffer();
            m_renderer.draw(buffer, drawList);

            if ((m_carOwner == widget) && m_carVisible)
            {
                m_renderer.setDrawOffset({ (float)widget->m_x + x + m_carX, (float)widget->m_y + y + m_carY });
                m_renderer.drawCaret();
            }

            displayWidgets(widget->m_children, widget->m_x + x, widget->m_y + y);
        }
    }

    if (clip) m_renderer.resetClipArea();
}

void UiLayer::setCaretState(Widget* owner, short height, bool show)
{
    if (show)
    {
        m_carOwner = owner;
        m_carH = height;
    
        m_renderer.setCaretSize(2, height);
        m_renderer.setCaretColor(vec4(0, 0, 0, 1));
    }
    else
    {
        if (m_carOwner == owner) m_carOwner = nullptr;
    }
}

void UiLayer::setCaretPos(Widget* owner, short x, short y)
{
    if (m_carOwner != owner) return;

    m_carX = x;
    m_carY = y;

    m_carTime = 0;
    m_carVisible = true;
}

void UiLayer::updateCaret(float dt)
{
    m_carTime += dt;

    if (m_carTime >= CarInterval)
    {
        m_carTime -= CarInterval;
        m_carVisible = !m_carVisible;
    }
}

void UiLayer::updateWidgets()
{
    m_renderer.beginDraw();

    displayWidgets(m_widgets, 0, 0);

    if (m_cursorVisible)
    {
        m_renderer.setDrawOffset({ static_cast<float>(m_cx), static_cast<float>(m_cy) });
        m_renderer.drawCursor(m_cursor->handle);
    }

    m_renderer.endDraw();
}

void UiLayer::updateLoop()
{
    while (true)
    {
        m_updateStartEvent.wait();
        updateWidgets();
        m_updateEndEvent.notify();
    }
}

void UiLayer::update(float dt)
{
    updateCaret(dt);

    for (Animation* anim : m_animations)
    {
        if (anim->m_active)
        {
            anim->update(dt);
            if (anim->m_widget) anim->m_widget->refresh();
        }
    }

    m_updateStartEvent.notify();
    //updateWidgets();
}

void UiLayer::display()
{
    m_updateEndEvent.wait();
    m_renderer.render();
}

} //namespace ui