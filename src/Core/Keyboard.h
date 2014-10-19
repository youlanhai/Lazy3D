#pragma once

#include "Base.h"
#include "RenderInterface.h"

namespace Lazy
{

    class LZDLL_API CKeyboard : public IBase, public IRenderable
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

    typedef RefPtr<CKeyboard> KeyboardPtr;

} // end namespace Lazy
