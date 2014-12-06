#include "stdafx.h"
#include "ModelFactory.h"

#include "AnimModel.h"

namespace Lazy
{

    ModelFactory::ModelFactory()
    {
    }

    ModelFactory::~ModelFactory()
    {
    }

    /*static*/ ModelPtr ModelFactory::loadModel(const std::wstring & filename)
    {
        ModelPtr pModel = new AnimalModel();
        
        if (!pModel->load(filename))
        {
            return nullptr;
        }

        return pModel;
    }

} // end namespace Lazy
