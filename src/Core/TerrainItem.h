#pragma once

#include <iostream>
#include "i3dobject.h"
#include "Model.h"

namespace Lazy
{
    void writeVector3(LZDataPtr dataPtr, const tstring & tag, const Vector3 & v);
    void readVector3(LZDataPtr dataPtr, const tstring & tag, Vector3 & v);

    class TerrainItem;
    class TerrainChunk;
    typedef RefPtr<TerrainItem> TerrainItemPtr;
    
    /** 表示了地图上的一个物件。*/
    class LZDLL_API TerrainItem : public I3DObject
    {
    public:
        explicit TerrainItem(uint32 id);
        ~TerrainItem(void);

        uint32 getID() const { return m_id; }

        virtual void update(float elapse);
        virtual void render(IDirect3DDevice9* pDevice);

        ModelPtr getModel(void) { return m_model; }
        void setModel(ModelPtr model);

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

        bool colid() const { return m_isCollid; }

        //获取真实的模型矩阵
        void getAbsModelMatrix(Matrix & mat) const;

        //{don't call the following method, unless you know what you did.
        /** 从chunk中删除自己。*/
        void removeFromChunks();
        void addChunk(TerrainChunk *pChunk);
        //}

    private:
        uint32      m_id;
        ModelPtr    m_model;
        bool        m_isCollid;
        Vector3     m_angle;

        /** item所属的chunk。一个item可能横跨多个chunk。*/
        std::vector<TerrainChunk*>  m_chunks;
    };

} // end namespace Lazy
