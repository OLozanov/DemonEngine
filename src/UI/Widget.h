#pragma once

#include "Utils/LinkedList.h"
#include <stdint.h>

#include "UI/Canvas.h"

namespace UI
{

class UiLayer;

class Widget : public ListNode<Widget>
{
public:

    enum MouseButton
    {
        mb_left = 0,
        mb_right = 1,
        mb_middle = 2
    };

    enum Flag
    {
        flag_visible = 1,
        flag_child = 2,
        flag_enabled = 4,
        flag_border = 8,
        flag_clip = 16
    };

    enum class Alignment
    {
        TopLeft,
        Left,
        Right,
        Top,
        Bottom,
        Center
    };

private:
    friend class UiLayer;

    Widget* m_parent;
    LinkedList<Widget> m_children;

    std::vector<vec2> m_border;

    void calculateExtent();

    void attachToUI(Widget* parent);
    void detachFromUI();
    void addChild(Widget* child);
    void removeChild(Widget* child);

    bool testBorder(int16_t& x, int16_t& y);

protected:
    uint32_t m_flags;

    Alignment m_alignment;

    int16_t m_x;
    int16_t m_y;

    uint16_t m_width;
    uint16_t m_height;

    int16_t m_left;
    int16_t m_top;
    int16_t m_right;
    int16_t m_bottom;

    Canvas m_canvas;

    bool m_redraw;

    void setBorder(const std::vector<vec2>& poly) { m_border = poly; }

    virtual void display();

    static bool InsidePolygon(const std::vector<vec2>& poly, const vec2& pt);

public:
    Widget();
    Widget(Widget* parent, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t flags = flag_visible, Alignment alignment = Alignment::TopLeft);
    
    virtual ~Widget();

    uint16_t width() { return m_width; }
    uint16_t height() { return m_height; }

    int16_t xcoord() { return m_x; }
    int16_t ycoord() { return m_y; }

    void screenPos(int16_t& x, int16_t& y);

    void show();
    void hide();

    bool isVisible();

    void enable();
    void disable();

    bool isEnabled();

    void setFocus();
    void killFocus();

    void showCaret(short x, short y, short height);
    void setCaretPos(short x, short y);
    void hideCaret();

    void move(int16_t x, int16_t y);
    void resize(uint16_t width, uint16_t height);

    void refresh() { m_redraw = true; }

    virtual void onResize() {}

    virtual void onCharInput(char chr) {}
    virtual void onKeyDn(int key) {}
    virtual void onKeyUp(int key) {}

    virtual void onMouseMove(short x, short y) {}
    virtual void onMouseOver() {}
    virtual void onMouseLeave() {}

    virtual void onMouseWheel(short delta) {}

    virtual void onMouseButtonDown(int button, short x, short y) {}
    virtual void onMouseButtonUp(int button, short x, short y) {}
    virtual void onMouseDoubleClick(int button) {}
};

} //namespace UI