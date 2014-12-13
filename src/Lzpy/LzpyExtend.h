#ifndef LAZY_PY_EXTEN
#define LAZY_PY_EXTEN

#include "LzpyBase.h"

///python交互
namespace Lazy
{
    ///测试导出类
    class PyTestClass : public PyBase
    {
        LZPY_DEF(PyTestClass);
    public:
        PyTestClass();

        LZPY_DEF_METHOD_0(test0);
        LZPY_DEF_METHOD_1(test1);
        LZPY_DEF_METHOD(test2);
        LZPY_DEF_METHOD(hello);
        LZPY_DEF_GET(testId, getTestId);
        LZPY_DEF_SET(testId, setTestId, int);

        int getTestId() { return mydata; }
        void setTestId(int v) { mydata = v; }

        int mydata;
    };


    LZPY_DEF_MODULE(helper);
}


#endif //LAZY_PY_EXTEN