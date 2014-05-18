#include "stdafx.h"
#include "LzpyConsole.h"

namespace Lzpy
{

    bool str1NCmp(std::wstring const & str1, std::wstring const & str2, size_t start, size_t n)
    {
        return false;
    }

    std::wstring relpaceString(std::wstring const & str, std::wstring const & dest, std::wstring const & src)
    {
        size_t n = str.size();
        size_t nDest = dest.size();

        if (n < nDest) return str;

        std::wstring strDest;
        for (size_t i = 0; i <= n - nDest; ++i)
        {
            
        }

        return strDest;
    }

    ConsoleEdit::ConsoleEdit()
    {
        setMutiLine(true);
    }

    ConsoleEdit::~ConsoleEdit()
    {

    }

    bool ConsoleEdit::onSysKey(bool isKeyDown, Lazy::EKEY_CODE key)
    {
        if (!m_parent) return false;

        ConsolePanel *parent = dynamic_cast <ConsolePanel*>(m_parent);
        if (parent->processEditSysKey(isKeyDown, key)) return true;

        return CEdit::onSysKey(isKeyDown, key);
    }

    void ConsoleEdit::clearText()
    {
        m_text.clear();
        m_cursor = 0;
    }




    ConsolePanel::ConsolePanel()
        : m_curCmd(0)
        , m_maxCacheSize(32)
    {
        setBgColor(0x7f000000);
        enableDrag(false);

        m_strResult = L"python console.";

        m_staticLabel.setAlign(Lazy::AlignType::Left);
        m_staticLabel.enableMutiLine(true);
        addChild(&m_staticLabel);

        m_markLabel.create(1, L">>>", 0, 0);
        addChild(&m_markLabel);

        addChild(&m_cmdEdit);

        show(false);
    }

    ConsolePanel::~ConsolePanel()
    {

    }

    void ConsolePanel::setSize(int w, int h)
    {
        CForm::setSize(w, h);

        m_staticLabel.setSize(w, h);
        m_cmdEdit.setSize(w, h);

        this->updateResult();
    }

    bool ConsolePanel::processEditSysKey(bool isKeyDown, DWORD key)
    {
        if (!isKeyDown)
        {
            return true;
        }


        if (key == VK_TAB)
        {
            return true;
        }
        else if (key == VK_UP)
        {

        }
        else if (key == VK_DOWN)
        {

        }
        else if (key == VK_RETURN)
        {
            std::wstring text = m_cmdEdit.getText();
            Lazy::trimStringW(text);

            if (text.empty())
            {
                m_cmdEdit.clearText();
                return true;
            }
            
            if (text.back() != L':')
            {
                m_cmdEdit.clearText();
                doRunScript(text);
                return true;
            }
        }

        return false;
    }

    void ConsolePanel::beginRun()
    {
        if (!m_pyOutput)
        {
            m_pyOutput = new_reference(helper::new_instance<PyConsoleOutput>());
        }

        if (!m_pyLocal)
        {
            m_pyLocal = new_reference(PyDict_New());
        }

        if (!m_pyGlobal)
        {
            PyObject *pyMain = PyImport_AddModule("__main__");
            m_pyGlobal = object( PyModule_GetDict(pyMain) );
        }

        m_pyOutput.cast<PyConsoleOutput>()->m_msg.clear();

        object sys = import("sys");
        m_pyOldStderr = sys.getattr("stderr");
        m_pyOldStdout = sys.getattr("stdout");
        sys.setattr("stderr", m_pyOutput);
        sys.setattr("stdout", m_pyOutput);
    }

    void ConsolePanel::endRun()
    {
        object sys = import("sys");
        sys.setattr("stderr", m_pyOldStderr);
        sys.setattr("stdout", m_pyOldStdout);
    }

    void ConsolePanel::doRunScript(std::wstring const & cmd)
    {
        if (cmd == L"clear")
        {
            m_strResult.clear();
            updateResult();
            return;
        }

        if (cmd == L"exit")
        {
            show(false);
            return;
        }

        beginRun();

        m_cmdCache.push_back(cmd);
        if (m_cmdCache.size() > m_maxCacheSize)
        {
            m_cmdCache.pop_front();
        }

        if (!m_strResult.empty())
            m_strResult.append(L"\n");
        m_strResult.append(L">>>");
        m_strResult.append(cmd);

        std::string cmdAsc;
        Lazy::wcharToChar(cmdAsc, cmd);

        PyObject *pRet = PyRun_StringFlags(cmdAsc.c_str(), Py_single_input,
            m_pyGlobal.get(), m_pyLocal.get(), nullptr);

        if (!pRet)
        {
            if (PyErr_Occurred()) PyErr_Print();
        }
        else
        {
            Py_DECREF(pRet);
        }

        std::wstring & msg = m_pyOutput.cast<PyConsoleOutput>()->m_msg;
        if (!msg.empty())
        {
            //msg.replace(L"\n", L"\n>>>");
            m_strResult.append(L"\n");
            m_strResult.append(msg);
        }

        updateResult();

        endRun();
    }

    void ConsolePanel::updateResult()
    {
        m_staticLabel.setText(m_strResult);

        Lazy::CSize size = m_staticLabel.getTextSize();
        Lazy::CSize size2 = m_markLabel.getTextSize();

        m_markLabel.setPosition(0, size.cy);
        m_cmdEdit.setPosition(size2.cx, size.cy);
    }

    //////////////////////////////////////////////////////////////////
    void registerConsoleOutput()
    {
        LZPY_REGISTER_CLASS_EX(ConsoleOutput, PyConsoleOutput, helper);
    }

    LZPY_CLASS_BEG(PyConsoleOutput);
        LZPY_METHOD_1(write);
        LZPY_METHOD_0(flush);
    LZPY_CLASS_END();

    PyConsoleOutput::PyConsoleOutput()
    {

    }

    LZPY_IMP_INIT(PyConsoleOutput)
    {
        return true;
    }

    LZPY_IMP_METHOD_1(PyConsoleOutput, write)
    {
        std::wstring msg;
        if (parse_object(msg, value))
        {
            m_msg += msg;
            return null_object;
        }

        return none_object;
    }

    LZPY_IMP_METHOD_0(PyConsoleOutput, flush)
    {
        return none_object;
    }

}//end namespace Lzpy