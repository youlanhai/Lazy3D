#include "stdafx.h"
#include "UISprite.h"

#include "UIWidget.h"

namespace Lazy
{

    ////////////////////////////////////////////////////////////////////

    TextureSprite::TextureSprite()
    {}

    TextureSprite::~TextureSprite()
    {}

    void TextureSprite::update(float elapse)
    {}

    void TextureSprite::render(IUIRender * pDevice, const CPoint & world)
    {
        CRect rc = getRect();
        rc.offset(world.x, world.y);

        pDevice->drawRect(rc, m_srcRect, 0xffffffff, m_texture);
    }

    void TextureSprite::setTexture(TexturePtr tex, const UVRect & rect)
    {
        m_texture = tex;
        m_srcRect = rect;

        if (m_texture)
            m_size = m_texture->getSize();
        else
            m_size.set(0, 0);
    }

    void TextureSprite::setUVRect(const UVRect & rc)
    {
        m_srcRect = rc;
    }

    ////////////////////////////////////////////////////////////////////
    ImageSprite::ImageSprite()
    {
    }


    ImageSprite::~ImageSprite()
    {
    }

    void ImageSprite::setImage(const tstring & image)
    {
        m_image = image;
        setTexture(TextureMgr::instance()->get(image));
    }

    ////////////////////////////////////////////////////////////////////
    ControlSprite::ControlSprite()
    {}

    ControlSprite::~ControlSprite()
    {}

    //ui控件有自己的update方法和render方法，这里不在做逻辑，只是调整坐标。
    void ControlSprite::update(float elapse)
    {}

    void ControlSprite::render(IUIRender * pDevice, const CPoint & world)
    {
        if (!m_control || !m_control->getParent()) return;

        CPoint pt = m_position + world;
        m_control->setAbsPosition(pt.x, pt.y);
    }

    ////////////////////////////////////////////////////////////////////

    WordSprite::WordSprite(bool grouped)
        : m_ch(0)
        , m_color(0xff000000)
        , m_grouped(grouped)
    {}

    WordSprite::~WordSprite()
    {}

    void WordSprite::setWord(wchar_t ch, FontPtr font, uint32 color)
    {
        m_ch = ch;
        m_color = color;

        WordInfo info;
        if (!font->getWord(ch, info))
        {
            m_texture = nullptr;
        }
        else
        {
            m_texture = info.texture;
            m_uvRect.set(info.x1, info.y1, info.x2, info.y2);

            m_size.set(info.ax, info.ay);
            m_offsetPos.set(info.dx, info.dy);
            m_offsetSize.set(info.dw, info.dh);
        }
    }

    void WordSprite::render(IUIRender * pDevice, const CPoint & world)
    {
        if (m_ch == '\n') return;
        if (!m_texture) return;

        CPoint pt = m_position + world + m_offsetPos;

        pDevice->drawWord(CRect(pt, m_offsetSize), m_uvRect, m_color, m_texture);
    }


    ////////////////////////////////////////////////////////////////////
    ITextSprite::ITextSprite()
        : m_layoutDirty(false)
        , m_color(0xff000000)
        , m_maxWidth(100)
        , m_lineSpace(4)
    {}

    ITextSprite::~ITextSprite()
    {}

    void ITextSprite::setText(const std::wstring & text, FontPtr font)
    {
        clear();

        m_text = text;
        m_layoutDirty = true;
    }

    void ITextSprite::insertText(size_t pos, const std::wstring & text, FontPtr font)
    {
        if (pos > m_text.length()) pos = m_text.length();

        for (size_t i = 0; i < text.length(); ++i, ++pos)
        {
            insertWord(pos, text[i], font);
        }

        m_layoutDirty = true;
    }

    void ITextSprite::delText(size_t pos, size_t count)
    {
        if (m_text.empty()) return;
        if (pos >= m_text.length()) return;

        for (size_t i = 0; i < count; ++i)
        {
            delWord(pos + count - i);
        }

        m_layoutDirty = true;
    }

    void ITextSprite::update(float elapse)
    {
        if (m_layoutDirty)
            layout();
    }

    void ITextSprite::clear()
    {
        m_layoutDirty = false;
        m_size.set(0, 0);
        m_text.clear();

        onClear();
    }

    void ITextSprite::layout()
    {
        if (!m_layoutDirty) return;

        m_layoutDirty = false;

        onLayout();
    }

    void ITextSprite::setMaxWidth(int width)
    {
        if (m_maxWidth == width) return;

        m_maxWidth = width;
        m_layoutDirty = true;
    }

    void ITextSprite::setLineSpace(int space)
    {
        if (m_lineSpace == space) return;

        m_lineSpace = space;
        m_layoutDirty = true;
    }

