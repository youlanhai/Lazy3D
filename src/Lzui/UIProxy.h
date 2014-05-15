#pragma once

namespace Lazy
{
    ///维持了一个外部的ui
    class UIProxy : public IControl
    {
    public:
        MAKE_UI_HEADER(UIProxy, uitype::Proxy);

        UIProxy();
        ~UIProxy();

        virtual bool loadFromStream(LZDataPtr config) override;
        virtual bool saveToStream(LZDataPtr config) override;

        bool loadHost(const tstring & layoutFile);

        ControlPtr getHost() const { return m_host; }

    private:
        tstring     m_layoutFile;
        ControlPtr  m_host;
    };

}//end namespace Lazy