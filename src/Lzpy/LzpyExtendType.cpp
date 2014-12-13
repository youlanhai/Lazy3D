#include "stdafx.h"

#include "LzpyExtendType.h"
#include "LzpyExtendHelper.h"

namespace Lazy
{
    namespace helper
    {
        //记录模块的类信息
        typedef std::vector<PyExtenInterface*> ExtenClassArray;
        typedef std::map<std::string, ExtenClassArray> ExtenClassMap;

        ExtenClassMap * getExtenClassMap()
        {
            static ExtenClassMap s_extenMap;
            return &s_extenMap;
        }

        ExtenClassArray* getExtenClasses(const std::string & modulename)
        {
            ExtenClassMap * pMap = getExtenClassMap();
            return &((*pMap)[modulename]);
        }

        PyObject* extenModule(const std::string & modulename)
        {
            ExtenClassArray * pool = getExtenClasses(modulename);
            for (PyExtenInterface * it : (*pool))
            {
                it->extenMethod();
            }

            return helper::getModule(modulename);
        }
    }


    PyExtenInterface::PyExtenInterface()
    {
    }

    PyExtenInterface::~PyExtenInterface()
    {
    }

    void PyExtenInterface::registerForExten(const std::string & mod, const std::string & cls)
    {
        mod_ = mod;
        cls_ = cls;

#ifdef ENABLE_EXPORT_MSG
        //Lazy::debugMessageA("Register class to %s.%s", mod_.c_str(), cls_.c_str());
#endif

        helper::getExtenClasses(mod_)->push_back(this);
    }

    void PyExtenInterface::addMethod(const char* name, PyCFunction fun)
    {
        addMethod(name, fun, METH_VARARGS, "");
    }

    ///添加类方法
    void PyExtenInterface::addMethod(const char *name, PyCFunction fun, int meth, const char *doc)
    {
        PyMethodDef def = { name, fun, meth, doc };
        methods_.push_back(def);
    }


    void PyExtenInterface::addMember(const char *name, int type, Py_ssize_t offset)
    {
        addMember(name, type, offset, 0, "");
    }

    ///添加类成员变量
    void PyExtenInterface::addMember(const char *name, int type, Py_ssize_t offset, int flags, char *doc)
    {
        assert(name);

        PyMemberDef def = { const_cast<char*>(name), type, offset, flags, doc };
        members_.push_back(def);
    }

    void PyExtenInterface::addGetSet(const char *name, getter get, setter set)
    {
        addGetSet(name, get, set, nullptr, nullptr);
    }

    ///添加类属性
    void PyExtenInterface::addGetSet(const char *name, getter get, setter set, char *doc, void *closure)
    {
        assert(name);

        PyGetSetDef def = { const_cast<char*>(name), get, set, doc, closure };
        getsets_.push_back(def);
    }

} // end namespace Lazy
