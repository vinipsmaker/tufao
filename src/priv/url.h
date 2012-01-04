#include <QRegExp>

namespace Tufao {
namespace Priv {

struct Url
{
    Url(const QRegExp &regex) :
        regex(regex)
    {}

    QRegExp regex;
};

} // namespace Priv
} // namespace Tufao
