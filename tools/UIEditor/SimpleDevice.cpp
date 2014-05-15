#include "StdAfx.h"
#include "SimpleDevice.h"

#include "Render/RenderDevice.h"
#include "Render/Texture.h"
#include "PyUI/PyUI.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

const char * gameModule = "MyGame";

namespace Editor
{
    namespace client
    {
        int width = 640;
        int height = 480;
        std::wstring image = L"";
        DWORD color = 0xffffffff;
    }

    struct Vertex
    {
        float x, y, z, rhw;
        DWORD cr;
        float u, v;
    };

    const DWORD VertexFvf = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
    const DWORD VertexSize = sizeof(Vertex);

}

bool initPython()
{
    Lazy::LZDataPtr root = Lazy::openSection(Editor::ConfigFile);

    std::wstring pyHome = root->readString(L"python/home");
    if (pyHome.empty())
    {
        LOG_ERROR(L"Python home was not found in config file!");
        return false;
    }

    Lazy::LZDataPtr paths = root->read(L"python/paths");
    if (!paths)
    {
        LOG_ERROR(L"Python script path was not found in config file!");
        return false;
    }

    Py_SetPythonHome(&pyHome[0]);

    Lzpy::registerConsoleOutput();

    PyImport_AppendInittab("helper", Lzpy::PyInit_helper);
    PyImport_AppendInittab("lui", Lzpy::PyInit_lui);
    PyImport_AppendInittab("lzd", Lzpy::PyInit_lzd);

    Py_Initialize();
    if(!Py_IsInitialized())
    {
        LOG_ERROR(L"Initialize python failed!");
        return false;
    }

    try
    {
        //初始化内置模块
        Lzpy::import("helper");
        Lzpy::import("lzd");
        Lzpy::import("lui");

        Lzpy::LzpyResInterface::initAll();

        //输出重定向
        Lzpy::object sys = Lzpy::import(L"sys");
        if (!sys) return false;

        Lzpy::object out = Lzpy::new_reference(Lzpy::helper::new_instance_ex<Lzpy::PyOutput>());
        sys.setattr("stdout", out);
        sys.setattr("stderr", out);
        PyRun_SimpleString("print('---------1234')");


        //设置脚本路径
        for (Lazy::LZDataPtr lzd : (*paths))
        {
            if (lzd->tag() == L"path")
            {
                LOG_INFO(L"add script path: %s", lzd->cvalue());
                Lzpy::addSysPath(lzd->value());
            }
        }

        //脚本入口
        Lzpy::object mygame = Lzpy::import(gameModule);
        if (!mygame)
        {
            LOG_ERROR(L"Can't find script MyGame.py!");
            return false;
        }

        mygame.call_method("init");
        mygame.call_method("onClientSize", Editor::client::width, Editor::client::height);
    }
    catch(std::exception e)
    {
        XWRITE_LOGA("ERROR: init python failed! %s", e.what());

        if (PyErr_Occurred())
            PyErr_Print();
        return false;
    }

    return true;
}

bool finiPython()
{
    if(!Py_IsInitialized())
    {
        return false;
    }

    try
    {
        Lzpy::object mygame = Lzpy::import(gameModule);
        mygame.call_method("fini");

        Lzpy::LzpyResInterface::finiAll();
    }
    catch(...)
    {
        if (PyErr_Occurred())
            PyErr_Print();
        return false;
    }


    Py_Finalize();
    return true;
}

SimpleDevice::SimpleDevice(void)
{
}


SimpleDevice::~SimpleDevice(void)
{
}

void SimpleDevice::updateScriptError()
{
    if (PyErr_Occurred())
    {
        LOG_ERROR(L"Script Error!");
        PyErr_Print();
    }
}

void SimpleDevice::clear()
{
    finiPython();

    m_guiMgr->destroy();
    m_guiMgr = NULL;


    Lazy::FontMgr::instance()->fini();
}

bool SimpleDevice::onInit()
{
    Lazy::LZDataPtr root = Lazy::openSection(Editor::ConfigFile);
    std::wstring  const & fontFile = root->readString(L"app/font");
    Lazy::FontMgr::instance()->init();
    Lazy::FontMgr::instance()->registerFontFile(fontFile, _T("def"));

    Lazy::rcDevice()->setFontShader(L"shader/font.fx");

    Editor::client::width = root->readInt(L"client/width", 640);
    Editor::client::height = root->readInt(L"client/height", 480);
    Editor::client::image = root->readString(L"client/image");
    Editor::client::color = root->readHex(L"client/color", 0xffffffff);

    m_guiMgr = new Lazy::CGUIManager(Lazy::rcDevice()->getDevice(), m_hWnd, m_hInstance);
    m_guiMgr->setSize(Editor::client::width, Editor::client::height);
    m_guiMgr->getUIRender()->setUIShader(L"shader/ui.fx");
    m_guiMgr->getUIRender()->setFontShader(L"shader/font.fx");

    m_fpsLabel = new Lazy::CLabel();
    m_guiMgr->addChild(m_fpsLabel.get());
    
    if (!initPython())
    {
        LOG_ERROR(L"Initialize python failed!");
        return false;
    }
    return true;
}

void SimpleDevice::onUpdate()
{
    SimpleFrame::onUpdate();

    Lazy::tstring str;
    formatString(str, _T("fps:%0.0f"), m_fps.getFps());
    m_fpsLabel->setText(str);

    m_guiMgr->update(m_fps.getElapse());

    updateScriptError();
}

void SimpleDevice::onRender()
{
    using namespace Editor::client;
#if 0
    //顶点数据
    static Editor::Vertex vertices[] =
    {
        { 0.0f, 0, 1.0f, 1.0f, color, 0.f, 0.f, },
        { width, 0.0f, 1.0f, 1.0f, color, 1.f, 0.f},
        { 0.0f, height, 1.0f, 1.0f, color, 0.f, 1.f},
        { width, height, 1.0f, 1.0f, color, 1.f, 1.f},
    };

    dx::Texture *pTex = Lazy::TextureMgr::instance()->getTexture(image);
    dx::Device * pDevice = Lazy::rcDevice()->getDevice();

    //在后台缓冲区绘制图形
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    pDevice->SetTexture(0, pTex);
    pDevice->SetFVF(Editor::VertexFvf);
    pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, Editor::VertexSize);
#endif

    m_guiMgr->render(Lazy::rcDevice()->getDevice());
}

bool SimpleDevice::onEvent(const Lazy::SEvent & event)
{
    try
    {

        if (m_guiMgr && m_guiMgr->processEvent(event))
        {
            return true;
        }
    }
    catch (Lzpy::python_error e)
    {
        LOG_ERROR(L"Exception in onEvent:%S", e.what());
        updateScriptError();
    }

    return SimpleFrame::onEvent(event);
}

void SimpleDevice::onSize(int w, int h)
{
    SimpleFrame::onSize(w, h);

    Lzpy::object mygame = Lzpy::import(gameModule);
    if (!mygame) return;

    Lazy::CRect rc;
    ::GetClientRect(m_hWnd, &rc);

    mygame.call_method("onSize", rc.width(), rc.height());
}
