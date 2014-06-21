#pragma once

#include "Base.h"
#include "RenderObj.h"

namespace Lazy
{

    class LZDLL_API CKeyboard : public IUpdate, public IBase
    {
    public:
        CKeyboard(void);

        ~CKeyboard(void);

        virtual void update(float fElapse);

        bool isKeyDown(DWORD vk);

        bool isKeyPress(DWORD vk);

        bool isKeyUp(DWORD vk);

        BYTE* getKeyboard(void) { return m_keyboard; }
    private:
        BYTE	m_keyboard[256];
        BYTE	m_oldKeyboard[256];
    };

} // end namespace Lazy
