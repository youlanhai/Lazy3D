#pragma once

#include "Render/SimpleFrame.h"

namespace Editor
{
    const std::wstring ConfigFile = L"uieditor.lzd";;
}

class SimpleDevice : public Lazy::SimpleFrame
{
public:
    SimpleDevice(void);
    ~SimpleDevice(void);

    void clear() override;

    virtual bool onEvent(const Lazy::SEvent & event) override;

    void updateScriptError();

protected:

    bool onInit() override;
    void onUpdate() override;
    void onRender() override;

    void onSize(int w, int h) override;

public:
    Lazy::RefPtr<Lazy::CGUIManager>     m_guiMgr;
    Lazy::ControlPtr    m_fpsLabel;

};
