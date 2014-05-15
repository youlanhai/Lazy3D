#include "PyScript.h"
#include "pyWraper.h"

using namespace boost::python;


cPyScript::cPyScript(void)
{
    Py_Initialize();
}

cPyScript::~cPyScript(void)
{
}


bool cPyScript::init(void)
{
    try
    {
        m_mainModule = import("__main__");
        m_mainNamespace = m_mainModule.attr("__dict__");
        initLazy();

        exec("import Lazy", m_mainNamespace);
        exec("Lazy.WRITE_LOG('这是来自脚本的输出')", m_mainNamespace);
        handle<>  hand =  handle<>(PyImport_ImportModule("Lazy"));
        call_method<void>(hand.get(), "WRITE_LOG", "这也是来自脚本的输出");
     
//         exec("import gameScript", m_mainNamespace, m_mainNamespace);
//         exec("gameScript.initScript()", m_mainNamespace, m_mainNamespace);

//         exec("data = gameScript.getData()", m_mainNamespace);
//         dict d = extract<dict>(m_mainNamespace["data"]);
//         std::string s = extract<std::string>(d[1]);
    }
    catch(...)
    {
        if (PyErr_Occurred())
            PyErr_Print();
        return false;
    }
    return true;
}


void cPyScript::release(void)
{
    if (m_mainNamespace)
    {
        try
        {
            exec("gameScript.releaseScript()", m_mainNamespace);
        }
        catch(...)
        {
            if (PyErr_Occurred())
                PyErr_Print();
        }
    }
}