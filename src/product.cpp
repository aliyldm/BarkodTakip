#include "product.h"

Product::Product(const QString &barcode, const QString &brand, const QDate &expiryDate)
    : barcode(barcode), brand(brand), expiryDate(expiryDate)
{
}

Product::Product(const QJsonObject &json)
    : barcode(json["barcode"].toString()),
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
    json["brand"] = brand;
    json["expiryDate"] = expiryDate.toString(Qt::ISODate);
    return json;
} 