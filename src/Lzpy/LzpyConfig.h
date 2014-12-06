#pragma once

#ifdef USE_BOOST_PYTHON_H
#include <boost/python.hpp>
#else
#include <Python.h>
#endif

#include <structmember.h>
#include <descrobject.h>

#ifndef DISABLE_EXPORT_MSG
#   define ENABLE_EXPORT_MSG
#endif

#include <vector>
#include <string>
#include <list>

