#ifndef TUFAO_GLOBAL_H
#define TUFAO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(TUFAO_LIBRARY)
#  define TUFAOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define TUFAOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // TUFAO_GLOBAL_H
