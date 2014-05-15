#pragma once

#include <string>
#include <boost/python.hpp>
#include "Base.h"

class cPyScript : public IBase
{
public:
    cPyScript(void);
    ~cPyScript(void);

    bool init(void);

    void release(void);
private:
    std::string m_moduleName;
    boost::python::object m_mainModule;
    boost::python::object m_mainNamespace;
    boost::python::object m_gui;
};
