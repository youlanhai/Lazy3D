
#include "stdafx.h"
#include "WrapperDataLoader.h"

#include "../utility/Utility.h"

using namespace Lazy;

const std::string & testG(const std::string & t)
{
    return t;
}

void wrapperDataLoader()
{
    LOG_INFO(L"EXPORT - wrapperDataLoader");

    auto copyConstRef = return_value_policy<copy_const_reference>();

    def("testG", testG, return_internal_reference<>());

    class_<LZDataBase, LZDataPtr, boost::noncopyable>("LZDataPtr", no_init)
        .def("setTag", &LZDataBase::setTag)
        .def("tag", &LZDataBase::tag, copyConstRef)
        .def("read", &LZDataBase::read)
        .def("write", &LZDataBase::write)
        .def("asInt", &LZDataBase::asInt)
        .def("asBool", &LZDataBase::asBool)
        .def("asFloat", &LZDataBase::asDouble)
        .def("asString", &LZDataBase::asString, copyConstRef)
        .def("setInt", &LZDataBase::setInt)
        .def("setBool", &LZDataBase::setBool)
        .def("setFloat", &LZDataBase::setDouble)
        .def("setString", &LZDataBase::setString)
        .def("readBool", &LZDataBase::readBool)
        .def("readFloat", &LZDataBase::readDouble)
        .def("readString", &LZDataBase::readString, copyConstRef)
        .def("writeInt", &LZDataBase::writeInt)
        .def("writeBool", &LZDataBase::writeBool)
        .def("writeFloat", &LZDataBase::writeDouble)
        .def("writeString", &LZDataBase::writeString)
        .def("countChildren", &LZDataBase::countChildren)
        .def("addChild", &LZDataBase::addChild)
        .def("getChild", &LZDataBase::getChild)
        .def("findChild", &LZDataBase::findChild)
        .def("findNextChild", &LZDataBase::findNextChild)
        ;

    def("openSection", openSection);
    def("saveSection", saveSection);
    def("clearSectionCache", clearSectionCache);
}
