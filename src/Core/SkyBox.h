/**********************************************************
 SkyBox.h（天空盒子类）
 作用: 创建天空盒子
 **********************************************************/
#pragma once

#include "Base.h"
#include "RenderObj.h"

namespace Lazy
{
    class SceneNode;

    class LZDLL_API SkyBox : public IBase, public IRender
    {
    public:
        SkyBox();

        ~SkyBox(void);

        /** 设置天空纹理。
            @param : texNames，纹理名称。格式为name%d.xxx。图片顺序为(z+为北):北东南西顶底。*/
        void setSkyImage(const std::wstring & texNames) { m_texNames = texNames; }
        const std::wstring & getSkyImage() const { return m_texNames; }

        /** 设置天空范围。*/
        void setSkyRange(const Vector3 & min_, const Vector3 & max_);

        void render(IDirect3DDevice9 *pDevice);

        void setSource(SceneNode* pSource) { m_pSource = pSource; }

        SceneNode* getSource(void) { return m_pSource; }

        void show(bool s) { m_visible = s; }

        bool visible() const { return m_visible; }

        void toggle() { show(!visible()); }

    protected:

        ///创建顶点
        bool create(IDirect3DDevice9 *pDevice);

        void fillVertext();

    private:
        SceneNode*              m_pSource;
        std::wstring            m_texNames;
        IDirect3DVertexBuffer9* m_pVertexBuffer;
        Vector3                 m_min;
        Vector3                 m_max;
        bool                    m_visible;
    };


} // end namespace Lazy
