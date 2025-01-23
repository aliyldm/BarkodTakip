#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>
#include <QDate>
#include <QJsonObject>

class Product {
public:
    Product(const QString &barcode, const QString &brand, const QDate &expiryDate);
    Product(const QJsonObject &json); // JSON'dan oluşturucu

    QString getBarcode() const { return barcode; }
    QString getBrand() const { return brand; }
    QDate getExpiryDate() const { return expiryDate; }
    int getDaysUntilExpiry() const;
    void setExpiryDate(const QDate &newDate) { expiryDate = newDate; }
    void setBarcode(const QString &newBarcode) { barcode = newBarcode; }
    void setBrand(const QString &newBrand) { brand = newBrand; }

    // JSON dönüşüm metodları
    QJsonObject toJson() const;

private:
    QString barcode;
    QString brand;
    QDate expiryDate;
};

#endif // PRODUCT_H 