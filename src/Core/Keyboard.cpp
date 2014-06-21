//Keyboard.cpp
#include "stdafx.h"
#include "Keyboard.h"

namespace Lazy
{

    CKeyboard::CKeyboard(void)
    {
        memset(m_keyboard, 0, sizeof(m_keyboard));
        memset(m_oldKeyboard, 0, sizeof(m_oldKeyboard));
    }

    CKeyboard::~CKeyboard(void)
    {
    }

    void CKeyboard::update(float)
    {
        memcpy(m_oldKeyboard, m_keyboard, sizeof(m_keyboard));
        GetKeyboardState(m_keyboard);
    }

    bool CKeyboard::isKeyDown(DWORD vk)
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

    bool CKeyboard::isKeyPress(DWORD vk)
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

    bool CKeyboard::isKeyUp(DWORD vk)
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
