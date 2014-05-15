
#include "stdafx.h"
#include "BSP.h"
#include "PhysicsDebug.h"
#include "PhysicsUtil.h"

namespace Physics
{
    const size_t BspMagic = Lazy::makeMagic('b', 's', 'p', 'x');
    const size_t BspVersion = 0;

    const float MAX_FLOAT_LOWER = MAX_FLOAT - 0.001f;

    //bsp文件的存贮路径
    static Lazy::tstring BspFilePath = _T("bsp/");
    
    void setBspFilePath(const Lazy::tstring & path)
    {
        BspFilePath = path;
    }

    const Lazy::tstring & getBspFilePath()
    {
        return BspFilePath;
    }

    float min3(float a, float b, float c)
    {
        if(a < b) return min2(a, c);
        else return min2(b, c);
    }

    float max3(float a, float b, float c)
    {
        if(a > b) return max2(a, c);
        else return max2(b, c);
    }

    //////////////////////////////////////////////////////////////////////////
    ///bsp
    //////////////////////////////////////////////////////////////////////////

    BspNode::BspNode(const Physics::Triangle & tri)
        : m_panel(tri)
    {
        m_triangleSet.push_back(tri);
    }

    void BspNode::addTriangle(const Physics::Triangle & tri, 
        const Physics::Polygon & splitPoly)
    {
        Physics::Plane panel(tri);
        if(panel == m_panel)
        {
            m_triangleSet.push_back(tri);
            return;
        }

        Physics::Polygon poly;
        if (!splitPoly.empty())
        {
            poly = splitPoly;
        }
        else
        {
            poly.addTriangle(tri);
        }

        Physics::Polygon fontP, backP;
        poly.split(m_panel, fontP, backP);
        if (!fontP.empty())
        {
            addToChild(m_front, tri, fontP);
        }
        if (!backP.empty())
        {
            addToChild(m_back, tri, backP);
        }
    }

