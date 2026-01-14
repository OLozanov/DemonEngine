#pragma once

#include "UI/Widget.h"
#include "Utils/EventHandler.h"

#include "Game/UI/Widgets/Label.h"
#include "Game/UI/Widgets/Button.h"
#include "Game/UI/Widgets/CheckBox.h"
#include "Game/UI/Widgets/ComboBox.h"
#include "Game/UI/Widgets/Slider.h"

#include "Render/Render.h"

namespace GameLogic
{

struct Settings
{
    size_t resolution = 7;
    bool fullscreen = false;
    bool vsync = false;

    bool gi = true;

    float volume = 0.5f;
};

class VideoPanel : public UI::Widget
{
public:
    VideoPanel(UI::Widget* parent, Settings& settings, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

    void update();
    void updateFullscreenStatus();

private:
    Settings& m_settings;

    Widgets::Label m_resLabel;
    Widgets::Label m_windowedLabel;
    Widgets::Label m_vsyncLabel;
    Widgets::Label m_giLabel;

    Widgets::ComboBox m_resolutionCombo;
    Widgets::CheckBox m_windowedChbox;
    Widgets::CheckBox m_vsyncChbox;
    
    Widgets::CheckBox m_giChbox;

    //void display() { m_canvas.rectangle(m_left, m_top, m_right, m_bottom); }
};

class AudioPanel : public UI::Widget
{
public:
    AudioPanel(UI::Widget* parent, Settings& settings, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

    void update();

private:
    Settings& m_settings;

    Widgets::Label m_volLabel;
    Widgets::Slider m_volSlider;
};

class SettingsPanel : public UI::Widget
{
public:
    SettingsPanel(UI::Widget* parent, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

    const Settings& settings() const { return m_settings; }
    void setSettings(const Settings& settings);
    void setFullscreenStatus(bool fullscreen);

    void display() override;

private:
    Settings m_settings;

    std::vector<vec2> m_polygon;

    Widgets::Button m_videoButton;    
    Widgets::Button m_audioButton;
    Widgets::Button m_controlsButton;

    VideoPanel m_videoPanel;
    AudioPanel m_audioPanel;
};

class SaveLoadPanel : public UI::Widget
{
public:
    SaveLoadPanel(UI::Widget* parent, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

    void display() override;
private:

};

class Menu : public UI::Widget
{
public:
    Menu();

    bool escape();

    Widgets::Button::OnClickEvent& OnNewGameEvent() { return m_newButton.OnClick; }
    Widgets::Button::OnClickEvent& OnSaveEvent() { return m_saveButton.OnClick; }
    Widgets::Button::OnClickEvent& OnLoadEvent() { return m_loadButton.OnClick; }
    Widgets::Button::OnClickEvent& OnExitEvent() { return m_exitButton.OnClick; }

    const Settings& settings() const { return m_settingsPanel.settings(); }
    void setSettings(const Settings& settings) { m_settingsPanel.setSettings(settings); }
    void setFullscreenStatus(bool fullscreen) { m_settingsPanel.setFullscreenStatus(fullscreen); }

    void display() override;

private:
    void showButtons();
    void hideButtons();

private:
    std::vector<vec2> m_plaquePolygon;

    Widgets::Button m_newButton;
    Widgets::Button m_saveButton;
    Widgets::Button m_loadButton;
    Widgets::Button m_settingsButton;
    Widgets::Button m_exitButton;

    SaveLoadPanel m_saveloadPanel;
    SettingsPanel m_settingsPanel;

    size_t m_cid;

    static constexpr uint16_t MenuWidth = 400;
    static constexpr uint16_t MenuHeight = 500;

    static constexpr uint16_t ButtonWidth = 200;
    static constexpr uint16_t ButtonHeight = 30;

    static const std::string Captions[];
};

} // namespace gamelogic