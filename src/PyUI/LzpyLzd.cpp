#include "stdafx.h"
#include "LzpyLzd.h"

using namespace Lazy;

namespace Lzpy
{

    object make_object(LZDataPtr ptr)
    {
        PyLazyData *p = helper::new_instance_ex<PyLazyData>();
        p->m_data = ptr;
        return new_reference(p);
    }

    bool parse_object(LZDataPtr & ptr, object o)
    {
        if (!helper::has_instance<PyLazyData>(o.get(), true)) return false;
        ptr = (o.cast<PyLazyData>())->m_data;
        return true;
    }

    ////////////////////////////////////////////////////////////////////
    LZPY_CLASS_BEG(PyLazyData);

        LZPY_METHOD(newOne);

        LZPY_METHOD(read);
        LZPY_METHOD(write);
        LZPY_METHOD(addChild);
        LZPY_METHOD(getChildren);
        LZPY_METHOD(getFirstChild);

        LZPY_METHOD(getTag);
        LZPY_METHOD(setTag);

        LZPY_METHOD(getValue);
        LZPY_METHOD(setValue);

        LZPY_METHOD(readBool);
        LZPY_METHOD(writeBool);

        LZPY_METHOD(readInt);
        LZPY_METHOD(writeInt);

        LZPY_METHOD(readHex);
        LZPY_METHOD(writeHex);

        LZPY_METHOD(readFloat);
        LZPY_METHOD(writeFloat);

        LZPY_METHOD(readString);
        LZPY_METHOD(writeString);

    LZPY_CLASS_END();

    PyLazyData::PyLazyData()
    {
    }


    PyLazyData::~PyLazyData()
    {
    }

    LZPY_IMP_INIT(PyLazyData)
    {
        PyErr_SetString(PyExc_RuntimeError, "Can't create instantiation for this type directly!");
        return false;
    }


    LZPY_IMP_METHOD(PyLazyData, newOne)
    {
        assert(m_data);

        wchar_t *tag = L"";
        wchar_t *value = L"";
        if (!PyArg_ParseTuple(arg.get(), "|uu", &tag, &value)) return null_object;

        PyLazyData *p = helper::new_instance_ex<PyLazyData>();
        p->m_data = m_data->newOne(tag, value);
        return new_reference(p);
    }

    LZPY_IMP_METHOD(PyLazyData, read)
    {
        assert(m_data);

        wchar_t *tag;
        if (!PyArg_ParseTuple(arg.get(), "u", &tag)) return null_object;

        LZDataPtr ptr = m_data->read(tag);
        if (!ptr) return none_object;

        PyLazyData *py = helper::new_instance_ex<PyLazyData>();
        py->m_data = ptr;

        return new_reference(py);
    }

    LZPY_IMP_METHOD(PyLazyData, write)
    {
        wchar_t *tag;
        wchar_t *value = L"";
        if (!PyArg_ParseTuple(arg.get(), "u|u", &tag, &value)) return null_object;

        LZDataPtr ptr = m_data->write(tag, value);
        if (!ptr) return none_object;

        PyLazyData *py = helper::new_instance_ex<PyLazyData>();
        py->m_data = ptr;

        return new_reference(py);
    }

    LZPY_IMP_METHOD(PyLazyData, addChild)
    {
        assert(m_data);

        PyLazyData *child;
        if (!PyArg_ParseTuple(arg.get(), "O", &child)) return null_object;

        if (!helper::has_instance<PyLazyData>(child, true)) return null_object;

        m_data->addChild(child->m_data);
        return none_object;
    }
    LZPY_IMP_METHOD(PyLazyData, getChildren)
    {
        assert(m_data);

        int n = m_data->countChildren();
        tuple children(n);
        for (int i = 0; i < n; ++i)
        {
            PyLazyData *py = helper::new_instance_ex<PyLazyData>();
            py->m_data = m_data->getChild(i);
            children.setitem(i, new_reference(py));
        }

        return children;
    }

    LZPY_IMP_METHOD(PyLazyData, getFirstChild)
    {
        assert(m_data);

        int n = m_data->countChildren();
        if (n > 0)
            return make_object(m_data->getChild(0));
        
        return none_object;
    }

    LZPY_IMP_METHOD(PyLazyData, load)
    {
        assert(m_data);

        wchar_t *file;
        if (!PyArg_ParseTuple(arg.get(), "u", &file)) return null_object;

        bool ret = m_data->load(file);
        return build_object(ret);
    }
    LZPY_IMP_METHOD(PyLazyData, save)
    {
        assert(m_data);

        wchar_t *file;
        if (!PyArg_ParseTuple(arg.get(), "u", &file)) return null_object;

        bool ret = m_data->save(file);
        return build_object(ret);
    }

    LZPY_IMP_METHOD(PyLazyData, getTag)
    {
        assert(m_data);

        return build_object(m_data->tag());
    }
    LZPY_IMP_METHOD(PyLazyData, setTag)
    {
        assert(m_data);

        wchar_t *str;
        if (!PyArg_ParseTuple(arg.get(), "u", &str)) return null_object;
        m_data->setTag(str);

        return none_object;
    }

    LZPY_IMP_METHOD(PyLazyData, getValue)
    {
        assert(m_data);

        return build_object(m_data->value());
    }
    LZPY_IMP_METHOD(PyLazyData, setValue)
    {
        assert(m_data);

        wchar_t *str;
        if (!PyArg_ParseTuple(arg.get(), "u", &str)) return null_object;
        m_data->setValue(str);

        return none_object;
    }

