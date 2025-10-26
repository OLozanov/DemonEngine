#pragma once

#include "UI/Widget.h"
#include "Utils/EventHandler.h"

namespace GameLogic
{
namespace Widgets
{

class CheckBox : public UI::Widget
{
public:
	using OnChangeEvent = Event<void(bool)>;

public:
	CheckBox(UI::Widget* parent, uint16_t x, uint16_t y, bool checked = false);
	~CheckBox() {}

	bool isChecked() { return m_checked; }
	void setValue(bool checked) { m_checked = checked; }

	void onMouseButtonUp(int button, short x, short y) override;
	void onMouseOver() override;
	void onMouseLeave() override;

	OnChangeEvent OnChange;

private:
	void display() override;

	bool m_over;
	bool m_checked;

	static const vec2 BorderPoly[];
};

} // namespace Widgets
} // namespace GameLogic