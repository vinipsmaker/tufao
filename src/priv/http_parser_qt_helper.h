#ifndef QTADAPTER_H
#define QTADAPTER_H

#include <QtGlobal>

// defines:
#define HTTP_PARSER_STRICT 0
#define HTTP_PARSER_DEBUG 0

// types:
#define int8_t qint8
#define uint8_t quint8
#define uint32_t quint32
#define int64_t qint64
#define uint64_t quint64

// functions:
#define assert(expr) (expr) ? qt_noop() : qt_assert(#expr, __FILE__, __LINE__)

#endif // QTADAPTER_H
