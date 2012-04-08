#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QtCore/QVariant>
#include <QtCore/QSettings>
#include <QtCore/QRegExp>

class DataHandler
{
public:
    DataHandler();
    DataHandler(const QVariant &data);
    DataHandler &operator =(const QVariant &data);
    operator QVariant() const;

    bool hasMethod() const;
    QByteArray method() const;
    void setMethod(const QByteArray &method);
    void unsetMethod();

    QRegExp urlRegex() const;
    void setUrlRegex(const QRegExp &regex);

    QString fileName() const;
    void setFileName(const QString &fileName);

    void save(QSettings &settings) const;

    static void save(QList<DataHandler> data, QSettings &settings);

private:
    QByteArray method_;
    QRegExp regex_;
    QString fileName_;
};

#endif // DATAHANDLER_H
