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



    /*static*/ ModelPtr ModelFactory::loadModel(const std::wstring & filename, int typeId)
    {

        ModelPtr pModel;

        if (typeId == ModelType::SkinModel)
        {
            pModel = new AnimalModel();
        }
        else
        {
            return nullptr;
        }

        if (!pModel->load(filename))
        {
            return nullptr;
        }

        return pModel;
    }

} // end namespace Lazy
