#pragma once


namespace Lazy
{
    struct CUSTOMVERTEX
    {
        FLOAT x, y, z, rhw;
        DWORD color;

        static DWORD FVF;
        static DWORD SIZE;
    };


    class Fps;

    ///简单的app框架
    class SimpleFrame : public Lazy::IEventReceiver
    {
    public:
        SimpleFrame(void);
        ~SimpleFrame(void);

        bool init(HWND hWnd, HINSTANCE hInstance);

        virtual bool onEvent(const Lazy::SEvent & event) override;

        void updateAndRender();

        virtual void clear();

    protected:

        virtual bool onInit();
        virtual void onUpdate();
        virtual void onRender();

        virtual void onSize(int w, int h);

    public:
        HWND        m_hWnd;
        HINSTANCE   m_hInstance;
        Fps         m_fps;
    };

    SimpleFrame * getFrame();

    ///简单的windows窗口
    class SimpleWindow
    {
    public:
        SimpleWindow();
        ~SimpleWindow();

        HWND handel() const { return m_hWnd; }

        void create(HWND hWnd);
        bool create(int w, int h);

    private:
        static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

    private:
        HWND m_hWnd;
    };
}