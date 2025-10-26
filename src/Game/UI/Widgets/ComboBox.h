#pragma once

#include "UI/Widget.h"
#include "UI/Animation.h"
#include "Utils/EventHandler.h"

namespace GameLogic
{
namespace Widgets
{

class ComboBox;

class ComboDropDown : public UI::Widget
{
private:
	ComboDropDown();
	~ComboDropDown() {}

	void onResize() override;
	void onMouseButtonUp(int button, short x, short y) override;
	void onMouseMove(short x, short y) override;
	void onMouseWheel(short delta) override;
	void onKeyDn(int key) override;

	void onWidgetClick(Widget* widget);

	void onScrollUpdate(float dt);
	void onDropDown(float dt);

	void display() override;

	static ComboDropDown& GetInstance();

public:
	static void Show(ComboBox* ownder, int16_t x, int16_t y, uint16_t width, uint16_t height);
	static void Hide();

private:
	ComboBox* m_owner;

	SubscriptionHandle m_clickSubscription;

	UI::CustomAnimation m_scrollAnimation;
	UI::CustomAnimation m_dropDownAnimation;
	
	float m_vpos;
	float m_offset;
	float m_scrollSpeed;
	float m_length;
	size_t m_highlight;

	std::vector<vec2> m_polygon;
	std::vector<vec2> m_bottomPoly;

	static constexpr float DropDownSpeed = 500.0f;

	// Scroll Animation
	static constexpr float ScrollAcceleration = 0.5f;
	static constexpr float MaxScrollSpeed = 300.0f;
	static constexpr float ScrollDrag = 0.95f;
};

class ComboBox : public UI::Widget
{
public:
	using OnChangeEvent = Event<void(size_t)>;

public:
	ComboBox(UI::Widget* parent, uint16_t x, uint16_t y, uint16_t width, Alignment alignment = Alignment::Center);
	~ComboBox() {}

	void addItem(const std::string& item);
	size_t selection() { return m_index; }
	void selectItem(size_t index);

	void onMouseButtonUp(int button, short x, short y) override;
	void onMouseOver() override;
	void onMouseLeave() override;

	OnChangeEvent OnChange;

private:
	void onCloseDropDown();
	void display() override;

private:
	bool m_over;
	bool m_dropdown;

	size_t m_index;

	std::vector<std::string> m_items;

	std::vector<vec2> m_polygon;

	friend class ComboDropDown;
};

} // namespace Widgets
} // namespace GameLogic