#include "datahandler.h"

DataHandler::DataHandler()
{
}

DataHandler::DataHandler(const QVariant &data)
{
    *this = data;
}

DataHandler &DataHandler::operator =(const QVariant &data)
{
    QVariantMap map = data.toMap();

    regex_ = map["regex"].toRegExp();
    fileName_ = map["filename"].toString();
    if (map.contains("method"))
        method_ = map["method"].toByteArray();

    return *this;
}

DataHandler::operator QVariant() const
{
    QVariantMap map;

    map["regex"] = urlRegex();
    map["filename"] = fileName_;
    if (hasMethod())
        map["method"] = method_;

    return map;
}

bool DataHandler::hasMethod() const
{
    return !method_.isEmpty();
}

QByteArray DataHandler::method() const
{
    return method_;
}

void DataHandler::setMethod(const QByteArray &method)
{
    method_ = method;
}

void DataHandler::unsetMethod()
{
    method_.clear();
}

QRegExp DataHandler::urlRegex() const
{
    return regex_;
}

void DataHandler::setUrlRegex(const QRegExp &regex)
{
    regex_ = regex;
}

QString DataHandler::fileName() const
{
    return fileName_;
}

void DataHandler::setFileName(const QString &fileName)
{
    fileName_ = fileName;
}

void DataHandler::save(QSettings &settings) const
{
    if (hasMethod())
        settings.setValue("method", method_);

    settings.setValue("filename", fileName_);
    settings.setValue("regex", regex_);
}

void DataHandler::save(QList<DataHandler> data, QSettings &settings)
{
    settings.setValue("version", "0.1");
    settings.beginWriteArray("plugins");
    for (int i = 0;i != data.size();++i) {
        settings.setArrayIndex(i);
        data[i].save(settings);
    }
    settings.endArray();
}
