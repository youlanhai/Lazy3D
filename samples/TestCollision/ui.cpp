
#include "../lzgui/LZGUI.h"
#include "ui.h"

ui* g_ui = NULL;
ui* getUI()
{
    return g_ui;
}

ui::ui(void)
{
    g_ui = this;
}

ui::ui(int id, PControl parent, LPCSTR image, int x, int y, int w, int h)
    : CForm(id, parent, image, x, y, w, h)
{
    g_ui = this;
    m_title = new CLabel(0, this, "标题呀", 0, 10);
    m_collisionTime = new CLabel(0, this, "", 0, 30);
}

ui::~ui(void)
{
}

void ui::setCollisionTime(int time)
{
    char buffer[64];
    sprintf_s(buffer, 64, "%d ms", time);
    m_collisionTime->setCaption(buffer);
}
