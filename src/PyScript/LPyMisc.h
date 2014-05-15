#pragma once

namespace LazyPy
{
    class PyEntity;

    class PySkyBox : public PyBase
    {
        LAZYPY_DEF(PySkyBox, PyBase);
    public:
        PySkyBox();
        ~PySkyBox();

        LAZYPY_DEF_METHOD_1(show);
        LAZYPY_DEF_METHOD_0(visible);
        LAZYPY_DEF_METHOD_0(toggle);
        LAZYPY_DEF_METHOD(setRange);

        LAZYPY_DEF_GET_MEMBER(source, m_source);
        LAZYPY_DEF_SET(source, setSource, object);

        LAZYPY_DEF_GET(image, m_sky->getSkyImage);
        LAZYPY_DEF_SET(image, m_sky->setSkyImage, std::wstring);

        void setSource(const object & v);

        RefPtr<cSkyBox> m_sky;
        object_ptr<PyEntity> m_source;
    };


    class PyTopboard : public PyBase
    {
        LAZYPY_DEF(PyTopboard, PyBase);
    public:
        PyTopboard();
        ~PyTopboard();

        LAZYPY_DEF_GET(text, m_topboard->getText);
        LAZYPY_DEF_SET(text, m_topboard->setText, std::wstring);

        LAZYPY_DEF_GET(color, m_topboard->getColor);
        LAZYPY_DEF_SET(color, m_topboard->setColor, DWORD);

        LAZYPY_DEF_GET(biasHeight, m_topboard->getBiasHeight);
        LAZYPY_DEF_SET(biasHeight, m_topboard->setBiasHeight, float);

        LAZYPY_DEF_GET_MEMBER(source, m_source);
        LAZYPY_DEF_SET(source, setSource, object);

        void setSource(const object & v);


        RefPtr<cTopBoard> m_topboard;
        object_ptr<PyEntity> m_source;
    };

    LAZYPY_DEF_MODULE(Lazy);
}