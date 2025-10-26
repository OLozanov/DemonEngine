#pragma once

#include "UI/Widget.h"
#include "Resources/Resources.h"

namespace GameLogic
{
namespace Widgets
{

class Label : public UI::Widget
{
public:

	Label(UI::Widget* parent, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const std::string& font, Alignment alignment = Alignment::TopLeft);
	~Label() {}

	void setColor(float r, float g, float b, float a = 1.0f);
	void setColor(const vec3& color);
	void setColor(const vec4& color);

	short setText(const char* text);

private:
	void display() override;

	std::string m_text;
};

class IconLabel : public UI::Widget
{
public:
	IconLabel(UI::Widget* parent, uint16_t x, uint16_t y, uint16_t width, uint16_t height, Image* img, Alignment alignment = Alignment::TopLeft);
	IconLabel(UI::Widget* parent, uint16_t x, uint16_t y, Image* img, Alignment alignment = Alignment::TopLeft);
	~IconLabel() {}

	void setColor(float r, float b, float g, float a = 1.0f);
	void setColor(const vec3& color);
	void setColor(const vec4& color);

	void setImage(Image* image) { m_image.reset(image); }

private:
	void display() override;

	ImagePtr m_image;
};

} // namespace Widgets
} // namespace GameLogic