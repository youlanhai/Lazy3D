
#include "stdafx.h"

#include "Test.h"
#include "pyWraper.h"
#include "WrapperDataLoader.h"
#include "WrapperUI.h"
#include "WrapperTool.h"
#include "WrapperMisc.h"
#include "WrapperEntity.h"
#include "WrapperMusic.h"

//////////////////////////////////////////////////////////////////////////
//生成导出模块

BOOST_PYTHON_MODULE(Lazy)
{
    LOG_INFO(L"EXPORT START - PyInit_Lazy");

    wrapperTest();

    //包装数据加载器
    wrapperDataLoader();

    //包装工具
    wrapperTool();

    //包装音乐
    wrapperMusic();

    //包装UI
    wrapperUI();

    //包装实体
    wrapperEntity();

    //包装杂项
    wrapperMisc();

    LOG_INFO(L"EXPORT END - PyInit_Lazy");

} 
