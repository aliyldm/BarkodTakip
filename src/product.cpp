#include "product.h"
#include <QDateTime>

Product::Product(const QString &barcode, const QString &name, const QString &brand, const QDate &expiryDate)
    : barcode(barcode), name(name), brand(brand), expiryDate(expiryDate)
{
}

Product::Product(const QJsonObject &json)
    : barcode(json["barcode"].toString()),
      name(json["name"].toString()),
      brand(json["brand"].toString()),
      expiryDate(QDate::fromString(json["expiryDate"].toString(), Qt::ISODate))
{
}

int Product::getDaysUntilExpiry() const
{
    return QDate::currentDate().daysTo(expiryDate);
}

QJsonObject Product::toJson() const
{
    QJsonObject json;
    json["barcode"] = barcode;
    json["name"] = name;
    json["brand"] = brand;
    json["expiryDate"] = expiryDate.toString(Qt::ISODate);
    return json;
} 