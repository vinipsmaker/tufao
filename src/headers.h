#ifndef TUFAO_HEADERS_H
#define TUFAO_HEADERS_H

#include <QMultiMap>
#include "ibytearray.h"

namespace Tufao {

struct Headers: public QMultiMap<IByteArray, QByteArray> {};

} // namespace Tufao

#endif // TUFAO_HEADERS_H
