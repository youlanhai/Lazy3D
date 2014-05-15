#pragma once


#include <iostream>

#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>
#include <boost/python/object/function_object.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;

#ifndef RT_VOID
#   define RT_VOID
#endif

#define _VT(name, rt, ...)  \
    if(override func = get_override(#name)) {rt func(__VA_ARGS__);}\
    else {rt T::##name(__VA_ARGS__); }

#define VT(rt, rtType, name) \
    rtType name(void) override { _VT(name, rt); }\
    rtType default_##name(void){ rt T::##name(); }

#define VT1(rt, rtType, name, aType1) \
    rtType name(aType1 a1) override { _VT(name, rt, a1); }\
    rtType default_##name(aType1 a1){ rt T::##name(a1); }

#define VT2(rt, rtType, name, aType1, aType2) \
    rtType name(aType1 a1, aType2 a2) override { _VT(name, rt, a1, a2); }\
    rtType default_##name(aType1 a1, aType2 a2){ rt T::##name(a1, a2); }

//包装宏 rt:返回宏。trType:返回值。name:函数名，aTypei：第i个参数类型
#define VT3(rt, rtType, name, aType1, aType2, aType3) \
    rtType name(aType1 a1, aType2 a2, aType3 a3) override { _VT(name, rt, a1, a2, a3); }\
    rtType default_##name(aType1 a1, aType2 a2, aType3 a3){ rt T::##name(a1, a2, a3); }

namespace boost
{
    namespace python
    {
        template <typename T> 
        struct pointee< RefPtr<T> >
        { 
            typedef T type; 
        };

        template <typename T>
        T* get_pointer(const RefPtr<T> & p)
        {
            return const_cast<T*>(p.get());
        }
    }
}