    LZPY_IMP_METHOD(PyLazyData, readBool)
    {
        assert(m_data);

        PyObject *boo = nullptr;
        wchar_t *tag;
        if (!PyArg_ParseTuple(arg.get(), "u|O", &tag, &boo)) return null_object;

        bool def = boo && PyObject_IsTrue(boo) != 0;
        bool ret = m_data->readBool(tag, def);

        return build_object(ret);
    }

    LZPY_IMP_METHOD(PyLazyData, writeBool)
    {
        assert(m_data);

        PyObject *boo;
        wchar_t *tag;
        if (!PyArg_ParseTuple(arg.get(), "uO", &tag, &boo)) return null_object;

        bool value = PyObject_IsTrue(boo) != 0;
        m_data->writeBool(tag, value);

        return none_object;
    }

    LZPY_IMP_METHOD(PyLazyData, readInt)
    {
        assert(m_data);

        int def = 0;
        wchar_t *tag;
        if (!PyArg_ParseTuple(arg.get(), "u|i", &tag, &def)) return null_object;

        int ret = m_data->readInt(tag, def);
        return build_object(ret);
    }
    LZPY_IMP_METHOD(PyLazyData, writeInt)
    {
        assert(m_data);

        int value;
        wchar_t *tag;
        if (!PyArg_ParseTuple(arg.get(), "ui", &tag, &value)) return null_object;

        m_data->writeInt(tag, value);
        return none_object;
    }

    LZPY_IMP_METHOD(PyLazyData, readHex)
    {
        assert(m_data);

        size_t def = 0;
        wchar_t *tag;
        if (!PyArg_ParseTuple(arg.get(), "u|I", &tag, &def)) return null_object;

        size_t ret = m_data->readHex(tag, def);
        return build_object(ret);
    }
    LZPY_IMP_METHOD(PyLazyData, writeHex)
    {
        assert(m_data);

        size_t value;
        wchar_t *tag;
        if (!PyArg_ParseTuple(arg.get(), "uI", &tag, &value)) return null_object;

        m_data->writeHex(tag, value);
        return none_object;
    }

    LZPY_IMP_METHOD(PyLazyData, readFloat)
    {
        assert(m_data);

        double def = 0.0;
        wchar_t *tag;
        if (!PyArg_ParseTuple(arg.get(), "u|d", &tag, &def)) return null_object;

        double ret = m_data->readDouble(tag, def);
        return build_object(ret);
    }
    LZPY_IMP_METHOD(PyLazyData, writeFloat)
    {
        assert(m_data);

        double value;
        wchar_t *tag;
        if (!PyArg_ParseTuple(arg.get(), "ud", &tag, &value)) return null_object;

        m_data->writeDouble(tag, value);
        return none_object;
    }

    LZPY_IMP_METHOD(PyLazyData, readString)
    {
        assert(m_data);

        wchar_t *def = L"";
        wchar_t *tag;
        if (!PyArg_ParseTuple(arg.get(), "u|u", &tag, &def)) return null_object;

        tstring ret = m_data->readString(tag, def);
        return build_object(ret);
    }
    LZPY_IMP_METHOD(PyLazyData, writeString)
    {
        assert(m_data);

        wchar_t *value;
        wchar_t *tag;
        if (!PyArg_ParseTuple(arg.get(), "uu", &tag, &value)) return null_object;

        m_data->writeString(tag, value);
        return none_object;
    }

    ///////////////////////////////////////////////////////////////////

    LZPY_DEF_FUN(openLzd)
    {
        wchar_t *path;
        PyObject *boo = nullptr;
        if (!PyArg_ParseTuple(arg, "u|O", &path, &boo)) return nullptr;

        bool loadIfMiss = boo && fromPyObject<bool>(boo);

        LZDataPtr ptr = Lazy::openSection(path, loadIfMiss, Lazy::DataType::Lzd);
        if (!ptr) Py_RETURN_NONE;

        PyLazyData *pData = helper::new_instance_ex<PyLazyData>();
        pData->m_data = ptr;

        return pData;
    }

    LZPY_DEF_FUN(openXml)
    {
        wchar_t *path;
        PyObject *boo = nullptr;
        if (!PyArg_ParseTuple(arg, "u|O", &path, &boo)) return nullptr;

        LZDataPtr ptr = Lazy::openSection(path, fromPyObject<bool>(boo), Lazy::DataType::Xml);
        if (!ptr) Py_RETURN_NONE;

        PyLazyData *pData = helper::new_instance_ex<PyLazyData>();
        pData->m_data = ptr;

        return pData;
    }

    LZPY_DEF_FUN(saveSection)
    {
        PyLazyData *pData;
        wchar_t *path;
        if (!PyArg_ParseTuple(arg, "Ou", &pData, &path)) return nullptr;

        if (!helper::has_instance<PyLazyData>(pData, true)) return nullptr;

        LZDataPtr ptr = pData->m_data;
        if (!ptr) Py_RETURN_FALSE;

        bool ret = saveSection(ptr, path);
        return toPyObject(ret);
    }

    LZPY_MODULE_BEG(lzd)
        LZPY_REGISTER_CLASS(lzd, PyLazyData);
        LZPY_FUN(openLzd);
        LZPY_FUN(openXml);
        LZPY_FUN(saveSection);
    LZPY_MODULE_END();
}