#include "Menu.h"
#include "Render/SceneManager.h"
#include "System/Win32App.h"
#include "System/AudioManager.h"

#include "Game/UI/Widgets/Style.h"

#include <sstream>

namespace GameLogic
{

const std::string Menu::Captions[] = { "Save Game", "Load Game", "Settings" };

VideoPanel::VideoPanel(UI::Widget* parent, Settings& settings, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
: UI::Widget(parent, x, y, width, height, 0)
, m_settings(settings)
, m_resLabel(this, 100, 27, 50, 20, UI::Widget::Alignment::Right)
, m_windowedLabel(this, 100, 57, 50, 20, UI::Widget::Alignment::Right)
, m_giLabel(this, 100, 87, 50, 20, UI::Widget::Alignment::Right)
, m_resolutionCombo(this, 112, 30, 100, UI::Widget::Alignment::Left)
, m_windowedChbox(this, 120, 60)
, m_giChbox(this, 120, 90)
{
    Render::GpuInstance& renderApi = Render::GpuInstance::GetInstance();

    m_resLabel.setText("Resolution:");
    m_windowedLabel.setText("Windowed:");
    m_giLabel.setText("GI:");

    m_resLabel.setColor(1.0f, 1.0f, 1.0f);
    m_windowedLabel.setColor(1.0f, 1.0f, 1.0f);
    m_giLabel.setColor(1.0f, 1.0f, 1.0f);

    std::stringstream sstream;

    for (const auto& mode : App::DisplayModeList())
    {
        sstream << mode.first << "x" << mode.second;
        m_resolutionCombo.addItem(sstream.str());

        sstream.str({});
    }

    m_resolutionCombo.selectItem(7);
    m_windowedChbox.setValue(true);

    if (!renderApi.rtxSupport())
    {
        m_giChbox.disable();
        m_giChbox.setValue(false);
    }
    else
        m_giChbox.setValue(true);

    m_resolutionCombo.OnChange.bind([this](size_t index) {

        App::Resize(index);
        m_settings.resolution = index;
    });

    m_windowedChbox.OnChange.bind([this](bool enable) {
        App::ToggleFullscreen();
    });

    m_giChbox.OnChange.bind([this](bool enable) {
        Render::SceneManager::GetInstance().enableGI(enable);

        m_settings.gi = enable;
    });
}

void VideoPanel::update()
{
    m_resolutionCombo.selectItem(m_settings.resolution);
    m_windowedChbox.setValue(!m_settings.fullscreen);
    m_giChbox.setValue(m_settings.gi);

    refresh();
}

void VideoPanel::updateFullscreenStatus()
{
    m_windowedChbox.setValue(!m_settings.fullscreen);
    refresh();
}

AudioPanel::AudioPanel(UI::Widget* parent, Settings& settings, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
: UI::Widget(parent, x, y, width, height, 0)
, m_settings(settings)
, m_volLabel(this, 120, 27, 50, 20, UI::Widget::Alignment::Right)
, m_volSlider(this, 130, 30, 250, UI::Widget::Alignment::Left)
{
    m_volLabel.setText("Sound volume:");
    m_volLabel.setColor(1.0f, 1.0f, 1.0f);

    m_volSlider.OnChange.bind([this]() {

        float volume = m_volSlider.position();

        AudioManager::GetInstance().setVolume(volume);
        m_settings.volume = volume;
    });
}

void AudioPanel::update()
{
    m_volSlider.setPosition(m_settings.volume);

    refresh();
}

SettingsPanel::SettingsPanel(UI::Widget* parent, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
: UI::Widget(parent, x, y, width, height, 0)
, m_videoButton(this, m_width / 3.0 * 0.5, m_bottom - 30, 120, 25, "Video")
, m_audioButton(this, m_width / 3.0 * 1.5, m_bottom - 25, 120, 25, "Audio")
, m_controlsButton(this, m_width / 3.0 * 2.5, m_bottom - 25, 120, 25, "Controls")
, m_videoPanel(this, m_settings, 0, 50, m_width, m_height - 100)
, m_audioPanel(this, m_settings, 0, 50, m_width, m_height - 100)
{
    m_polygon = { { float(m_left), float(m_top) },
                  { float(m_right) - width * 0.05f, float(m_top) },
                  { float(m_right), float(m_top) + height * 0.05f },
                  { float(m_right), float(m_bottom) },
                  { float(m_left) + width * 0.05f, float(m_bottom) },
                  { float(m_left), float(m_bottom) - height * 0.05f } };

    //setBorder(m_polygon);

    m_canvas.setFillColor(0.0f, 0.48f, 0.69f, 0.9f);

    m_videoButton.enable();
    m_audioButton.enable();
    m_controlsButton.enable();

    m_videoPanel.show();

    m_videoButton.OnClick.bind([this]() {
        m_videoButton.move(m_width / 3.0 * 0.5, m_bottom - 30);
        m_audioButton.move(m_width / 3.0 * 1.5, m_bottom - 25);
        m_controlsButton.move(m_width / 3.0 * 2.5, m_bottom - 25);

        m_videoPanel.show();
        m_audioPanel.hide();
    });

    m_audioButton.OnClick.bind([this]() {
        m_videoButton.move(m_width / 3.0 * 0.5, m_bottom - 25);
        m_audioButton.move(m_width / 3.0 * 1.5, m_bottom - 30);
        m_controlsButton.move(m_width / 3.0 * 2.5, m_bottom - 25);

        m_videoPanel.hide();
        m_audioPanel.show();
    });

    m_controlsButton.OnClick.bind([this]() {
        m_videoButton.move(m_width / 3.0 * 0.5, m_bottom - 25);
        m_audioButton.move(m_width / 3.0 * 1.5, m_bottom - 25);
        m_controlsButton.move(m_width / 3.0 * 2.5, m_bottom - 30);

        m_videoPanel.hide();
        m_audioPanel.hide();
    });
}

void SettingsPanel::setSettings(const Settings& settings) 
{ 
    m_settings = settings;

    m_videoPanel.update();
    m_audioPanel.update();
}

void SettingsPanel::setFullscreenStatus(bool fullscreen)
{
    m_settings.fullscreen = fullscreen;
    m_videoPanel.updateFullscreenStatus();
}

void SettingsPanel::display()
{
    m_canvas.rectangle(m_left, m_top + 50, m_right, m_bottom - 50);
    //m_canvas.polygon(m_polygon);
}

SaveLoadPanel::SaveLoadPanel(UI::Widget* parent, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
: UI::Widget(parent, x, y, width, height, 0)
{

}

void SaveLoadPanel::display()
{
    short top = m_top + 50;

    for (size_t i = 0; i < 8; i++)
    {
        m_canvas.setFillColor(0.0f, 0.48f, 0.69f, 0.9f);
        m_canvas.rectangle(m_left, top, m_right, top + 20);

        m_canvas.setFillColor(0.0f, 0.58f, 0.69f, 0.9f);
        m_canvas.rectangle(m_left, top + 20, m_right, top + 40);

        top += 40;
    }
}

Menu::Menu()
: UI::Widget(nullptr, 0, 0, MenuWidth, MenuHeight, flag_visible)
, m_newButton(this, MenuWidth / 2, 140, ButtonWidth, ButtonHeight, "New Game")
, m_saveButton(this, MenuWidth / 2, 180, ButtonWidth, ButtonHeight, "Save Game")
, m_loadButton(this, MenuWidth / 2, 220, ButtonWidth, ButtonHeight, "Load Game")
, m_settingsButton(this, MenuWidth / 2, 260, ButtonWidth, ButtonHeight, "Settings")
, m_exitButton(this, MenuWidth / 2, 440, ButtonWidth, ButtonHeight, "Exit")
, m_saveloadPanel(this, -MenuWidth * 0.1f, 0, MenuWidth * 1.2f, MenuHeight)
, m_settingsPanel(this, -MenuWidth * 0.1f, 0, MenuWidth * 1.2f, MenuHeight)
, m_cid(-1)
{
    int16_t left = MenuWidth / 2 - 70;

    m_plaquePolygon = { { float(left), float(m_top) },
                        { float(left) + 130, float(m_top) },
                        { float(left) + 140, float(m_top) + 17 },
                        { float(left) + 140, float(m_top) + 25 },
                        { float(left) + 10, float(m_top) + 25 }, 
                        { float(left), float(m_top) + 8 } };

    m_canvas.setFont("Audiowide");
    m_canvas.setTextColor(Widgets::Style::WidgetTextColor);

    m_newButton.enable();
    m_loadButton.enable();
    m_settingsButton.enable();
    m_exitButton.enable();

    m_loadButton.OnClick.bind([this]() {
        hideButtons();
        m_saveloadPanel.show();

        m_cid = 1;

        refresh();
    });

    m_settingsButton.OnClick.bind([this]() {
        hideButtons();
        m_settingsPanel.show();

        m_cid = 2;

        refresh();
    });
}

bool Menu::escape()
{
    if (m_saveloadPanel.isVisible())
    {
        m_saveloadPanel.hide();
        showButtons();

        m_cid = -1;

        return false;
    }

    if (m_settingsPanel.isVisible())
    {
        m_settingsPanel.hide();
        showButtons();

        m_cid = -1;

        return false;
    }

    return true;
}

void Menu::showButtons()
{
    m_newButton.show();
    m_saveButton.show();
    m_loadButton.show();
    m_settingsButton.show();
    m_exitButton.show();

    refresh();
}

void Menu::hideButtons()
{
    m_newButton.hide();
    m_saveButton.hide();
    m_loadButton.hide();
    m_settingsButton.hide();
    m_exitButton.hide();
}

void Menu::display()
{
    m_canvas.setFillColor(0.0f, 0.48f, 0.69f, 0.9f);

    if (m_cid != -1)
    {
        const std::string& caption = Captions[m_cid];

        m_canvas.polygon(m_plaquePolygon);

        short x = MenuWidth / 2 - m_canvas.getFont()->textWidth(caption) / 2;
        short y = m_top;

        m_canvas.text(x, y, caption);
    }
}

} // namespace gamelogic