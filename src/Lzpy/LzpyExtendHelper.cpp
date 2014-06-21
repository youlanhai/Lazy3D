#include "stdafx.h"
#include "LzpyExtendHelper.h"

namespace Lzpy
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


        //记录模块的方法信息
        typedef std::vector<PyMethodDef>  ExtenFunArray;
        typedef std::map<std::string, ExtenFunArray>  ExtenFunMap;

        ExtenFunMap * getExtenFunMap()
        {
            static ExtenFunMap s_funMap;
            return &s_funMap;
        }

        ExtenFunArray * getExtenFuns(const std::string & modulename)
        {
            ExtenFunMap *pMap = getExtenFunMap();
            return &((*pMap)[modulename]);
        }


        ///获取模块。如果模块不存在，会自动创建。
        PyObject* getModule(const std::string & modulename)
        {
            static std::map <std::string, PyObject*> s_moduleMap;
            static std::list < PyModuleDef> s_moduleDefHolder;

            //先查看module表中是否存在
            std::map <std::string, PyObject*>::iterator it = s_moduleMap.find(modulename);
            if (it != s_moduleMap.end()) return it->second;

            PyObject * module = nullptr; // PyImport_ImportModule(modulename.c_str());
            if (module)
            {
                s_moduleMap[modulename] = module;
                return module;
            }

            PyErr_Clear();

            //如果module表中不存在，就创建这个module.

            PyModuleDef defModule =
            {
                PyModuleDef_HEAD_INIT,
                cacheName(modulename),
                NULL,
                -1,
                NULL, NULL, NULL, NULL, NULL
            };

            ExtenFunArray *pool = getExtenFuns(modulename);
            if (!pool->empty())
            {
                PyMethodDef def = { NULL, NULL, 0, NULL };
                pool->push_back(def);
                defModule.m_methods = &(*pool)[0];
            }

            //缓存的目的，是防止模块声明对象析构
            s_moduleDefHolder.push_back(defModule);

            module = PyModule_Create(&(s_moduleDefHolder.back()));
            s_moduleMap[modulename] = module;
            return module;
        }

        char* cacheName(const std::string & name)
        {
            static std::list<std::string> s_nameCache;
            s_nameCache.push_back(name);
            std::string & str = s_nameCache.back();
            return &str[0];
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
        Lazy::debugMessageA("Register class to %s.%s", mod_.c_str(), cls_.c_str());
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
        PyMethodDef def = { helper::cacheName(name), fun, meth, doc };
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

        PyMemberDef def = { helper::cacheName(name), type, offset, flags, doc };
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

        PyGetSetDef def = { helper::cacheName(name), get, set, doc, closure };
        getsets_.push_back(def);
    }

    ////////////////////////////////////////////////////////////////////


    void pyExtenFunction(const char *modulename,
                         const char *funname, PyCFunction func)
    {
        pyExtenFunction(modulename, funname, func, METH_VARARGS, "");
    }

    void pyExtenFunction(const char *modulename, const char *funname,
                         PyCFunction func, int argFlag, char *doc)
    {
        Lazy::debugMessageA("Register export function %s.%s", modulename, funname);

        PyMethodDef def = { funname, func, argFlag, doc };

        helper::getExtenFuns(modulename)->push_back(def);
    }

    ///////////////////////////////////////////////////////////////////

    namespace
    {
        typedef std::vector<LzpyResInterface*> ResInterfaces;
        ResInterfaces & resInterfaces()
        {
            static ResInterfaces s_mgr;
            return s_mgr;
        }
    }

    LzpyResInterface::LzpyResInterface()
    {
        resInterfaces().push_back(this);
    }

    LzpyResInterface::~LzpyResInterface()
    {
        ResInterfaces::iterator it = std::find(resInterfaces().begin(), resInterfaces().end(), this);
        resInterfaces().erase(it);
    }

    void LzpyResInterface::initAll()
    {
        ResInterfaces & pool = resInterfaces();
        for (LzpyResInterface * p : pool)
        {
            p->init();
        }
    }

    void LzpyResInterface::finiAll()
    {
        ResInterfaces & pool = resInterfaces();
        for (LzpyResInterface * p : pool)
        {
            p->fini();
        }
    }
}