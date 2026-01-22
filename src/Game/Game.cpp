#include "System/Win32App.h"

#include "Game.h"
#include "Resources/Resources.h"
#include "UI/UiLayer.h"

#include "System/AudioManager.h"

#include "Physics/PhysicsManager.h"

#include "Game/Core/Particle.h"

#include "Game/Core/Character.h"
#include "Game/Core/RagDoll.h"

#include "Game/Objects/Container.h"
#include "Game/Objects/Debris.h"
#include "Game/Objects/Explosion.h"

#include <iomanip>
#include <sstream>
#include <fstream>

namespace GameLogic
{

const std::string  Game::SettingsFile = "settings.cfg";
    
Game::Game()
: m_sceneManager(Render::SceneManager::GetInstance())
, m_camera(m_sceneManager.getCamera())
, m_world(m_sceneManager.getWorld())
, m_physicsManager(Physics::PhysicsManager::GetInstance())
, m_activeMap(false)
, m_gameState(GameState::Run)
, m_pauseLable(nullptr, 0, 0, 18, 18, "Audiowide", UI::Widget::Alignment::Center)
, m_msg(nullptr, 0, 0, 18, 18, UI::Widget::Alignment::Center)
, m_fps(nullptr, 10, 10, 18, 18, UI::Widget::Alignment::TopLeft)
, m_consoleState(ConsoleState::Closed)
, m_consolePos(-300)
, m_updateCounter(0)
, m_stats(false)
, m_wireframe(false)
, m_ghostMode(false)
, m_player({ 0.0, 1.0, 0.0 }, m_playerCamera)
, m_vehicle(nullptr)
, m_playerCamera(m_camera)
, m_firstPersonCamera(m_camera)
, m_thirdPersonCamera(m_camera)
, m_cameraController(&m_playerCamera)
, m_cameraMode(CameraMode::FirstPerson)
, m_medkitSound(ResourceManager::GetSound("Items/medkit_pickup.wav"))
{
    m_randomGenerator.seed(1729);

    AudioManager::GetInstance().setVolume(0.5);

    UI::UiLayer& uiLayer = UI::UiLayer::GetInstance();

    m_sceneManager.setSkybox("sky01");

    m_console.OnCommand.bind(this, &Game::onConsoleCommand);

    m_hud.onScreenFaded().bind_async(m_asyncQueue, this, &Game::mapFinished);

    uiLayer.setCursor(ResourceManager::GetImage("ui/cursor.dds"));
    //uiLayer.showCursor(true);

    m_objects.append(&m_player);

    m_player.OnDamage.bind([this](Actor*, uint32_t damage) {
        takeDamage(damage);
    });

    m_player.OnDie.bind([this](Actor*) {
        hideWeapons();
    });

    m_playerCamera.setObject(&m_player);

    equipWeapon(m_pistols);
    m_pistols.onAttack.bind(this, &Game::weaponFire);

    m_msg.setColor(0.0f, 0.7f, 1.0f);
    m_msg.hide();
    m_msgTimer.OnFire.bind([this]()
    {
        m_msg.hide();
    });

    m_fps.setColor(0.0f, 1.0f, 0.0f);
    m_fps.hide();

    m_menu.hide();

    m_menu.OnNewGameEvent().bind([this]() {
        mapTransit("map1", true);
    });

    m_menu.OnExitEvent().bind([]() {
        App::Shutdown();
    });

    m_pauseLable.setColor(0.0f, 0.48f, 0.69f);
    m_pauseLable.setText("PAUSE");
    m_pauseLable.hide();
}

void Game::saveSettings()
{
    std::ofstream out;

    out.open(SettingsFile);

    if (!out.is_open()) return;

    const Settings& settings = m_menu.settings();

    out << "resolution " << settings.resolution << std::endl;
    out << "fullscreen " << App::IsFullscreen() << std::endl;  // We can bypass menu data here
    out << "vsync " << settings.vsync << std::endl;
    out << "gi " << settings.gi << std::endl;
    out << "volume " << settings.volume << std::endl;
}

void Game::readSettings(Settings& settings)
{
    std::ifstream in;

    in.open(SettingsFile);

    if (!in.is_open()) return;

    std::string param;

    while (!in.eof())
    {
        in >> param;

        if (param == "resolution") in >> settings.resolution;
        else if (param == "fullscreen") in >> settings.fullscreen;
        else if (param == "vsync") in >> settings.vsync;
        else if (param == "gi") in >> settings.gi;
        else if (param == "volume") in >> settings.volume;
        else in >> param;
    }

    m_menu.setSettings(settings);
}

void Game::loadSettings()
{
    // Settings
    Settings settings;

    readSettings(settings);

    AudioManager::GetInstance().setVolume(settings.volume);
    m_sceneManager.enableGI(settings.gi);

    App::Resize(settings.resolution);
    if (settings.fullscreen) App::ToggleFullscreen();
    App::EnableVSync(settings.vsync);
}

void Game::processCommandLine(const std::string& cmd)
{
    if (cmd.empty())
    {
        mapTransit("test1", true);
        return;
    }

    // Split command
    static const char* delim = "	 "; // space and tab

    std::vector<std::string_view> arg;

    size_t start = 0;
    size_t end = 0;

    while (true)
    {
        end = cmd.find_first_of(delim, start);

        if (end == std::string::npos)
        {
            arg.emplace_back(std::string_view(cmd).substr(start));
            break;
        }
        else
        {
            arg.emplace_back(std::string_view(cmd).substr(start, end - start));
            start = cmd.find_first_not_of(delim, end);

            if (start == std::string::npos) break;
        }
    }

    if (arg.size() > 1)
    {
        if (arg[0] == "-m")
        {
            mapTransit(std::string(arg[1]));
            return;
        }
    }
}

void Game::onScreenResize(int width, int height)
{
    m_hud.resize(width, height);
    m_console.resize(width, height * ConsoleScreenPart);

    if (m_consoleState == ConsoleState::Closed) m_console.move(0, -m_console.height());

    m_pauseLable.move(width * 0.5, height * 0.6);
    m_msg.move(width / 2, height / 2);

    m_menu.move(width / 2 - m_menu.width() / 2, height / 2 - m_menu.height() / 2);
}

void Game::onSwichFullscreen(bool fullscreen)
{
    m_menu.setFullscreenStatus(fullscreen);
}

void Game::onKeyPress(int key, bool keyDown)
{
    if (key == 192) // Tilde
    {
        processConsole();
        return;
    }

    if (m_consoleState != ConsoleState::Closed) return;
    if (m_gameState == GameState::Finished) return;

    if (key == 0x1B && keyDown) // Escape
    {
        if (m_gameState != GameState::Menu)
        {
            showMenu();
        }
        else
        {
            if (m_menu.escape()) hideMenu();
        }
    }

    if (key == 0x13 && keyDown) // Pause
    {
        if (m_gameState != GameState::Menu && m_gameState != GameState::Pause)
        {
            m_gameState = GameState::Pause;
            m_pauseLable.show();
        
        } 
        else if (m_gameState == GameState::Pause)
        {
            m_gameState = GameState::Run;
            m_pauseLable.hide();
        }
    }

    if (m_gameState != GameState::Run) return;

    if (key == 'E' && keyDown)
    {
        use();
        return;
    }

    if (m_vehicle)
    {
        if (key == 'H' && keyDown)
        {
            toggleCameraMode();
            return;
        }

        m_vehicle->input(key, keyDown);
    }
    else
        m_player.input(key, keyDown);
}

void Game::onMouseMove(int x, int y)
{
    if (m_consoleState != ConsoleState::Closed) return;

    if (m_gameState == GameState::Menu) UI::UiLayer::GetInstance().onMouseMove(x, y);
    
    if (m_gameState != GameState::Run) return;

    m_cameraController->onMouseMove(x, y);
}

void Game::onMouseButton(int button, bool down)
{
    if (m_consoleState != ConsoleState::Closed) return;

    if (m_gameState == GameState::Menu) UI::UiLayer::GetInstance().onMouseButton(button, down);

    if (m_gameState != GameState::Run) return;

    m_mouseButtons[button] = down;
}

void Game::onMouseWheel(short delta)
{
    if (m_gameState == GameState::Menu) UI::UiLayer::GetInstance().onMouseWheel(delta);

    if (m_gameState != GameState::Run) return;

    m_cameraController->onScroll(delta);
}

void Game::resetMap()
{
    m_player.resetClimbArea();

    if (m_vehicle)
    {
        m_vehicle = nullptr;
        m_cameraController = &m_playerCamera;
        equipWeapon(m_pistols);
    }
    else
    {
        m_objects.remove(&m_player);
        m_physicsManager.removeRigidBody(&m_player);
    }

    Hitable::ClearObjects();
    Pickable::ClearObjects();

    m_sceneManager.reset();
    m_physicsManager.reset();

    m_staticObjects.destroy();
    m_objects.destroy();
    m_triggers.clear();
    m_vehicleTriggers.clear();
    m_activeObjects.clear();
    m_climbAreas.clear();

    m_hud.hide();

    m_objects.append(&m_player);
    Hitable::AddObject(&m_player);

    m_player.setVelocity({});
}

void Game::mapTransit(const std::string& mapname, bool new_game)
{
    if (m_activeMap) resetMap();

    m_player.moveTo({ 0.0, 1.0, 0.0 });
    m_player.resetInput();

    std::string filename = "./Maps/" + mapname + ".dcm";

    //m_camera.setPos({ 0.0, 1.0, 0.0 });
    m_playerCamera.setAngles(0.0f, 0.0f);

    m_activeMap = loadMap(filename);

    if (m_activeMap)
    {
        m_physicsManager.addRigidBody(&m_player);

        // Joint test setup
        /*{
            PhysicsObject* obj[3];

            for (int i = 0; i < 3; i++)
            {
                obj[i] = new PhysicsObject({ 1, float(i + 1) + 1, 0 }, {}, 20, collision_solid | collision_hitable | collision_actor, ResourceManager::GetModel("debug_bone.msh"));
                m_objects.append(obj[i]);
            }

            const vec3& bbox0 = obj[0]->boundingBox();
            const vec3& bbox1 = obj[1]->boundingBox();
            const vec3& bbox2 = obj[2]->boundingBox();

            float angmin[3] = { -1.92f, 0.0f, -1.92f };
            float angmax[3] = { 1.92f, 0.0f, 1.92f };

            Physics::Joint* joint1 = new Physics::Joint(obj[0], { 0.0f, bbox0[1], 0.0f }, obj[1], { 0.0f, -bbox1[1], 0.0f }, angmin, angmax);
            Physics::Joint* joint2 = new Physics::Joint(obj[1], { 0.0f, bbox1[1], 0.0f }, obj[2], { 0.0f, -bbox2[1], 0.0f }, angmin, angmax);

            m_physicsManager.addJoint(joint1);
            m_physicsManager.addJoint(joint2);
        }*/

        m_sceneManager.updateWorld();

        ResourceManager::Cleanup();

        m_hud.reset();
        m_hud.show();
       
        if (m_gameState == GameState::Menu)
        {
            UI::UiLayer::GetInstance().showCursor(false);
            m_menu.hide();
        }

        if (m_gameState != GameState::Run) m_gameState = GameState::Run;

        m_console.print(std::string("loaded map ") + mapname);
    }
    else
        m_console.print(std::string("failed to load map ") + mapname);

    if (new_game)
    {
        m_player.makeInvulnerable(false);
        m_player.revive();
        m_player.setGhostMode(false);
        m_player.walk();

        m_hud.setHealth(m_player.health());

        hideWeapons();
        equipWeapon(m_pistols);
    }

    App::SyncTime();
}

void Game::mapFinished()
{
    if (m_nextmap.empty())
    {
        showMessage("To be continue ...");
        resetMap();

        m_gameState = GameState::Run;
    }
    else
    {
        mapTransit(m_nextmap, false);
    }
}

void Game::update(float dt)
{
    if (m_stats)
    {
        m_updateCounter++;

        if (m_updateCounter >= 30)
        {
            m_updateCounter = 0;

            std::stringstream stream;
            stream << "fps: " << std::setprecision(3) << (1.0f / dt);

            m_fps.setText(stream.str().c_str());
        }
    }

    if (m_consoleState != ConsoleState::Closed) updateConsole(dt);
    
    if (isRunning())
    {
        m_sceneManager.clearSprites();

        m_physicsManager.run(dt);

        for (GameObject* object : m_objects) object->update(dt);

        if (m_vehicle)
            for (Trigger& trigger : m_vehicleTriggers) trigger.test(m_vehicle->location(), m_vehicle->boundingBox());
        else
            for (Trigger& trigger : m_triggers) trigger.test(m_player.pos(), m_player.bbox());

        Pickable::TestObjects(m_player.pos(), m_player.bbox());

        if (!m_player.isClimbing() && m_player.isWalking())
        {
            for (const ClimbArea& climbArea : m_climbAreas)
            {
                if (climbArea.test(m_player.pos(), m_player.bbox())) m_player.attachToClimbArea(&climbArea);
            }
        }

        if (!m_player.isDead())
        {
            m_playerCamera.setViewOffset({ 0.0f, m_player.headDist(), 0.0f });

            if (m_weapon)
            {
                m_weapon->update(dt);

                if (m_mouseButtons[LeftMouseButton]) m_weapon->fire();
            }
        }

        AudioManager::SetListenerPos(m_camera.pos());
    }

    m_asyncQueue.execute();

    m_cameraController->update(dt);
}

bool Game::isRunning()
{
    return m_gameState == GameState::Run && m_consoleState == ConsoleState::Closed;
}

void Game::showMenu()
{
    UI::UiLayer& uiLayer = UI::UiLayer::GetInstance();

    m_pauseLable.hide();

    m_gameState = GameState::Menu;
    m_menu.show();
    uiLayer.showCursor(true);
}

void Game::hideMenu()
{
    UI::UiLayer& uiLayer = UI::UiLayer::GetInstance();

    m_menu.hide();
    uiLayer.showCursor(false);

    m_gameState = GameState::Run;
}

void Game::processConsole()
{
    switch (m_consoleState)
    {
    case ConsoleState::Closed:
        m_consoleState = ConsoleState::Opening;
        m_console.show();

        if (m_gameState == GameState::Menu) UI::UiLayer::GetInstance().showCursor(false);
        break;

    case ConsoleState::Opened:
        m_consoleState = ConsoleState::Closing;
        m_console.killFocus();
        break;
    }
}

void Game::updateConsole(float dt)
{
    int consoleHeight = m_console.height();

    switch (m_consoleState)
    {
    case ConsoleState::Opening:
        
        m_consolePos += dt * 500;
        
        if (m_consolePos >= 0)
        {
            m_consolePos = 0;
            m_consoleState = ConsoleState::Opened;
            m_console.setFocus();
        }
        break;

    case ConsoleState::Closing:
        
        m_consolePos -= dt * 500;
        
        if (m_consolePos <= -consoleHeight)
        {
            m_consolePos = -consoleHeight;
            m_consoleState = ConsoleState::Closed;
            m_console.hide();

            if (m_gameState == GameState::Menu) UI::UiLayer::GetInstance().showCursor(true);
        }
        break;
    }

    m_console.move(0, m_consolePos);
}

void Game::showMessage(const char* text)
{
    m_msg.setText(text);
    m_msg.show();
    m_msgTimer.start(2, true);
}

void Game::toggleCameraMode()
{
    m_cameraMode = m_cameraMode == CameraMode::FirstPerson ? CameraMode::ThirdPerson : CameraMode::FirstPerson;
    setVehicleCamera();
}

void Game::setVehicleCamera()
{
    if (m_cameraMode == CameraMode::FirstPerson)
    {
        m_firstPersonCamera.setObject(m_vehicle);
        m_firstPersonCamera.setViewOffset(m_vehicle->viewPoint());
        m_cameraController = &m_firstPersonCamera;
    }
    else
    {
        m_thirdPersonCamera.setObject(m_vehicle);
        m_cameraController = &m_thirdPersonCamera;
    }
}

void Game::mountVehicle(Vehicle* vehicle)
{
    m_player.resetInput();

    m_objects.remove(&m_player);
    m_physicsManager.removeRigidBody(&m_player);

    m_vehicle = vehicle;

    setVehicleCamera();
    hideWeapons();
}

void Game::dismountVehicle()
{
    if (m_vehicle->velocity().length() > 5.0f) return;

    m_player.moveTo(m_vehicle->location() + m_vehicle->orientation() * vec3(-1.2f, 0.3f, 0.3f));

    m_vehicle->dismount();
    m_vehicle = nullptr;

    m_objects.append(&m_player);
    m_physicsManager.addRigidBody(&m_player);
    m_cameraController = &m_playerCamera;

    m_playerCamera.setAngles(m_firstPersonCamera.verticalAngle(), m_firstPersonCamera.horizontalAngle());

    equipWeapon(m_pistols);
}

void Game::equipWeapon(Weapon& weapon)
{
    m_weapon = &weapon;
    m_weapon->equip();
}

void Game::hideWeapons()
{
    m_weapon = nullptr;
    m_sceneManager.clearOverlay();
}

void Game::takeDamage(uint32_t damage)
{
    m_hud.setHealth(m_player.health());

    float intensity = std::min(std::max(0.2f, damage / 25.0f), 0.5f);
    m_hud.setHighlight({ 1.0f, 0.0f, 0.0f }, intensity);
}

void Game::weaponFire()
{
    Render::Camera& camera = m_sceneManager.getCamera();

    std::uniform_real_distribution<float> distribution(-0.005f, 0.005f);

    float xdev = distribution(m_randomGenerator);
    float ydev = distribution(m_randomGenerator);

    const mat3& cammat = camera.basis();
    vec3 direction = cammat[2] + cammat[0] * xdev + cammat[1] * ydev;
    direction.normalize();

    float dist;

    Collision::TraceRayInfo traceInfo;

    if (Physics::PhysicsManager::GetInstance().traceRay(camera.pos(), direction, collision_hitable, traceInfo))
    {
        if (traceInfo.object)
        {
            vec3 point = camera.pos() + direction * traceInfo.dist;
            reinterpret_cast<Hitable*>(traceInfo.object)->hit(point, direction, 5);
        }

        if (traceInfo.dist < 200)
        {
            vec3 hitpos = camera.pos() + direction * (traceInfo.dist - 0.05);

            Particle* particle = nullptr;

            if ((traceInfo.layers & collision_character) != 0)
                particle = new Particle(hitpos, 0.07f, { 0, -0.1f, 0 }, 0.0f, 0.4f, ResourceManager::GetImage("Effects/blood01.dds"));
            else
                particle = new Particle(hitpos, 0.1f, traceInfo.norm * 0.3f, 2.0f, 1.8f, ResourceManager::GetImage("Effects/smoke.dds"));

            m_objects.append(particle);
        }
    }
}

void Game::pickMedkit(Item* item)
{
    if (m_player.heal(20))
    {
        m_hud.setHealth(m_player.health());
        AudioManager::Play(m_medkitSound);

        m_asyncQueue.invoke([this, item]() {
            
            m_physicsManager.removeRigidBody(item);
            m_sceneManager.removeObject(item);

            m_objects.remove(item);

            delete item;
        });
    }
}

void Game::use()
{
    if (m_vehicle)
    {
        dismountVehicle();
        return;
    }

    Render::Camera& camera = m_sceneManager.getCamera();

    Collision::TraceRayInfo traceInfo;

    if (Physics::PhysicsManager::GetInstance().traceRay(camera.pos(), camera.direction(), collision_pickable, traceInfo))
    {
        if (traceInfo.dist < UseDistance && traceInfo.object)
        {
            reinterpret_cast<Usable*>(traceInfo.object)->use();
        }
    }
}

void Game::activateObject(const std::string& id)
{
    if (!id.empty())
    {
        auto it = m_activeObjects.find(id);
        if (it != m_activeObjects.end()) it->second->activate();
    }
}

void Game::createDebris(uint32_t type, const vec3& pos, const mat3& rot, float lifetime, const vec3& velocity)
{
    Debris* debris = new Debris(pos, rot, lifetime, type);

    debris->setVelocity(velocity);

    m_objects.append(debris);

    debris->OnLifetimeExpires.bind_async(m_asyncQueue, [this](Debris* obj)
    {
        m_physicsManager.removeRigidBody(obj);
        m_sceneManager.removeObject(obj);
        m_objects.remove(obj);

        delete obj;
    });
}

void Game::destroyContainer(Container* container)
{
    const vec3& pos = container->location();

    m_physicsManager.removeRigidBody(container);
    m_sceneManager.removeObject(container);
    m_objects.remove(container);

    Item* item = nullptr;

    std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
    vec3 rot = vec3(distribution(m_randomGenerator), distribution(m_randomGenerator), distribution(m_randomGenerator)) * math::pi2;

    switch (container->item())
    {
    case Container::Item::Medkit:
        item = new Item(pos, mat3::Rotate(rot.x, rot.y, rot.z), 0.1, 20, ResourceManager::GetModel("Items/medkit.msh"));

        item->OnPickup.bind([this, item]() {
            pickMedkit(item);
            });
    }

    if (item)
    {
        item->setVelocity({ 0, -0.1, 0 });
        m_objects.append(item);
    }

    // Create debris
    const mat3& orientation = container->orientation();

    for (int i = 0; i < 8; i++)
    {
        vec3 offset = { distribution(m_randomGenerator), distribution(m_randomGenerator), distribution(m_randomGenerator) };
        vec3 rot = vec3(distribution(m_randomGenerator), distribution(m_randomGenerator), distribution(m_randomGenerator)) * math::pi2;
        mat3 orientation = mat3::Rotate(rot.x, rot.y, rot.z);

        vec3 velocity = offset;
        velocity.normalize();

        uint32_t type = i <= 5 ? 2 : 1;

        float lifetime = 10.0f + distribution(m_randomGenerator) * 5.0f;

        createDebris(type, pos + offset * 0.4f, orientation, lifetime, velocity * 2.0f);
    }

    delete container;
}

void Game::destroyBarrel(Breakable* barrel)
{
    vec3 pos = barrel->location();

    m_physicsManager.removeRigidBody(barrel);
    m_sceneManager.removeObject(barrel);
    m_objects.remove(barrel);

    delete barrel;

    m_asyncQueue.invoke([this, pos]() {
        Hitable::Impact(pos, 150);
    });

    // Run this after explosion, otherwise scraps speed is too high
    m_asyncQueue.invoke([this, pos]() {
        constexpr int N = 3;

        std::uniform_real_distribution<float> distribution(0.0f, 5.0f);

        for (int i = 0; i < N; i++)
        {
            float ang = 360.0 / N * i;
            mat3 mat = mat3::Rotate(0, ang / 180.0f * math::pi, 0);

            vec3 fpos = pos + mat[0] * 0.2 + mat[2] * 0.2;
            vec3 velocity = fpos - pos;
            velocity.normalize();

            fpos += mat[1] * 0.2;

            float lifetime = 10.0f + distribution(m_randomGenerator);

            Debris* scrap = new Debris(fpos, mat, lifetime, 0);

            scrap->setVelocity(velocity * 6);

            m_objects.append(scrap);

            scrap->OnLifetimeExpires.bind_async(m_asyncQueue, [this](Debris* obj)
                {
                    m_physicsManager.removeRigidBody(obj);
                    m_sceneManager.removeObject(obj);
                    m_objects.remove(obj);

                    delete obj;
                });
        }
    });

    Explosion* explosion = new Explosion(pos, 5.0);
    m_objects.append(explosion);

    explosion->OnStop.bind_async(m_asyncQueue, [this](Explosion* obj)
    {
        m_objects.remove(obj);
        delete obj;
    });
}

void Game::onCharacterDeath(Character* character, const vec3& impulse)
{
    RagDoll* ragdoll = new RagDoll(character);
    m_objects.append(ragdoll);

    m_objects.remove(character);
    m_sceneManager.removeObject(character);
    m_sceneManager.unregisterSkeletalObject(character);
    delete character;

    ragdoll->applyImpulse(impulse);
}

void Game::onConsoleCommand(const std::string& cmd)
{
    // Split command
    static const char* delim = "	 "; // space and tab

    std::vector<std::string_view> arg;

    size_t start = 0;
    size_t end = 0;

    while (true)
    {
        end = cmd.find_first_of(delim, start);

        if (end == std::string::npos)
        {
            arg.emplace_back(std::string_view(cmd).substr(start));
            break;
        }
        else
        {
            arg.emplace_back(std::string_view(cmd).substr(start, end - start));
            start = cmd.find_first_not_of(delim, end);

            if (start == std::string::npos) break;
        }
    }

    if (arg[0] == "wire")
    {
        m_wireframe = !m_wireframe;
        m_sceneManager.setWireframeMode(m_wireframe);

        m_console.print(m_wireframe ? "wireframe mode on" : "wireframe mode off");
        return;
    }

    if (arg[0] == "god")
    {
        bool godMode = m_player.toggleInvulnerability();

        m_console.print(godMode ? "god mode on" : "god mode off");
        return;
    }

    if (arg[0] == "walk")
    {
        if (m_ghostMode) m_player.setGhostMode(false);

        m_ghostMode = false;
        m_player.walk();
        m_console.print("walking");
        return;
    }

    if (arg[0] == "fly")
    {
        if (m_ghostMode) m_player.setGhostMode(false);

        m_ghostMode = false;
        m_player.fly();
        m_console.print("flying");
        return;
    }

    if (arg[0] == "ghost")
    {
        m_ghostMode = true;
        m_player.fly();
        m_player.setGhostMode(true);
        m_console.print("ghost mode");
        return;
    }

    if (arg[0] == "stats")
    {
        if (m_stats)
        {
            m_fps.hide();
            m_console.print("hide stats");

            m_stats = false;
        }
        else
        {
            m_fps.show();
            m_console.print("show stats");
        
            m_stats = true;
        }

        return;
    }

    if (arg[0] == "exit")
    {
        App::Shutdown();
    }

    if (arg.size() > 1)
    {
        if (arg[0] == "map")
        {
            mapTransit(std::string(arg[1]));
            return;
        }
    }

    m_console.print(cmd);
}

} // namespace GameLogic