    ////////////////////////////////////////////////////////////////////
    TextLineSprite::TextLineSprite(bool grouped)
        : m_grouped(grouped)
    {

    }

    TextLineSprite::~TextLineSprite()
    {

    }

    void TextLineSprite::setText(const tstring & text, FontPtr font)
    {
        clear();

        for (size_t i = 0; i < text.size(); ++i)
        {
            insertWord(i, text[i], font);
        }
    }

    void TextLineSprite::setColor(uint32 color)
    {
        if (color == m_color) return;

        ITextSprite::setColor(color);

        for (std::vector<WordSpritePtr>::iterator it = m_sprites.begin();
                it != m_sprites.end(); ++it)
        {
            (*it)->setColor(color);
        }
    }

    void TextLineSprite::insertWord(size_t pos, wchar_t ch, FontPtr font)
    {
        WordSprite *pSprite = new WordSprite(true);
        pSprite->setWord(ch, font, m_color);

        insertWord(pos, pSprite);
    }

    void TextLineSprite::insertWord(size_t pos, WordSpritePtr pSprite)
    {
        if (pos > m_text.length()) pos = m_text.length();

        m_text.insert(pos, 1, pSprite->getWord());
        m_sprites.insert(m_sprites.begin() + pos, pSprite);

        m_layoutDirty = true;
    }

    void TextLineSprite::delWord(size_t pos)
    {
        if (m_text.empty()) return;
        if (pos >= m_text.length()) return;

        m_text.erase(pos, 1);
        m_sprites.erase(m_sprites.begin() + pos);

        m_layoutDirty = true;
    }

    void TextLineSprite::onClear()
    {
        m_sprites.clear();
    }

    void TextLineSprite::onLayout()
    {
        m_size.set(0, 0);

        for (std::vector<WordSpritePtr>::iterator it = m_sprites.begin();
                it != m_sprites.end(); ++it)
        {
            m_size.cy = max(m_size.cy, (*it)->getSize().cy);
        }

        for (std::vector<WordSpritePtr>::iterator it = m_sprites.begin();
                it != m_sprites.end(); ++it)
        {
            const CSize & sz = (*it)->getSize();
            (*it)->setPosition(m_size.cx, m_size.cy - sz.cy);//底对齐
            m_size.cx += sz.cx;
        }
    }

    void TextLineSprite::render(IUIRender * pDevice, const CPoint & world)
    {
        CPoint pt = m_position + world;

        for (std::vector<WordSpritePtr>::iterator it = m_sprites.begin();
                it != m_sprites.end(); ++it)
        {
            (*it)->render(pDevice, pt);
        }
    }

    bool TextLineSprite::getCursorByPos(size_t & cursor, int x, int)
    {
        layout();

        int width = 0;
        cursor = 0;

        for (WordSpritePtr & spr : m_sprites)
        {
            if (x <= width) return true;

            width += spr->getSize().cx;
            ++cursor;
        }

        return true;
    }

    void TextLineSprite::getCursorPos(size_t cursor, int & x, int & y)
    {
        layout();

        x = y = 0;
        if (m_sprites.empty() || cursor < 1) return;

        WordSpritePtr spr;
        if (cursor <= m_sprites.size()) spr = m_sprites[cursor - 1];
        else spr = m_sprites.back();

        x = spr->getPosition().x + spr->getSize().cx;
    }

    ////////////////////////////////////////////////////////////////////
    TextViewSprite::TextViewSprite()
    {
    }

    TextViewSprite::~TextViewSprite()
    {

    }

    TextLineSpritePtr TextViewSprite::createLineSprite()
    {
        TextLineSpritePtr p = new TextLineSprite(true);
        p->setColor(m_color);
        p->setLineSpace(m_lineSpace);
        p->setMaxWidth(m_maxWidth);
        return p;
    }

    void TextViewSprite::setText(const std::wstring & text, FontPtr font)
    {
        ITextSprite::setText(text, font);

        if (text.empty())
            return;

        TextLineSpritePtr curLine = createLineSprite();
        m_lineSprites.push_back(curLine);

        for (size_t i = 0; i < text.size(); ++i)
        {
            curLine->insertWord(curLine->length(), text[i], font);

            if (text[i] == '\n' && i + 1 < text.size())
            {
                curLine = createLineSprite();
                m_lineSprites.push_back(curLine);
            }
        }
        m_layoutDirty = true;
    }

    void TextViewSprite::setColor(uint32 color)
    {
        if (color == m_color) return;

        ITextSprite::setColor(color);

        for (std::vector<TextLineSpritePtr>::iterator it = m_lineSprites.begin();
                it != m_lineSprites.end(); ++it)
        {
            (*it)->setColor(color);
        }
    }

