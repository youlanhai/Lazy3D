#pragma once

namespace Lzpy
{
    class PyEntity;

    class PySkyBox : public PyBase
    {
        LZPY_DEF(PySkyBox, PyBase);
    public:
        PySkyBox();
        ~PySkyBox();

        LZPY_DEF_METHOD(setRange);

        LZPY_DEF_GET_MEMBER(source, m_source);
        LZPY_DEF_SET(source, setSource, object);

        LZPY_DEF_GET(image, m_sky->getSkyImage);
        LZPY_DEF_SET(image, m_sky->setSkyImage, std::wstring);

        LZPY_DEF_GET(visible, m_sky->visible);
        LZPY_DEF_SET(visible, m_sky->show, bool);


        void setSource(const object & v);
        PyEntity * getPySource();

        RefPtr<cSkyBox> m_sky;
        object_ptr<PyEntity> m_source;
    };


    class PyTopboard : public PyBase
    {
        LZPY_DEF(PyTopboard, PyBase);
    public:
        PyTopboard();
        ~PyTopboard();

        LZPY_DEF_GET(text, m_topboard->getText);
        LZPY_DEF_SET(text, m_topboard->setText, std::wstring);

        LZPY_DEF_GET(color, m_topboard->getColor);
        LZPY_DEF_SET(color, m_topboard->setColor, DWORD);

        LZPY_DEF_GET(biasHeight, m_topboard->getBiasHeight);
        LZPY_DEF_SET(biasHeight, m_topboard->setBiasHeight, float);

        LZPY_DEF_GET_MEMBER(source, m_source);
        LZPY_DEF_SET(source, setSource, object);

        void setSource(const object & v);


        RefPtr<cTopBoard> m_topboard;
        object_ptr<PyEntity>  m_source;
    };

    LZPY_DEF_MODULE(Lazy);
}