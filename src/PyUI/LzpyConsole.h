#pragma once

#include <queue>

namespace Lzpy
{
    class ConsoleEdit : public Lazy::CEdit
    {
    public:
        ConsoleEdit();
        ~ConsoleEdit();

        virtual bool onSysKey(bool isKeyDown, Lazy::EKEY_CODE key) override;

        void clearText();
    };

    class ConsolePanel : public Lazy::CForm
    {
    public :
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
        Lazy::CLabel m_staticLabel;
        Lazy::CLabel m_markLabel;
        ConsoleEdit  m_cmdEdit;

        std::wstring m_strResult;
        std::deque<std::wstring> m_cmdCache;
        size_t m_maxCacheSize;
        size_t m_curCmd;

        object m_pyLocal;
        object m_pyGlobal;
        object m_pyOutput;
        object m_pyOldStderr;
        object m_pyOldStdout;
    };



    class PyConsoleOutput : public PyBase
    {
        LZPY_DEF(PyConsoleOutput, PyBase);
    public:
        PyConsoleOutput();

        LZPY_DEF_METHOD_1(write);
        LZPY_DEF_METHOD_0(flush);

        std::wstring m_msg;
    };

    void registerConsoleOutput();

}//end namespace Lzpy