    void TextViewSprite::insertWord(size_t pos, wchar_t ch, FontPtr font)
    {
        if (pos > m_text.size()) return;

        m_text.insert(pos, 1, ch);

        if (m_lineSprites.empty())
            m_lineSprites.push_back(createLineSprite());

        for (TextLineSpritePtr & spr : m_lineSprites)
        {
            if (pos <= spr->length())
            {
                spr->insertWord(pos, ch, font);
                break;
            }
            pos -= spr->length();
        }

        m_layoutDirty = true;
    }


    void TextViewSprite::delWord(size_t pos)
    {
        if (pos >= m_text.size()) return;

        for (std::vector<TextLineSpritePtr>::iterator it = m_lineSprites.begin();
                it != m_lineSprites.end(); ++it)
        {
            if (pos <= (*it)->length())
            {
                (*it)->delWord(pos);
                break;
            }
            pos -= (*it)->length();
        }

        m_layoutDirty = true;
    }


    void TextViewSprite::onClear()
    {
        m_lineSprites.clear();
    }

    void TextViewSprite::onLayout()
    {
        size_t i = 0;
        while (i < m_lineSprites.size())
        {
            TextLineSpritePtr line = m_lineSprites[i];
            line->layout();

            size_t textLen = line->length();
            if (textLen == 0)//删除空行
            {
                m_lineSprites.erase(m_lineSprites.begin() + i);
                continue;
            }

            if (textLen == 1)//每行至少要有一个字符
            {
                ++i;
                continue;
            }

            if(line->getSize().cx > m_maxWidth)//TODO 优化这里的分割算法
            {
                WordSpritePtr lastSprite = line->getSprite(textLen - 1);
                //移除末尾文字
                line->delWord(textLen - 1);

                if (i + 1 >= m_lineSprites.size())
                    m_lineSprites.push_back(createLineSprite());

                //插到下一行的行首
                m_lineSprites[i + 1]->insertWord(0, lastSprite);

                continue;
            }
            else if (line->getSize().cx < m_maxWidth)
            {
                WordSpritePtr lastSprite = line->getSprite(textLen - 1);
                if (lastSprite->getWord() != '\n')//末尾不是换行符，可能需要从下一行借一个
                {
                    TextLineSpritePtr nextLine;

                    //由于上一行的借位，可能会导致下一行为空行，需要删除空行重新排版。
                    while (!nextLine && i + 1 < m_lineSprites.size())
                    {
                        if (m_lineSprites[i + 1]->length() == 0)
                        {
                            m_lineSprites.erase(m_lineSprites.begin() + i);
                        }
                        else
                        {
                            nextLine = m_lineSprites[i + 1];
                            break;
                        }
                    }

                    if (!nextLine)//没有后续行了
                        break;

                    WordSpritePtr firstSprite = nextLine->getSprite(0);
                    if (line->getSize().cx + firstSprite->getSize().cx < m_maxWidth)
                    {
                        nextLine->delWord(0);
                        line->insertWord(textLen, firstSprite);
                        continue;
                    }
                }
            }

            ++i;
        }


        m_size.set(0, 0);
        for (std::vector<TextLineSpritePtr>::iterator it = m_lineSprites.begin();
                it != m_lineSprites.end(); ++it)
        {
            (*it)->setPosition(0, m_size.cy);
            m_size.cx = max(m_size.cx, (*it)->getSize().cx);
            m_size.cy += (*it)->getSize().cy + m_lineSpace;
        }
    }

    void TextViewSprite::render(IUIRender * pDevice, const CPoint & world)
    {
        CPoint pt = m_position + world;

        for (std::vector<TextLineSpritePtr>::iterator it = m_lineSprites.begin();
                it != m_lineSprites.end(); ++it)
        {
            (*it)->render(pDevice, pt);
        }
    }

    bool TextViewSprite::getCursorByPos(size_t & cursor, int x, int y)
    {
        layout();

        cursor = 0;
        if (m_lineSprites.empty()) return true;

        int height = 0;
        size_t lines = 0;
        for (TextLineSpritePtr & spr : m_lineSprites)
        {
            if (y <= height + spr->getSize().cy) break;

            height += spr->getSize().cy;
            cursor += spr->length();
            ++lines;
        }
        if (lines >= m_lineSprites.size()) return true;

        size_t cursor2;
        m_lineSprites[lines]->getCursorByPos(cursor2, x, y);
        cursor += cursor2;
        return true;
    }

    void TextViewSprite::getCursorPos(size_t cursor, int & x, int & y)
    {
        layout();

        x = y = 0;
        if (m_lineSprites.empty()) return;

        for (TextLineSpritePtr & spr : m_lineSprites)
        {
            if (cursor <= spr->length())
            {
                spr->getCursorPos(cursor, x, y);
                y = spr->getPosition().y;

                return;
            }
            cursor -= spr->length();
        }
    }
}