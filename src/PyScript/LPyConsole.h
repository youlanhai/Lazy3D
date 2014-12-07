#pragma once

#include "../Lzui/UIEdit.h"
#include "../Lzui/UIWindow.h"

#include "../utility/UtilConfig.h"

#include <queue>

namespace Lzpy
{
    class PyConsoleOutput;

    using Lazy::Widget;
    using Lazy::Edit;
    using Lazy::Window;

    class ConsoleEdit : public Edit
    {
    public:
        //MAKE_UI_HEADER(ConsoleEdit, Edit)

        ConsoleEdit();
        ~ConsoleEdit();

        virtual bool onSysKey(bool isKeyDown, Lazy::EKEY_CODE key) override;

        void clearText();
    };

    class ConsolePanel : public Window
    {
    public :
        //MAKE_UI_HEADER(ConsolePanel, Window)

        ConsolePanel();
        ~ConsolePanel();

        void setSize(int w, int h) override;

        bool processEditSysKey(bool isKeyDown, DWORD key);

    private:

        void beginRun();
        void endRun();
        void doRunScript(std::wstring const & cmd);

        void updateResult();

    private:
        Lazy::Label m_staticLabel;
        Lazy::Label m_markLabel;
        ConsoleEdit  m_cmdEdit;

        std::wstring m_strResult;
        std::deque<std::wstring> m_cmdCache;
        size_t m_maxCacheSize;
        size_t m_curCmd;

        object m_pyLocal;
        object m_pyGlobal;
        object m_pyOldStderr;
        object m_pyOldStdout;

        object_ptr<PyConsoleOutput> m_pyOutput;
    };



    class PyConsoleOutput : public PyBase
    {
        LZPY_DEF(PyConsoleOutput);
    public:
        PyConsoleOutput();

        LZPY_DEF_METHOD_1(write);
        LZPY_DEF_METHOD_0(flush);

        std::wstring m_msg;
    };

}//end namespace Lzpy