#pragma once

#include "Base.h"
#include "RenderInterface.h"

namespace Lazy
{

    class LZDLL_API Keyboard : public IBase, public IRenderable
    {
    public:
        Keyboard(void);
        ~Keyboard(void);

        virtual void update(float fElapse);

        bool isKeyDown(DWORD vk);
        bool isKeyPress(DWORD vk);
        bool isKeyUp(DWORD vk);

        BYTE* getKeyboard(void) { return m_keyboard; }
    private:
        BYTE	m_keyboard[256];
        BYTE	m_oldKeyboard[256];
    };

    typedef RefPtr<Keyboard> KeyboardPtr;

} // end namespace Lazy
