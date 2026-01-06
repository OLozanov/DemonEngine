#pragma once

#include "math/math3d.h"

#include <string>

namespace GameLogic
{
namespace Widgets
{

class Style
{
public:
    static constexpr vec4 WidgetColor = { 0.0f, 0.48f, 0.69f, 1.0f };

    static constexpr vec4 WidgetTextColor = { 0.9f, 0.9f, 0.9f, 1.0f };
    static constexpr vec4 WidgetTextColorHighlight = { 0.0f, 0.2f, 0.5f, 1.0f };

    static constexpr vec4 WidgetLineColor = { 0.0f, 0.2f, 0.5f, 1.0f };

    static constexpr vec4 WidgetBgColor = { 0.0f, 0.6f, 0.8f, 1.0f };
    static constexpr vec4 WidgetBgColor2 = { 0.0f, 0.6f, 0.8f, 0.7f };
    static constexpr vec4 WidgetBgColorHighlight = { 1.0f, 0.99f, 0.79f, 1.0f };

    static const std::string CaptionFont;
    static const std::string CapitalFont;
};

} // namespace Widgets
} // namespace GameLogic