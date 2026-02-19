#include "Game.h"
#include "Game/GameMap.h"
#include "Game/Core/PhysicsObject.h"
#include "Game/SurfaceObject.h"
#include "Editor/DCM.h"
#include "Resources/Resources.h"
#include "Render/Light.h"
#include "Render/FogVolume.h"
#include "Render/Surface.h"

#include "Game/Core/AmbientSound.h"

#include "Game/Objects/Breakable.h"
#include "Game/Objects/Container.h"
#include "Game/Objects/SlideDoor.h"
#include "Game/Objects/MovingObject.h"
#include "Game/Objects/Item.h"
#include "Game/Objects/Activator.h"
#include "Game/Objects/Animated.h"

#include "Game/Core/Character.h"
#include "Game/Core/Vehicle.h"
#include "Game/Core/ParticleEmitter.h"

#include "Collision/PolygonalCollisionShape.h"

#include "stdio.h"
#include <iostream>

namespace GameLogic
{

const vec3 JeepWheels[] = { vec3{-0.68f, -0.05f, -0.938f},
                            vec3{0.68f, -0.05f, -0.938f},
                            vec3{0.68f, -0.05f, 0.938f},
                            vec3{-0.68f, -0.05f, 0.938f} };

const VehicleParams JeepParams = { {-0.35f, 0.7f, 0.0f},      // viewPoint
                                   { -0.35f, 0.5f, 0.35f },   // steeringWheelPos
                                   -45.0f,                    // steeringWheelAngle
                                   50.0f,                     // motorPower
                                   30.0f,                     // reverseMotorPower
                                   500.0f,                    // mass
                                   0.33f,                     // wheelRadius
                                   0.9f,                      // wheelFriction
                                   0.1f,                      // rollResistance
                                   0.08f,                     // suspensionLength
                                   50.0f,                     // suspensionStiffness
                                   15.0f,                     // suspensionDamping
                                   _countof(JeepWheels),      // numWheels
                                   JeepWheels };              // wheelPos

const Character::CharacterParams TrooperParams = { "enemies/trooper01.msh",                                 // model;
                                                   { 0.2f, 0.5f, 0.2f },                                    // bbox;
                                                   40.0f,                                                   // mass;
                                                   1.2f,                                                    // speed;
                                                   25,                                                      // health;
                                                   5,                                                       // damage;
                                                   2.0f,                                                    // attackDistance;
                                                   75 / 5 * Render::ArticulatedObject::default_frame_rate,  // attackTime;
                                                   15,                                                      // weaponBone;
                                                   {0.26f, 0.0f, 0.03f },                                   // weaponPos;
                                                   {math::pi * 0.5f, 0, 0},                                 // weaponRot;
                                                   { 0, 30 },                                               // idleAnim
                                                   { 30, 70 },                                              // moveAnim
                                                   { 70, 80 },                                              // attackAnim
                                                   { 80, 100 }                                              // deathAnim
                                                    };

void readString(std::string& str, FILE* file)
{
    uint16_t len;
    
    fread(&len, sizeof(uint16_t), 1, file);
    if (len > 0)
    {
        str.resize(len);
        fread(str.data(), sizeof(char), len, file);
    }
}

bool Game::loadMap(const std::string& name)
{
    FILE* file;
    DcmHeader head;

    //Open file
    errno_t error = fopen_s(&file, name.c_str(), "rb");

    if (error)
    {
        std::cout << "Can't open file " << name << std::endl;
        return false;
    }

    fread(&head, 1, sizeof(DcmHeader), file);

    if (head.signature != DcmSignature)
    {
        std::cout << "Map loading: Not a DCM file" << std::endl;
        return false;
    }

    if (head.version != DcmVersion)
    {
        std::cout << "Map loading: Invalid map version" << std::endl;
        return false;
    }

    fseek(file, head.eoffset, SEEK_SET);

    std::vector<Render::Vertex> verts;
    std::vector<Render::Leaf> leafs;
    std::vector<Render::Node> nodes;
    std::vector<Render::Zone> zones;
    std::vector<Render::Portal> portals;

    std::vector<vec3> raytraceVerts;
    m_sceneManager.initStaticGeometry();

    DcmEntry* entry = new DcmEntry[head.entrynum];
    fread(entry, head.entrynum, sizeof(DcmEntry), file);

    for (int e = 0; e < head.entrynum; e++)
    {
        fseek(file, entry[e].offset, SEEK_SET);

        switch (entry[e].type)
        {
        case DcmEntryType::entry_mapinfo:
            loadMapInfo(file);
        break;

        case DcmEntryType::entry_geometry:
        {
            uint32_t vnum;
            fread(&vnum, sizeof(uint32_t), 1, file);
            
            verts.resize(vnum);
            fread(verts.data(), sizeof(Render::Vertex), vnum, file);

            fread(&vnum, sizeof(uint32_t), 1, file);
        }
        break;

        case DcmEntryType::entry_leafs:
            loadLeafs(file, leafs);
        break;

        case DcmEntryType::entry_nodes:

            uint32_t numnodes;
            fread(&numnodes, 1, sizeof(uint32_t), file);

            std::cout << "nodes num: " << numnodes << std::endl;

            nodes.resize(numnodes);
            fread(nodes.data(), numnodes, sizeof(Render::Node), file);

        break;

        case DcmEntryType::entry_portals:
            loadPortals(file, portals);
        break;

        case DcmEntryType::entry_zoneinfo:
            loadZones(file, zones);
        break;

        case DcmEntryType::entry_objects:
            loadObjects(file);
        break;

        case DcmEntryType::entry_entities:
            loadEntities(file);
        break;

        case DcmEntryType::entry_surfaces:
            loadSurfaces(file);
        break;

        default:
            break;
        }
    }

    std::vector<Render::GeometryData> geometryData;

    for (const Render::Leaf& leaf : leafs)
    {
        if (leaf.raytraceGeometry.count == 0) continue;

        geometryData.push_back({ leaf.raytraceGeometry.offset, leaf.raytraceGeometry.count });
    }

    m_world.setData(verts, leafs, nodes, zones, portals);
    m_sceneManager.addStaticGeometry(m_world.vertexData(), geometryData);

    fclose(file);

    return true;
}

void Game::loadMapInfo(FILE* file)
{
    uint16_t len;
    fread(&len, sizeof(uint16_t), 1, file);

    std::string skyname;
    skyname.resize(len);
    fread(skyname.data(), 1, len, file);

    m_sceneManager.setSkybox(skyname);

    bool dirLight = false;
    vec3 dirLightDirection;
    vec3 dirLightColor;

    fread(&dirLight, sizeof(bool), 1, file);
    fread(&dirLightDirection, sizeof(vec3), 1, file);
    fread(&dirLightColor, sizeof(vec3), 1, file);

    if (dirLight)
        m_sceneManager.setDirectionalLight(dirLightDirection, dirLightColor);

    bool gi = false;
    vec3 giColor;

    fread(&gi, sizeof(bool), 1, file);
    fread(&giColor, sizeof(vec3), 1, file);

    if (gi)
        m_sceneManager.setGi(giColor);
}

void Game::loadLeafs(FILE* file, std::vector<Render::Leaf>& leafs)
{
    uint32_t numleafs;
    
    fread(&numleafs, 1, sizeof(uint32_t), file);
    leafs.resize(numleafs);

    std::vector<Poly> polys;

    uint32_t rtxoffset = 0;

    for (uint32_t l = 0; l < numleafs; l++)
    {
        uint32_t nverts = 0;
        uint32_t dnum;

        Render::Leaf& leaf = leafs[l];

        fread(&leaf.bbox.min, 1, sizeof(vec3), file);
        fread(&leaf.bbox.max, 1, sizeof(vec3), file);

        fread(&leaf.zone, 1, sizeof(Render::Index), file);
        fread(&leaf.flags, sizeof(uint8_t), 1, file);
        fread(&dnum, 1, sizeof(uint32_t), file);

        std::string matname;
        bool sky = false;

        leaf.regularGeometry.reserve(dnum);

        for (int i = 0; i < dnum; i++)
        {
            uint32_t mlen;
            fread(&mlen, 1, sizeof(uint32_t), file);

            matname.resize(mlen);
            fread(matname.data(), 1, mlen, file);

            Material* material = ResourceManager::GetMaterial(matname);

            uint32_t offset;
            uint32_t vertexnum;

            fread(&offset, sizeof(uint32_t), 1, file);
            fread(&vertexnum, sizeof(uint32_t), 1, file);

            if (material->type == Material::material_transparent)
                leaf.transparentGeometry.push_back({ material, offset, vertexnum });
            else if (material->type == Material::material_emissive)
                leaf.emissiveGeometry.push_back({ material, offset, vertexnum });
            else
                leaf.regularGeometry.push_back({ material, offset, vertexnum });
        }

        fread(&leaf.raytraceGeometry.offset, sizeof(uint32_t), 1, file);
        fread(&leaf.raytraceGeometry.count, sizeof(uint32_t), 1, file);

        uint32_t polynum;
        fread(&polynum, 1, sizeof(uint32_t), file);

        auto& collisionPolygons = leafs[l].collisionPolygons;
        collisionPolygons.resize(polynum);

        for (uint32_t p = 0; p < polynum; p++)
        {
            Collision::CollisionPolygon& poly = collisionPolygons[p];
            uint32_t vnum;

            fread(&vnum, sizeof(uint32_t), 1, file);

            poly.verts.resize(vnum);
            fread(poly.verts.data(), sizeof(vec3), vnum, file);

            // calculate normal
            vec3 a = poly.verts[1] - poly.verts[0];
            vec3 b = poly.verts[2] - poly.verts[0];
            poly.plane.xyz = a ^ b;
            poly.plane.xyz.normalize();
            poly.plane.w = -(poly.verts[0] * poly.plane.xyz);

            uint32_t triangles = poly.verts.size() - 2;
        }

        static const mat3 GlobalOrientation = {};
        static const vec3 GlobalPosition = {};

        Collision::PolygonalCollisionShape* collisionShape = new Collision::PolygonalCollisionShape(GlobalOrientation, GlobalPosition, collisionPolygons.size(), collisionPolygons.data());
        Physics::StationaryBody* staticBody = new Physics::StationaryBody(collisionShape, leafs[l].bbox, collision_map);

        Physics::PhysicsManager::GetInstance().addStationaryBody(staticBody);
    }
}

void Game::loadZones(FILE* file, std::vector<Render::Zone>& zones)
{
    uint32_t numzones;

    fread(&numzones, 1, sizeof(uint32_t), file);
    zones.resize(numzones);

    for (uint32_t z = 0; z < numzones; z++)
    {
        uint32_t lnum;
        uint32_t pnum;

        fread(&zones[z].type, 1, sizeof(uint8_t), file);
       
        fread(&lnum, 1, sizeof(uint32_t), file);
        zones[z].leafs.resize(lnum);
        fread(zones[z].leafs.data(), lnum, sizeof(Render::Index), file);

        fread(&pnum, 1, sizeof(uint32_t), file);
        zones[z].portals.resize(pnum);
        fread(zones[z].portals.data(), pnum, sizeof(Render::Index), file);
    }

    std::cout << "zones num: " << numzones << std::endl;
}

void Game::loadPortals(FILE* file, std::vector<Render::Portal>& portals)
{
    uint32_t numportals;

    fread(&numportals, 1, sizeof(uint32_t), file);
    portals.resize(numportals);

    for (uint32_t p = 0; p < numportals; p++)
    {
        fread(&portals[p].plane, 1, sizeof(vec4), file);

        fread(&portals[p].zone[0], 1, sizeof(Render::Index), file);
        fread(&portals[p].zone[1], 1, sizeof(Render::Index), file);

        uint32_t vnum;
        fread(&vnum, 1, sizeof(uint32_t), file);

        portals[p].verts.resize(vnum);
        portals[p].vbuff[0].reserve(vnum * 2);
        portals[p].vbuff[1].reserve(vnum * 2);

        portals[p].bid = -1;

        fread(portals[p].verts.data(), vnum, sizeof(vec3), file);

        portals[p].center = {};

        for (size_t v = 0; v < portals[p].verts.size(); v++) portals[p].center += portals[p].verts[v];
        portals[p].center *= 1.0f / portals[p].verts.size();
    }
}

void Game::loadObjects(FILE* file)
{
    while (true)
    {
        ObjectType otag;

        fread(&otag, 1, sizeof(ObjectType), file);
        if (otag == ObjectType::object_endlist) break;

        if (otag == ObjectType::object_omni_light)
        {
            vec3 pos;
            float radius;
            float falloff;
            float power;
            Render::LightShadow shadow;
            uint8_t r, g, b;

            fread(&pos, sizeof(vec3), 1, file);
            fread(&radius, sizeof(float), 1, file);
            fread(&falloff, sizeof(float), 1, file);
            fread(&power, sizeof(float), 1, file);
            fread(&shadow, sizeof(Render::LightShadow), 1, file);
            fread(&r, sizeof(uint8_t), 1, file);
            fread(&g, sizeof(uint8_t), 1, file);
            fread(&b, sizeof(uint8_t), 1, file);

            vec3 flux = vec3(r / 255.0, g / 255.0, b / 255.0) * power;

            Render::Light* light = new Render::Light(pos, flux, radius, falloff, shadow);

            m_sceneManager.addLight(light);
        }

        if (otag == ObjectType::object_spot_light)
        {
            vec3 pos;
            vec3 dir;
            float radius;
            float falloff;
            float outerAngle;
            float innerAngle;
            float power;
            Render::LightShadow shadow;
            uint8_t r, g, b;

            fread(&pos, sizeof(vec3), 1, file);
            fread(&dir, sizeof(vec3), 1, file);
            fread(&radius, sizeof(float), 1, file);
            fread(&falloff, sizeof(float), 1, file);
            fread(&outerAngle, sizeof(float), 1, file);
            fread(&innerAngle, sizeof(float), 1, file);
            fread(&power, sizeof(float), 1, file);
            fread(&shadow, sizeof(Render::LightShadow), 1, file);
            fread(&r, sizeof(uint8_t), 1, file);
            fread(&g, sizeof(uint8_t), 1, file);
            fread(&b, sizeof(uint8_t), 1, file);

            vec3 flux = vec3(r / 255.0, g / 255.0, b / 255.0) * power;

            Render::Light* light = new Render::Light(pos, dir, flux, radius, falloff, outerAngle, innerAngle, shadow);

            m_sceneManager.addLight(light);
        }

        if (otag == ObjectType::object_static)
        {
            vec3 pos;
            mat3 mat;

            fread(&pos, sizeof(vec3), 1, file);
            fread(&mat[0], sizeof(vec3), 1, file);
            fread(&mat[1], sizeof(vec3), 1, file);

            mat[1] -= mat[0] * (mat[0] * mat[1]);   //ortagonize
            mat[2] = mat[0] ^ mat[1];

            std::string modelName;
            uint32_t len;

            fread(&len, sizeof(uint32_t), 1, file);
            modelName.resize(len);

            fread(modelName.data(), 1, len, file);

            uint8_t flags;
            fread(&flags, sizeof(uint8_t), 1, file);

            Model* model = ResourceManager::GetModel(modelName);
            StaticGameObject* object = new StaticGameObject(pos, mat, model, flags);

            m_staticObjects.append(object);
        }

        if (otag == ObjectType::object_sound)
        {
            vec3 pos;
            float volume;

            fread(&pos, sizeof(vec3), 1, file);
            fread(&volume, sizeof(float), 1, file);

            std::string source;

            uint32_t len;
            fread(&len, sizeof(uint32_t), 1, file);

            if (len > 0)
            {
                source.resize(len);
                fread(source.data(), 1, len, file);
            }

            AmbientSound* sound = new AmbientSound(pos, volume, ResourceManager::GetSound(source));
            m_objects.append(sound);
        }

        if (otag == ObjectType::object_trigger)
        {
            vec3 pos;
            vec3 size;
            bool vehicle;

            fread(&pos, sizeof(vec3), 1, file);
            fread(&size, sizeof(vec3), 1, file);
            fread(&vehicle, sizeof(bool), 1, file);

            std::string id = {};

            uint16_t len;
            
            fread(&len, sizeof(uint16_t), 1, file);
            if (len > 0)
            {
                id.resize(len);
                fread(id.data(), sizeof(char), len, file);
            }

            Trigger& trigger = vehicle ? m_vehicleTriggers.emplace_back(size, pos) :
                                         m_triggers.emplace_back(size, pos);

            trigger.OnTrigger.bind([this, id]() {
                activateObject(id);
            });          
        }

        if (otag == ObjectType::object_player_start)
        {
            vec3 pos;
            float ang;

            fread(&pos, sizeof(vec3), 1, file);
            fread(&ang, sizeof(float), 1, file);

            m_player.moveTo(pos);
            m_playerCamera.setAngles(0.0f, ang + 180.0f);
        }

        if (otag == ObjectType::object_map_finish)
        {
            vec3 pos;
            vec3 size;
            std::string nextmap;

            fread(&pos, sizeof(vec3), 1, file);
            fread(&size, sizeof(vec3), 1, file);
            readString(nextmap, file);

            Trigger& trigger = m_triggers.emplace_back(size, pos);

            trigger.OnTrigger.bind([this, nextmap]() {
                m_nextmap = nextmap;
                m_gameState = GameState::Finished;
                m_hud.fade();
            });
        }

        if (otag == ObjectType::object_ladder)
        {
            vec3 pos;
            float height;
            float ang;

            fread(&pos, sizeof(vec3), 1, file);
            fread(&height, sizeof(float), 1, file);
            fread(&ang, sizeof(float), 1, file);

            vec2 norm = { sinf(ang), cosf(ang) };

            m_climbAreas.emplace_back(pos, -norm, vec3(0.25f, height, 0.1f));
        }

        if (otag == ObjectType::object_fog_volume)
        {
            vec3 pos;
            vec3 size;
            vec3 color;
            float density;
            uint8_t type;
            bool lighting;

            fread(&pos, sizeof(vec3), 1, file);
            fread(&size, sizeof(vec3), 1, file);
            fread(&color, sizeof(vec3), 1, file);
            fread(&density, sizeof(float), 1, file);
            fread(&type, sizeof(uint8_t), 1, file);
            fread(&lighting, sizeof(bool), 1, file);

            Render::FogVolume* volume = new Render::FogVolume(pos, size, color, density, type, lighting);
            m_sceneManager.addFogVolume(volume);
        }
    }
}

void Game::loadEntities(FILE* file)
{
    while (true)
    {
        EntityClass eclass;

        fread(&eclass, 1, sizeof(EntityClass), file);
        if (eclass == EntityClass::Invalid) break;

        vec3 pos;
        mat3 mat;

        fread(&pos, sizeof(vec3), 1, file);
        fread(&mat[0], sizeof(vec3), 1, file);
        fread(&mat[1], sizeof(vec3), 1, file);

        mat[1] -= mat[0] * (mat[0] * mat[1]);   //ortagonize
        mat[2] = mat[0] ^ mat[1];

        if (eclass == EntityClass::Crate)
        {
            Container::Item item;
            fread(&item, sizeof(uint8_t), 1, file);

            Container* container = new Container(pos, mat, 0, item);
            m_objects.append(container);

            container->OnDestroy.bind_async(m_asyncQueue, [this](Breakable* object) { 
                destroyContainer(reinterpret_cast<Container*>(object));
            });
        }

        if (eclass == EntityClass::Barrel)
        {
            Breakable* object = new Breakable(pos, mat, 20, 60,
                                              ResourceManager::GetModel("Tech/barrel.msh"),
                                              nullptr,
                                              nullptr,
                                              ResourceManager::GetSound("explosion.wav"));

            m_objects.append(object);
            object->OnDestroy.bind(this, &Game::destroyBarrel);
        }

        if (eclass == EntityClass::SlideDoor)
        {
            std::string id;
            bool closed;
            bool gi;

            readString(id, file);

            fread(&closed, sizeof(bool), 1, file);
            fread(&gi, sizeof(bool), 1, file);

            SlideDoor* door = new SlideDoor("", pos, mat, closed, gi);
            m_objects.append(door);

            Trigger& trigger = m_triggers.emplace_back(door->StationaryBody::bbox(), door->StationaryBody::pos());
            
            trigger.OnTrigger.bind(door, &SlideDoor::open);
            trigger.OnRelease.bind(door, &SlideDoor::release);

            if (!id.empty()) m_activeObjects[id] = door;
        }

        if (eclass == EntityClass::Mover)
        {
            std::string id;
            std::string model;
            std::string sound;

            vec3 dir;
            float dist;
            float speed;

            readString(id, file);
            readString(model, file);
            readString(sound, file);

            fread(&dir, sizeof(vec3), 1, file);
            fread(&dist, sizeof(float), 1, file);
            fread(&speed, sizeof(float), 1, file);

            MovingObject* mover = new MovingObject(id, pos, mat, dir, dist, speed, model, sound);
            m_objects.append(mover);

            if (!id.empty()) m_activeObjects[id] = mover;
        }

        if (eclass == EntityClass::Animated)
        {
            std::string id;
            std::string model;
            bool play;

            readString(id, file);
            readString(model, file);

            fread(&play, sizeof(bool), 1, file);

            Animated* animated = new Animated(pos, mat, model, play);

            m_objects.append(animated);

            if (!id.empty()) m_activeObjects[id] = animated;
        }

        if (eclass == EntityClass::Medkit)
        {
            Item* object = new Item(pos, mat, 0.1f, 15.0f, ResourceManager::GetModel("Items/medkit.msh"));

            object->OnPickup.bind([this, object]() {
                pickMedkit(object);
            });

            m_objects.append(object);
        }

        if (eclass == EntityClass::Trooper)
        {
            float rot = atan2(-mat[0].z, mat[0].x);

            Character* actor = new Character(pos, rot, TrooperParams);
            m_objects.append(actor);

            actor->OnDeath.bind_async(m_asyncQueue, [this](Character* character, const vec3& impulse)
            {
                onCharacterDeath(character, impulse);
            });

            actor->setTarget(&m_player);
  
      }

       if (eclass == EntityClass::TechSwitch)
        {
            std::string objectId;
            bool switchOn;
            bool useOnce;

            readString(objectId, file);
            fread(&switchOn, sizeof(bool), 1, file);
            fread(&useOnce, sizeof(bool), 1, file);

            Activator* activator = new Activator(50, pos, mat, 
                                                 ResourceManager::GetModel("Tech/switch01.msh"),
                                                 ResourceManager::GetSound("Tech/switch1.wav"),
                                                 switchOn,
                                                 {0, 2},
                                                 {2, 4},
                                                 useOnce,
                                                 false);

            activator->OnSwitch.bind([this, objectId](bool switchOn)
            {
                activateObject(objectId);
            });

            m_objects.append(activator);
        }

        if (eclass == EntityClass::Vehicle)
        {
            Vehicle* vehicle = new Vehicle(pos, mat, JeepParams, 
                                           ResourceManager::GetModel("Vehicles/jeep.msh"), 
                                           ResourceManager::GetModel("Vehicles/wheel01.msh"),
                                           ResourceManager::GetModel("Vehicles/steer_wheel.msh"),
                                           ResourceManager::GetSound("Vehicle/engine_idle.wav"),
                                           ResourceManager::GetSound("Vehicle/engine_full.wav"),
                                           ResourceManager::GetSound("Vehicle/metal_slam.wav"));

            vehicle->OnMount.bind(this, &Game::mountVehicle);

            m_objects.append(vehicle);
        }

        if (eclass == EntityClass::SteamEmitter)
        {
            std::string id;

            readString(id, file);

            ParticleEmitter* emitter = new ParticleEmitter(pos, mat, 0.45f, 0.07f, 0.1f, 1.8f, 20, ResourceManager::GetImage("Effects/steam.dds"));

            m_objects.append(emitter);

            if (!id.empty()) m_activeObjects[id] = emitter;
        }
    }
}

void Game::loadSurfaces(FILE* file)
{
    while (true)
    {
        SurfaceTopology type;

        fread(&type, 1, sizeof(SurfaceTopology), file);
        if (type == SurfaceTopology::Invalid) break;

        if (type == SurfaceTopology::Rectangle)
        {
            uint16_t xsize;
            uint16_t ysize;

            vec3 pos;

            fread(&xsize, 1, sizeof(uint16_t), file);
            fread(&ysize, 1, sizeof(uint16_t), file);
            fread(&pos, 1, sizeof(vec3), file);
            
            std::vector<Render::Vertex> vertices(xsize * ysize);
            fread(vertices.data(), sizeof(Render::Vertex), vertices.size(), file);

            std::string matname;

            uint16_t mlen;
            fread(&mlen, 1, sizeof(uint16_t), file);
            matname.resize(mlen);

            fread(matname.data(), 1, mlen, file);

            std::vector<Render::SurfaceLayer> layers;

            uint16_t layerNum = 0;
            fread(&layerNum, sizeof(uint8_t), 1, file);

            layers.resize(layerNum);

            for (size_t l = 0; l < layerNum; l++)
            {
                layers[l].mask.resize(xsize * ysize);

                fread(&layers[l].orientation, sizeof(uint8_t), 1, file);

                std::string mname;
                uint16_t tlen;
                fread(&tlen, sizeof(uint16_t), 1, file);

                mname.resize(tlen);

                fread(mname.data(), 1, tlen, file);

                layers[l].material = ResourceManager::GetMaterial(mname);

                fread(layers[l].mask.data(), sizeof(float), xsize * ysize, file);
            }

            std::vector<Render::SurfaceLayerDetails> layerDetails;

            uint16_t layerDetailsNum = 0;
            fread(&layerDetailsNum, sizeof(uint8_t), 1, file);

            layerDetails.reserve(layerDetailsNum);

            for (size_t l = 0; l < layerDetailsNum; l++)
            {
                uint8_t layer;
                float density;

                fread(&layer, sizeof(uint8_t), 1, file);
                fread(&density, sizeof(float), 1, file);

                std::string model;
                std::string material;

                uint16_t strlen;
                fread(&strlen, sizeof(uint16_t), 1, file);
                model.resize(strlen);
                fread(model.data(), 1, strlen, file);

                fread(&strlen, sizeof(uint16_t), 1, file);
                material.resize(strlen);
                fread(material.data(), 1, strlen, file);

                layerDetails.push_back({layer, model, material, density});
            }

            uint16_t cresx = 0;
            uint16_t cresy = 0;

            fread(&cresx, 1, sizeof(uint16_t), file);
            fread(&cresy, 1, sizeof(uint16_t), file);

            std::vector<vec3> clvertices(cresx * cresy);

            if (cresx != 0)
                fread(clvertices.data(), sizeof(vec3), clvertices.size(), file);

            bool layered = layerNum > 0 || layerDetailsNum > 0;

            Render::Surface* surface = layered ? new Render::LayeredSurface(pos, ResourceManager::GetMaterial(matname), xsize, ysize, vertices, layers, layerDetails) :
                                                        new Render::Surface(pos, ResourceManager::GetMaterial(matname), xsize, ysize, vertices);
        
            const BBox& bbox = surface->bbox();

            SurfaceObject* gameObj = new SurfaceObject(pos, bbox, surface, cresx, cresy, clvertices);
            m_staticObjects.append(gameObj);
        }

        if (type == SurfaceTopology::Triangle)
        {
            uint16_t res;

            vec3 pos;

            fread(&res, 1, sizeof(uint16_t), file);
            fread(&pos, 1, sizeof(vec3), file);

            std::vector<Render::Vertex> vertices(res * (res + 1) / 2);
            fread(vertices.data(), sizeof(Render::Vertex), vertices.size(), file);

            std::string matname;

            uint16_t mlen;
            fread(&mlen, 1, sizeof(uint16_t), file);
            matname.resize(mlen);

            fread(matname.data(), 1, mlen, file);

            uint16_t cres = 0;

            fread(&cres, 1, sizeof(uint16_t), file);

            std::vector<vec3> clvertices(cres * (cres + 1) / 2);

            if (cres != 0)
                fread(clvertices.data(), sizeof(vec3), clvertices.size(), file);

            Render::Surface* surface = new Render::Surface(pos, ResourceManager::GetMaterial(matname), res, vertices);

            const BBox& bbox = surface->bbox();

            SurfaceObject* gameObj = new SurfaceObject(pos, bbox, surface, cres, clvertices);
            m_staticObjects.append(gameObj);
        }
    }
}

} //namespace gamelogic