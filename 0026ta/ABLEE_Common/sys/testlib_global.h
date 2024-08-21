#ifndef TESTLIB_GLOBAL_H
#define TESTLIB_GLOBAL_H

//#include <QtCore/qglobal.h>

#if defined(TESTLIB_LIBRARY)
#  define TESTLIBSHARED_EXPORT __attribute__((visibility("default")))
#else
#  define TESTLIBSHARED_EXPORT __attribute__((visibility("default")))
#endif

#endif // TESTLIB_GLOBAL_H
