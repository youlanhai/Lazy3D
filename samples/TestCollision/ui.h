#pragma once

class ui : public CForm
{
public:
    ui(void);

    ui(int id, PControl parent, LPCSTR image, int x, int y, int w, int h);

    ~ui(void);

    void setCollisionTime(int time);

private:
    RefPtr<CLabel> m_title;
    RefPtr<CLabel> m_collisionTime;
};

ui* getUI();