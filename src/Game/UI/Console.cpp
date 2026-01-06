#include "Console.h"
#include "System/Keys.h"

namespace GameLogic
{

Console::Console()
: Widget(nullptr, 0, 0, 400, 200, 0)
, m_lines(m_height/18 - 1)
, m_commands(6)
, m_cmdPtr(m_commands.end())
, m_cmdText(5, m_height - 18, m_width)
{
	m_canvas.setFont("Quantico");
	m_canvas.setLineJointType(UI::JointType::Round);

	m_cmdText.setFont(m_canvas.getFont());
	m_penta.resize(5);
}

void Console::onResize()
{
	float ang = 0;

	for (int a = 0; a < 5; a++)
	{
		m_penta[a] = { sinf(ang) * 120 + m_width/2.0f, cosf(ang) * 120 + m_height/2.0f };

		ang += 2 * math::pi / 5 * 2;
	}

	m_lines.resize(m_height / 18);

	showCaret(5, m_height - 20, 16);

	refresh();
}

void Console::onCharInput(char chr)
{
	m_cmdText.input(chr);
	updateCaretPos();

	refresh();
}

void Console::onKeyDn(int key)
{
	if (key == KeyReturn)
	{
		m_commands.push(m_cmdText.text());
		OnCommand(m_cmdText.text());

		m_cmdText.clear();
		refresh();
	
		m_cmdPtr = m_commands.end();

		updateCaretPos();

		return;
	} 
	
	if (key == KeyUp)
	{
		if (m_cmdPtr == m_commands.begin()) return;
		m_cmdPtr--;
		m_cmdText.setText(*m_cmdPtr);

		updateCaretPos();

		refresh();

		return;
	}

	if (key == KeyDown)
	{
		if (m_cmdPtr.next() == m_commands.end()) return;
		m_cmdPtr++;
		m_cmdText.setText(*m_cmdPtr);

		updateCaretPos();

		refresh();
	
		return;
	}

	m_cmdText.onKeyPress(key);
	updateCaretPos();

	refresh();
}

void Console::updateCaretPos()
{
	setCaretPos(m_cmdText.caretPos(), m_height - 20);
}

void Console::print(const std::string& str)
{
	m_lines.push(str);
}

void Console::display()
{
    m_canvas.setFillColor(0.4, 0.4, 0.4);
    m_canvas.rectangle(0, 0, m_width, m_height);

	m_canvas.setFillColor(0.15, 0.15, 0.15);
	m_canvas.rectangle(0, m_height-2, m_width, m_height);

	m_canvas.setLineSize(20);
	m_canvas.setLineColor(1.0, 0.0, 0.0);
	m_canvas.line(m_penta, true);

	m_canvas.setTextColor(0, 0, 0);

	m_canvas.text(2, m_height-20, m_cmdText.text());

	m_canvas.setTextColor(0, 0.9, 0);

	size_t lnum = m_lines.num();
	short ly = m_height - 18 * (lnum + 1);

	for(const auto& line : m_lines)
	{
		m_canvas.text(2, ly, line);
		ly += 18;
	}
}

} // namespace gamelogic