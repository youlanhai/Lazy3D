﻿#pragma once

#ifndef LAZY_PY_WRAPPER_H
#define LAZY_PY_WRAPPER_H

#include "../utility/Object.h"

#include "LzpyObject.h"
#include "LzpyTuple.h"
#include "LzpyStr.h"
#include "LzpyList.h"
#include "LzpyDict.h"

namespace Lzpy
{
    object import(const std::wstring & str);
    object import(const char *name);
    void addSysPath(const std::wstring & path);
    void addSysPaths(const std::vector<std::wstring> & paths);
} // end namespace Lzpy


namespace Lazy
{
    class ScriptObject : public Object
    {
    public:
        ScriptObject()
        {
        }

        ~ScriptObject()
        {}

        virtual Lzpy::object createScriptSelf() const
        { 
            return Lzpy::none_object;
        }

        /** 如果self还没有创建出来，这里会调用createScriptSelf函数进行创建。*/
        Lzpy::object getSelf() const 
        {
            if (m_self.is_null())
                const_cast<ScriptObject*>(this)->m_self = createScriptSelf();
            return m_self;
        }

        Lzpy::object getScriptListener() const
        { 
            return m_scriptListener;
        }

        void setScriptListener(Lzpy::object script)
        {
            m_scriptListener.call_method_quiet("onUnbind", getSelf());
            m_scriptListener = script;
            m_scriptListener.call_method_quiet("onBind", getSelf());
        }

    protected:
        /** 与c++对应的python对象。*/
        Lzpy::object        m_self;

        /** 用于接收消息的python对象。*/
        Lzpy::object        m_scriptListener;
    };

} // end namespace Lazy


#endif //LAZY_PY_WRAPPER_H
