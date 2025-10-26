#pragma once

#include "UI/Widget.h"
#include "UI/Text.h"

#include "Utils/EventHandler.h"
#include "Utils/RingBuffer.h"

namespace GameLogic
{

class Console : public UI::Widget
{
    RingBuffer<std::string> m_lines;
    RingBuffer<std::string> m_commands;

    RingBuffer<std::string>::Iterator m_cmdPtr;

    UI::Text m_cmdText;
    std::vector<vec2> m_penta;

    void onResize() override;
    void onCharInput(char chr) override;
    void onKeyDn(int key) override;

    void updateCaretPos();

    void display();

public:
    Console();

    void print(const std::string& str);

    Event<void(const std::string&)> OnCommand;
};

} // namespace gamelogic