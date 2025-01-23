#pragma once

#include <QString>
#include <QDate>
#include <QJsonObject>

class Product {
public:
    Product(const QString &barcode, const QString &name, const QString &brand, const QDate &expiryDate);
    Product(const QJsonObject &json); // JSON'dan oluşturucu

    QString getBarcode() const { return barcode; }
    QString getName() const { return name; }
    QString getBrand() const { return brand; }
    QDate getExpiryDate() const { return expiryDate; }
    int getDaysUntilExpiry() const;
    void setExpiryDate(const QDate &newDate) { expiryDate = newDate; }
    void setBarcode(const QString &newBarcode) { barcode = newBarcode; }
    void setName(const QString &newName) { name = newName; }
    void setBrand(const QString &newBrand) { brand = newBrand; }

    // JSON dönüşüm metodları
    QJsonObject toJson() const;

private:
    QString barcode;
    QString name;
    QString brand;
    QDate expiryDate;
}; 