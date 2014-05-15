#pragma once
#include <iostream>
#include "i3dobject.h"
#include "Model.h"

namespace Lazy
{

    void writeVector3(LZDataPtr dataPtr, const Lazy::tstring & tag, const Math::Vector3 & v);
    void readVector3(LZDataPtr dataPtr, const Lazy::tstring & tag, Math::Vector3 & v);

}


struct TerrainResNode
{
    int id;
    int type;
    std::wstring path;

    int getID(){ return id; }

    friend std::istream& operator>>(std::istream & in, TerrainResNode & node)
    {
        std::string path;
        in>>node.id>>node.type>>path;
        Lazy::charToWChar(node.path, path);
        return in;
    }

    bool operator==(const TerrainResNode& node)
    {
        return id == node.id;
    }
};

class LZDLL_API cTerrainRes : public IBase
{
public:
    typedef std::vector<TerrainResNode>     ResContiner;
    typedef ResContiner::iterator           ResIterator;

    static cTerrainRes* instance();

    static void release();

    bool load(const Lazy::tstring & filename);
    
    int getType(int id);

    const std::wstring & getPath(int id);

    TerrainResNode* find(int id);

    ResContiner* getResPool(){ return &m_resPool; }
protected:
    cTerrainRes();
    ~cTerrainRes();

    ResContiner         m_resPool;
    TerrainResNode      m_cache;
    static cTerrainRes   *m_pInstance;
};


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
    
    ModelPtr getModel(void){ return m_model; }

    void setModel(ModelPtr model);

    void setChunkId(int id){ m_chunkId = id; }
    int getChunkId() const { return m_chunkId; }

public:
    const Math::Vector3 & getAngle(void) const { return m_angle; }

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

    bool save(Lazy::LZDataPtr dataPtr) const;
    bool load(Lazy::LZDataPtr dataPtr);

    const Lazy::tstring & uuid() const { return m_uuid; }
    bool colid() const { return m_collid; }
    
    bool isReference() const { return m_isRef; }
    void setReference(bool ref){ m_isRef = ref; }

    size_t getRefChunk() const { return m_refChunk;}
    void setRefChunk(size_t id) { m_refChunk = id; }

    //获取真实的模型矩阵
    void getAbsModelMatrix(Math::Matrix4x4 & mat) const;

public://旧接口
    int getID() const { return m_id; }

    void setModelID(int id);
    int getModelID(void){ return m_modeID; }

    friend std::ostream& operator<<(std::ostream & out, TerrainItem & node );
    friend std::istream& operator>>(std::istream & in, TerrainItem & node );

protected:
    int         m_id;
    int         m_chunkId;
    int         m_modeID;
    ModelPtr    m_model;
    bool        m_collid;
    Lazy::tstring m_uuid;
    Math::Vector3     m_angle;

    bool        m_isRef;    //是否是引用自其它chunk
    size_t      m_refChunk; //引用id。
};
