#pragma once

#include <iostream>
#include "i3dobject.h"
#include "Model.h"

namespace Lazy
{
    void writeVector3(LZDataPtr dataPtr, const tstring & tag, const Vector3 & v);
    void readVector3(LZDataPtr dataPtr, const tstring & tag, Vector3 & v);

    typedef RefPtr<class TerrainItem> TerrainItemPtr;
//////////////////////////////////////////////////////////////////////////

    class LZDLL_API TerrainItem : public I3DObject
    {
    public:
        TerrainItem(void);

        ~TerrainItem(void);

        //克隆一个副本
        TerrainItemPtr clone() const;

        virtual void update(float elapse);

        virtual void render(IDirect3DDevice9* pDevice);

        ModelPtr getModel(void) { return m_model; }

        void setModel(ModelPtr model);

        void setChunkId(int id) { m_chunkId = id; }
        int getChunkId() const { return m_chunkId; }

    public:
        const Vector3 & getAngle(void) const { return m_angle; }

        void addLookAngle(float angle);	    //绕Look旋转 roll
        void addUpAngle(float angle);		//绕up旋转 yaw
        void addRightAngle(float angle);	//绕right旋转 pitch

        //更新旋转轴
        void updateRotationAxis(void);

        bool intersectWithCursor();

        bool isActive(void);

        virtual void focusCursor(UINT msg);

        virtual void onFocusCursor(UINT msg);

        float distToCamera(void);

        bool save(LZDataPtr dataPtr) const;
        bool load(LZDataPtr dataPtr);

        const tstring & uuid() const { return m_uuid; }
        bool colid() const { return m_collid; }

        bool isReference() const { return m_isRef; }
        void setReference(bool ref) { m_isRef = ref; }

        size_t getRefChunk() const { return m_refChunk;}
        void setRefChunk(size_t id) { m_refChunk = id; }

        //获取真实的模型矩阵
        void getAbsModelMatrix(Matrix & mat) const;

    protected:
        int         m_chunkId;
        ModelPtr    m_model;
        bool        m_collid;
        tstring m_uuid;
        Vector3     m_angle;

        bool        m_isRef;    //是否是引用自其它chunk
        size_t      m_refChunk; //引用id。
    };

} // end namespace Lazy
