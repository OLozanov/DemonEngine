#pragma once

#include "Utils/AsyncCall.h"

#include "System/Timer.h"

#include "Physics/PhysicsManager.h"
#include "Render/SceneManager.h"
#include "Resources/Model.h"
#include "Render/StaticObject.h"

#include "Game/UI/Console.h"
#include "Game/UI/Widgets/Label.h"
#include "Game/UI/Hud.h"
#include "Game/UI/Menu.h"

#include "Game/Camera/CameraController.h"
#include "Game/Camera/FirstPersonCamera.h"
#include "Game/Camera/ThirdPersonCamera.h"

#include "GameObject.h"
#include "Game/StaticGameObject.h"
#include "Game/Trigger.h"
#include "Game/ClimbArea.h"

#include "Game/Core/Player.h"
#include "Game/Core/Vehicle.h"
#include "Game/Objects/Item.h"
#include "Game/Objects/Container.h"

#include "Game/Weapon/Pistols.h"

#include <random>
#include <map>

namespace GameLogic
{

class Breakable;

class Game
{
public:
    Game();

    void saveSettings();
    void loadSettings();

    void processCommandLine(const std::string& cmd);

    void onScreenResize(int width, int height);

    void onKeyPress(int key, bool keyDown);
    void onMouseMove(int x, int y);
    void onMouseButton(int button, bool down);
    void onMouseWheel(short delta);

    void update(float dt);

private:
    void readSettings(Settings& settings);

    bool loadMap(const std::string& name);
    void loadMapInfo(FILE* file);
    void loadLeafs(FILE* file, std::vector<Render::Leaf>& leafs);
    void loadZones(FILE* file, std::vector<Render::Zone>& zones);
    void loadPortals(FILE* file, std::vector<Render::Portal>& portals);
    void loadObjects(FILE* file);
    void loadEntities(FILE* file);
    void loadSurfaces(FILE* file);

    void resetMap();
    void mapTransit(const std::string& mapname, bool new_game = true);
    void mapFinished();

    bool isRunning();

    void showMenu();
    void hideMenu();

    void processConsole();
    void updateConsole(float dt);

    void showMessage(const char* text);

    void toggleCameraMode();

    void setVehicleCamera();
    void mountVehicle(Vehicle* vehicle);
    void dismountVehicle();

    void equipWeapon(Weapon& weapon);
    void hideWeapons();

    void takeDamage(uint32_t damage);
    void weaponFire();
    void pickMedkit(Item* item);

    void use();

    void activateObject(const std::string& id);

    void createDebris(uint32_t type, const vec3& pos, const mat3& rot, float lifetime, const vec3& velocity);

    void destroyContainer(Container* container);
    void destroyBarrel(Breakable* barrel);

    void onConsoleCommand(const std::string& cmd);

private:
    Render::SceneManager& m_sceneManager;
    Render::Camera& m_camera;
    Render::World& m_world;

    Physics::PhysicsManager& m_physicsManager;

    enum class GameState { Run, Pause, Menu, Finished };
    enum class ConsoleState { Closed, Opened, Opening, Closing };
    enum class CameraMode { FirstPerson, ThirdPerson };

    AsyncCallQueue m_asyncQueue;

    bool m_activeMap;
    GameState m_gameState;

    std::string m_nextmap;

    LinkedList<StaticGameObject> m_staticObjects;
    LinkedList<GameObject> m_objects;
    std::vector<Trigger> m_triggers;
    std::map<std::string, GameObject*> m_activeObjects;
    std::vector<ClimbArea> m_climbAreas;

    // Timers
    Timer m_msgTimer;

    // UI
    Hud m_hud;
    Widgets::Label m_pauseLable;
    Widgets::Label m_msg;
    Widgets::Label m_fps;
    Menu m_menu;

    Console m_console;
    ConsoleState m_consoleState;
    float m_consolePos;

    uint32_t m_updateCounter;
    bool m_stats;
    bool m_wireframe;
    bool m_ghostMode;

    Player m_player;
    Vehicle* m_vehicle;

    CameraController* m_cameraController;
    FirstPersonCamera m_playerCamera;
    FirstPersonCamera m_firstPersonCamera;
    ThirdPersonCamera m_thirdPersonCamera;

    CameraMode m_cameraMode;

    Weapon* m_weapon;
    Pistols m_pistols;

    // Resources
    SoundPtr m_medkitSound;

    bool m_mouseButtons[3];

    std::mt19937 m_randomGenerator;

    enum MouseButton
    {
        LeftMouseButton = 0,
        RightMouseButton = 1,
        MiddleMouseButton = 2
    };

    static const std::string SettingsFile;

    static constexpr float ConsoleScreenPart = 0.65f;
    static constexpr float UseDistance = 0.7f;
};

} // namespace GameLogic