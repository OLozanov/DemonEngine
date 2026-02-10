#pragma once

#include "UI/Widget.h"
#include "UI/Animation.h"
#include "Utils/LinkedList.h"
#include "Utils/EventHandler.h"

#include "UI/Render.h"
#include "UI/Font.h"

#include "System/Sync.h"

#include <map>
#include <thread>

namespace UI
{

class UiLayer
{
    UiLayer();
    ~UiLayer();
    UiLayer(UiLayer& ui) = delete;
    UiLayer& operator =(UiLayer& ui) = delete;

    UiRenderer& renderer() { return m_renderer; }

    void attach(Widget* widget) { m_widgets.append(widget); }
    void detach(Widget* widget) { m_widgets.remove(widget); }

    void addAnimation(Animation* anim) { m_animations.append(anim); }
    void removeAnimation(Animation* anim) { m_animations.remove(anim); }

    Widget* findWidget(LinkedList<Widget>& widgets, short& x, short& y);

    void displayWidgets(LinkedList<Widget>& widgets, int16_t x, int16_t y);

    void setFocus(Widget * widget) { m_focus = widget; }
    void killFocus(Widget* widget) { if (m_focus == widget) m_focus = nullptr; }

    void setCaretState(Widget* owner, short height, bool show);
    void setCaretPos(Widget* owner, short x, short y);
    //void setCaretColor(const vec4& color);

    void updateCaret(float dt);

public:
    static UiLayer& GetInstance();

    Font* getFont(const std::string& name);

    void init(int width, int height);
    void resize(int width, int height);

    void onCharInput(char chr);
    void onKeyPress(int key, bool keyDown);
    void onMouseMove(int x, int y);
    void onMouseButton(int button, bool keyDown);
    
    void onMouseWheel(short delta);

    void setCursor(Image* cursor);
    void showCursor(bool show) { m_cursorVisible = show; };
    void setCursorPos(short x, short y) { m_cx = x; m_cy = y; }

    void update(float dt);
    void display();

    Notification<void(short, short)> OnCursorMoved;
    Notification<void(int, bool)> OnMouseClick;
    Notification<void(Widget*)> OnWidgetClicked;
    //Notification<void()> OnInput;

private:
    void updateWidgets();
    void updateLoop();

private:
    std::thread m_updateThread;

    Signal m_updateStartEvent;
    Signal m_updateEndEvent;

    int m_width;
    int m_height;
    
    UiRenderer m_renderer;
    LinkedList<Widget> m_widgets;
    LinkedList<Animation> m_animations;

    std::map<std::string, Font*> m_fonts;

    Widget * m_focus; 	    //Keyboard input focus
    Widget * m_overWidget;  //Widget under cursor

    //Caret
    Widget* m_carOwner;
    short m_carX;
    short m_carY;
    short m_carH;
    float m_carTime;
    bool m_carVisible;
    static constexpr float CarInterval = 0.5;   //half of second

    //Mouse cursor
    ResourcePtr<Image> m_cursor;
    bool m_cursorVisible;
    short m_cx;
    short m_cy;

    friend void Widget::detachFromUI();
    friend void Widget::attachToUI(Widget*);
    friend void Widget::setFocus();
    friend void Widget::killFocus();
    friend void Widget::showCaret(short x, short y, short height);
    friend void Widget::setCaretPos(short x, short y);
    friend void Widget::hideCaret();
    friend Canvas::Canvas();
    friend Animation::Animation(Widget*, bool);
    friend Animation::~Animation();
};

} //namespace ui