    bool BspNode::collision(const Physics::Triangle & triangle,
        const Physics::Vector3 & offset,
        Physics::CollisionPrevent & ci, 
        const Matrix4x4 & world) const
    {
        //Physics::addDebugDrawTriangle(m_triangleSet[0], 0x3fffffff);

        float epsilon = 0.00001f;

        float da0 = m_panel.distToPoint(triangle.a);
        float da1 = m_panel.distToPoint(triangle.b);
        float da2 = m_panel.distToPoint(triangle.c);

        float db0 = m_panel.distToPoint(triangle.a + offset);
        float db1 = m_panel.distToPoint(triangle.b + offset);
        float db2 = m_panel.distToPoint(triangle.c + offset);

        float minD = min2(min3(da0, da1, da2), min3(db0, db1, db2));
        float maxD = max2(max3(da0, da1, da2), max3(db0, db1, db2));

        if (minD > -epsilon)//仅考虑前面的
        {
            if(m_front) return m_front->collision(triangle, offset, ci, world);
        }
        else if(maxD < epsilon)//仅考虑后面的
        {
            if(m_back) return m_back->collision(triangle, offset, ci, world);
        }
        else//跨越平面的
        {
            bool collied = collisionThis(triangle, offset, ci, world);
            if(m_front) collied |= m_front->collision(triangle, offset, ci, world);
            if(m_back) collied |= m_back->collision(triangle, offset, ci, world);
            return collied;

            //根据offset的方向，决定先判断正面还是背面
            bool isFromFront = m_panel.normal.dot(offset) < 0.0f;
            if(isFromFront)
            {
                if (m_front && m_front->collision(triangle, offset, ci, world))
                {
                    return true;
                }
            }
            else
            {
                if (m_back && m_back->collision(triangle, offset, ci, world))
                {
                    return true;
                }
            }

            if (collisionThis(triangle, offset, ci, world))
            {
                return true;
            }

            if(isFromFront)
            {
                if (m_back && m_back->collision(triangle, offset, ci, world))
                {
                    return true;
                }
            }
            else
            {
                if (m_front && m_front->collision(triangle, offset, ci, world))
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool BspNode::collisionThis(
        const Physics::Triangle & triangle,
        const Physics::Vector3 & offset,
        Physics::CollisionPrevent & ci, 
        const Matrix4x4 & world) const
    {
        bool collied = false;
        for (Physics::TriangleSet::const_iterator it = m_triangleSet.begin();
            it != m_triangleSet.end(); ++it)
        {
            if(it->intersect(triangle, offset))
            {
                collied |= ci.collision(*it, world);
            }
        }
        return collied;
    }

    bool BspNode::collision(Physics::CollisionPrevent & ci, const Matrix4x4 & world) const
    {
        assert(this && "collision ");
        bool collied = false;

        int side = ci.witchSide(m_triangleSet[0], world);

        if (side == Physics::SIDE_FRONT)
        {
            if(m_front) return m_front->collision(ci, world);
        }
        else if (side == Physics::SIDE_BACK)
        {
            if(m_back) return m_back->collision(ci, world);
        }
        else if (side == Physics::SIDE_BOTH)
        {
            collied |= collisionThis(ci, world);
            if(m_front) collied |= m_front->collision(ci, world);
            if(m_back) collied |= m_back->collision(ci, world);
            return collied;
        }

        return collied;
    }

    bool BspNode::collisionThis(Physics::CollisionPrevent & ci, const Matrix4x4 & world) const
    {
        bool collied = false;
        for (Physics::TriangleSet::const_iterator it = m_triangleSet.begin();
            it != m_triangleSet.end(); ++it)
        {
            collied |= ci.collision(*it, world);
        }
        return collied;
    }


    void BspNode::addToChild(BspNodePtr & child, 
        const Physics::Triangle & tri,  
        const Physics::Polygon & poly)
    {
        if (!child)
        {
            child = new BspNode(tri);
            return;
        }
        child->addTriangle(tri, poly);
    }

    void BspNode::drawDebug(IDirect3DDevice9 *device) const
    {
        for(Physics::TriangleSet::const_iterator it = m_triangleSet.begin();
            it != m_triangleSet.end(); ++it)
        {
            Physics::drawTriangle(device, *it, 0x7f7f7f7f);
        }
        if (m_front)
        {
            m_front->drawDebug(device);
        }
        if (m_back)
        {
            m_back->drawDebug(device);
        }
    }

    void BspNode::load(Lazy::DataStream & stream)
    {
        int triangleNum = stream.loadStruct<int>();
        m_triangleSet.resize(triangleNum);

        for (Physics::TriangleSet::iterator it = m_triangleSet.begin(); 
            it != m_triangleSet.end(); ++it)
        {
            stream.loadStruct(*it);
        }
        m_panel.set(m_triangleSet[0].normal(), m_triangleSet[0].a);

        int childCount = stream.loadStruct<int>();
        if (childCount <= 0)
        {
            return;
        }
        else if(childCount == 1)
        {
            int type = stream.loadStruct<int>();

            BspNodePtr ptr = new BspNode();
            ptr->load(stream);

            if (type == 0) { m_front = ptr; }
            else if (type == 1) { m_back = ptr; }
            else{ assert("bsp child type error!"); }
        }
        else if (childCount == 2)
        {
            m_front = new BspNode();
            m_front->load(stream);

            m_back = new BspNode();
            m_back->load(stream);
        }
    }

    void BspNode::save(Lazy::DataStream & stream) const
    {
        int triangleNum = m_triangleSet.size();
        if (triangleNum == 0)
        {
            return;
        }
        stream.saveStruct(triangleNum);

        for (Physics::TriangleSet::const_iterator it = m_triangleSet.begin(); 
            it != m_triangleSet.end(); ++it)
        {
            stream.saveStruct(*it);
        }

        int childCount = 0;
        if (m_front)  ++childCount;
        if (m_back) ++childCount;

        stream.saveStruct(childCount);

        if (m_front)
        {
            //如果孩子数大于2，则不用存贮标示符
            if (childCount < 2) 
            {
                stream.saveStruct(0);
            }
            m_front->save(stream);
        }

        if (m_back)
        {
            //如果孩子数大于2，则不用存贮标示符
            if (childCount < 2)
            {
                stream.saveStruct(1);
            }
            m_back->save(stream);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////


    bool BspTree::generateBsp(ID3DXMesh* mesh)
    {
        if (!mesh)
        {
            return false;
        }

        MeshExtractor extractor;
        if(!extractor.build(mesh)) return false;

        for(size_t i=0; i<extractor.nbTriangle(); ++i)
        {
            addTriangle(extractor.getTriangle(i));
        }

        return true;
    }

    bool BspTree::load(const Lazy::tstring & fname)
    {
        Lazy::DataStream stream;
        if(!Lazy::getfs()->readBinary(fname, stream.steam())) return false;

        size_t magic;
        stream.loadStruct(magic);
        if(magic != BspMagic) return false;

        size_t version;
        stream.loadStruct(version);
        if(version != BspVersion) return false;

        stream.skip(16);

        m_root = new BspNode();
        m_root->load(stream);

        return true;
    }

    bool BspTree::save(const Lazy::tstring & fname) const
    {
        if(!m_root) return false;

        Lazy::DataStream stream;
        stream.saveStruct(BspMagic);
        stream.saveStruct(BspVersion);

        //预留16个字节
        for(int i=0; i<16; ++i)
        {
            stream.append(0);
        }

        m_root->save(stream);

        Lazy::streambuffer & buffer = stream.steam();
        return Lazy::getfs()->writeBinary(fname, &buffer[0], buffer.size());
    }

    bool BspTree::collision(Physics::CollisionPrevent & ci, const Matrix4x4 & world) const
    {
        assert(m_root);
#if 0
        return m_root->collision(ci, world);
#else
        Physics::Triangle triangle(
            ci.m_point8[0], 
            (ci.m_point8[1] + ci.m_point8[2])/2.0f,
            ci.m_point8[3]);
        Physics::Vector3 dest(ci.m_point8[4]);

        Matrix4x4 invWorld;
        D3DXMatrixInverse(&invWorld, 0, &world);
        triangle.applyMatrix(invWorld);
        dest.applyMatrix(invWorld);

        Physics::addDebugDrawTriangle(triangle, 0xffffffff);

        return m_root->collision(triangle, dest - triangle.a, ci, world);

#endif
    }

    BspNodePtr BspTree::createBspNode(const Physics::Triangle & tri)
    {
        return new BspNode(tri);
    }

    void BspTree::addTriangle(const Physics::Triangle & tri)
    {
        if (!m_root)
        {
            m_root = createBspNode(tri);
            return;
        }

        Physics::Polygon poly;
        m_root->addTriangle(tri, poly);
    }

    void BspTree::drawDebug(IDirect3DDevice9 *pDevice, const Matrix4x4 * pWorld/*=NULL*/) const
    {
        if (!pWorld)
        {
            Matrix4x4 world;
            D3DXMatrixIdentity(&world);
            pDevice->SetTransform(D3DTS_WORLD, &world);
        }
        else
        {
            pDevice->SetTransform(D3DTS_WORLD, pWorld);
        }

        if (m_root)
        {
            m_root->drawDebug(pDevice);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //bsp缓存相关
    //////////////////////////////////////////////////////////////////////////

    typedef std::map<Lazy::tstring, BspTreePtr> BSPTree2Cache;
    BSPTree2Cache g_bspTree2Cache;

    BspTreePtr loadBspTree(const Lazy::tstring & bspName)
    {
        BSPTree2Cache::iterator it = g_bspTree2Cache.find(bspName);
        if (it != g_bspTree2Cache.end())
        {
            return it->second;
        }

        LOG_DEBUG(L"Load bsp tree[%s]....", bspName.c_str());

        BspTreePtr tree = new BspTree();
        if(!tree->load(bspName))
        {
            LOG_ERROR(L"Load bsp data '%s' failed!", bspName.c_str());
            tree = NULL;
        }
        g_bspTree2Cache[bspName] = tree;

        LOG_DEBUG(L"Load bsp tree[%s] finish.", bspName.c_str());

        return tree;
    }

    BspTreePtr getModelBspTree(const Lazy::tstring & mname)
    {
        Lazy::tstring modelName = mname;
        Lazy::removeFilePath(modelName);
        Lazy::removeFileExt(modelName);

        Lazy::tstring tempPath = BspFilePath + modelName;
        tempPath += _T(".bsp");

        return loadBspTree(tempPath);
    }

    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////
    BspNodePtr AVLBspTree::createBspNode(const Physics::Triangle & tri)
    {
        return new AVLBspNode(this, NULL, tri);
    }

    void AVLBspTree::setRoot(AVLBspNodePtr ptr)
    {
        m_root = ptr;
        if (ptr)
        {
            ptr->setParent(NULL);
        }
    }

    void AVLBspTree::testSave(const std::wstring & fname)
    {
        Lazy::LZDataPtr xmlRoot = new Lazy::xml();
        if (!xmlRoot || !m_root)
        {
            return;
        }
        AVLBspNodePtr root = (AVLBspNodePtr)m_root;
        root->testSave(xmlRoot);
        xmlRoot->save(fname);
    }
    //////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////

    void AVLBspNode::updateBalence()
    {
        m_balence = 0;
        if (m_front)
        {
            m_balence += 1;
            m_balence += abs(left()->m_balence);
        }
        if(m_back)
        {
            m_balence -= 1;
            m_balence -= abs(right()->m_balence);
        }
    }

    void AVLBspNode::addToChild(BspNodePtr & child, 
        const Physics::Triangle & tri,  
        const Physics::Polygon & poly)
    {
        if (child)
        {
            child->addTriangle(tri, poly);
            return;
        }

        AVLBspNodePtr node = new AVLBspNode(m_tree, this, tri);
        child = node;

        AVLBspNodePtr unbalencedNode = NULL;
        while(!node->isRoot())
        {
            node = node->m_parent;
            node->updateBalence();
            if(node->m_balence<-1 || node->m_balence>1)
            {
                unbalencedNode = node;
                break;
            }
        }
        if(!unbalencedNode)
        {
            return;
        }

        AVLBspNodePtr base = unbalencedNode->parent();
        bool isOnLeft = false;
        if (base)
        {
            isOnLeft = unbalencedNode->isLeft();
        }

        if (unbalencedNode->m_balence > 1)//L
        {
            node = unbalencedNode->left();
            //assert(node && "left node is NULL!");

            if (node->m_balence > 0)//L
            {
                //LL
                unbalencedNode->setLeft(node->right());
                node->setRight(unbalencedNode);

                unbalencedNode->updateBalence();
            }
            else //R
            {
                //LR
                AVLBspNodePtr parent = node;
                node = node->right();
                assert(node && "LR : node right is NULL!");

                unbalencedNode->setLeft(node->right());
                parent->setRight(node->left());

                unbalencedNode->updateBalence();
                parent->updateBalence();

                node->setLeft(parent);
                node->setRight(unbalencedNode);
            }
        }
        else if(unbalencedNode->m_balence < -1)//R
        {
            node = unbalencedNode->right();
            assert(node && "right node is NULL!");
            //assert(node->m_balence == 0 && "R: balence is Zero! needn't to be rotate!");
            if (node->m_balence == 0)
            {
                node->m_balence = 0;
            }

            if (node->m_balence < 0)//R
            {
                //RR
                unbalencedNode->setRight(node->left());
                node->setLeft(unbalencedNode);

                unbalencedNode->updateBalence();
            }
            else//L
            {
                //RL
                AVLBspNodePtr parent = node;
                node = node->left();
                assert(node && "RL: left node is NULL!");

                parent->setLeft(node->right());
                unbalencedNode->setRight(node->left());

                unbalencedNode->updateBalence();
                parent->updateBalence();

                node->setLeft(unbalencedNode);
                node->setRight(parent);
            }
        }
        else
        {
            assert("unbalencedNode's balence is right!");
        }

        node->updateBalence();

        if(NULL == base)//root
        {
            m_tree->setRoot(node);
        }
        else if(isOnLeft)//left
        {
            base->setLeft(node);
        }
        else//right
        {
            base->setRight(node);
        }

    }

    void AVLBspNode::testSave(Lazy::LZDataPtr xmlRoot)
    {
        Lazy::tstring buffer;
        formatString(buffer, L"0x%d[%d]", (DWORD)this, m_balence);
        Lazy::LZDataPtr root = xmlRoot->write(buffer);

        if (m_front)
        {
            left()->testSave(root);
        }
        if(m_back)
        {
            right()->testSave(root);
        }
    }

}//end namespace Physics