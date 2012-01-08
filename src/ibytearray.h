#ifndef TUFAO_IBYTEARRAY_H
#define TUFAO_IBYTEARRAY_H

#include <QByteArray>

namespace Tufao {

class IByteArray : public QByteArray
{
public:
    IByteArray();
    IByteArray(const QByteArray &ba);
    IByteArray(const char *str);
    IByteArray(const char *data, int size);
    IByteArray(int size, char ch);
    operator QByteArray() const;

    bool operator !=(const IByteArray &ba) const;
    bool operator <(const IByteArray &ba) const;
    bool operator <=(const IByteArray &ba) const;
    IByteArray &operator =(const IByteArray &ba);
    bool operator == (const IByteArray &ba) const;
    bool operator >(const IByteArray &ba) const;
    bool operator >=(const IByteArray &ba) const;

    bool operator !=(const QByteArray &ba) const;
    bool operator <(const QByteArray &ba) const;
    bool operator <=(const QByteArray &ba) const;
    IByteArray &operator =(const QByteArray &ba);
    bool operator == (const QByteArray &ba) const;
    bool operator >(const QByteArray &ba) const;
    bool operator >=(const QByteArray &ba) const;
};

inline IByteArray::IByteArray()
{}

inline IByteArray::IByteArray(const QByteArray &ba) :
    QByteArray(ba)
{}

inline IByteArray::IByteArray(const char *str) :
    QByteArray(str)
{}

inline IByteArray::IByteArray(const char *data, int size) :
    QByteArray(data, size)
{}

inline IByteArray::IByteArray(int size, char ch) :
    QByteArray(size, ch)
{}

inline IByteArray::operator QByteArray() const
{
    return QByteArray(*this);
}

inline bool IByteArray::operator !=(const IByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) != 0;
}

inline bool IByteArray::operator <(const IByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) < 0;
}

inline bool IByteArray::operator <=(const IByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) <= 0;
}

inline IByteArray &IByteArray::operator =(const IByteArray &ba)
{
    return *this = ba;
}

inline bool IByteArray::operator ==(const IByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) == 0;
}

inline bool IByteArray::operator >(const IByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) > 0;
}

inline bool IByteArray::operator >=(const IByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) >= 0;
}

inline bool IByteArray::operator !=(const QByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) != 0;
}

inline bool IByteArray::operator <(const QByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) < 0;
}

inline bool IByteArray::operator <=(const QByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) <= 0;
}

inline IByteArray &IByteArray::operator =(const QByteArray &ba)
{
    return *this = ba;
}

inline bool IByteArray::operator ==(const QByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) == 0;
}

inline bool IByteArray::operator >(const QByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) > 0;
}

inline bool IByteArray::operator >=(const QByteArray &ba) const
{
    return qstricmp(constData(), ba.constData()) >= 0;
}

} // namespace Tufao

#endif // TUFAO_IBYTEARRAY_H
