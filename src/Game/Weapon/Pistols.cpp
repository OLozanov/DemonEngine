#include "Pistols.h"
#include "Render/SceneManager.h"
#include "Resources/Resources.h"

#include "System/AudioManager.h"

namespace GameLogic
{

Pistols::Pistols()
: m_right(ResourceManager::GetModel("Weapon/pistol_right.msh"))
, m_left(ResourceManager::GetModel("Weapon/pistol_left.msh"))
, m_shotSound(ResourceManager::GetSound("Weapon/pistol.wav"))
, m_state(State::IdleRight)
, m_pending(false)
{
    m_right.setMat(mat4::Translate({ 0.2, -0.12, 0.25 }) * mat4::RotateY(-math::pi * 0.5));
    m_left.setMat(mat4::Translate({ -0.2, -0.12, 0.25 }) * mat4::RotateY(-math::pi * 0.5));

    m_right.setRepeat(false);
    m_left.setRepeat(false);

    m_right.setFrameRate(AnimSpeed);
    m_left.setFrameRate(AnimSpeed);

    m_right.setTime(6 * AnimSpeed);
    m_left.setTime(6 * AnimSpeed);
}

void Pistols::equip()
{
    m_state = State::IdleRight;

    Render::SceneManager& sm = Render::SceneManager::GetInstance();
    sm.addOverlay(&m_right);
    sm.addOverlay(&m_left);

    m_right.stop();
    m_left.stop();

    m_right.setTime(6 * AnimSpeed);
    m_left.setTime(6 * AnimSpeed);
}

void Pistols::unequip()
{

}

void Pistols::fire()
{
    switch (m_state)
    {
    case State::IdleRight:
        m_right.run(0);
        AudioManager::Play(m_shotSound);
        m_state = State::FireRight;
        m_fire = false;
    break;
    case State::IdleLeft:
        m_left.run(0);
        AudioManager::Play(m_shotSound);
        m_state = State::FireLeft;
        m_fire = false;
    break;
    default:
        m_pending = true;
    }
}

void Pistols::stopFire()
{

}

void Pistols::update(float dt)
{
    switch (m_state)
    {
    case State::FireRight:
        m_right.animate(dt);
        if (!m_right.isRunning())
        {
            if (m_pending)
            {
                m_left.run(0);
                AudioManager::Play(m_shotSound);
                m_state = State::FireLeft;
                m_pending = false;
                m_fire = false;
            }
            else 
                m_state = State::IdleLeft;
        }
        else
        {
            float time = m_right.getAnimTime();
            bool fireTime = (time > 0 && time < AnimSpeed);

            if (fireTime && !m_fire)
            {
                m_fire = true;
                onAttack();
            }
        }
    break;
    case State::FireLeft:
        m_left.animate(dt);
        if (!m_left.isRunning())
        {
            if (m_pending)
            {
                m_right.run(0);
                AudioManager::Play(m_shotSound);
                m_state = State::FireRight;
                m_pending = false;
                m_fire = false;
            }
            else
            m_state = State::IdleRight;
        }
        else
        {
            float time = m_left.getAnimTime();
            bool fireTime = (time > 0 && time < AnimSpeed);

            if (fireTime && !m_fire)
            {
                m_fire = true;
                onAttack();
            }
        }
    break;
    }
}

} // namespace gamelogic