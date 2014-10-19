//Keyboard.cpp
#include "stdafx.h"
#include "Keyboard.h"

namespace Lazy
{

    Keyboard::Keyboard(void)
    {
        memset(m_keyboard, 0, sizeof(m_keyboard));
        memset(m_oldKeyboard, 0, sizeof(m_oldKeyboard));
    }

    Keyboard::~Keyboard(void)
    {
    }

    void Keyboard::update(float)
    {
        memcpy(m_oldKeyboard, m_keyboard, sizeof(m_keyboard));
        GetKeyboardState(m_keyboard);
    }

    bool Keyboard::isKeyDown(DWORD vk)
    {
        if((m_keyboard[vk] & 0x80) != 0 && (m_oldKeyboard[vk] & 0x80) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Keyboard::isKeyPress(DWORD vk)
    {
        if((m_keyboard[vk] & 0x80) != 0 && (m_oldKeyboard[vk] & 0x80) != 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Keyboard::isKeyUp(DWORD vk)
    {
        if((m_keyboard[vk] & 0x80) == 0 && (m_oldKeyboard[vk] & 0x80) != 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

} // end namespace Lazy
