#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QCheckBox>
#include <QGroupBox>
#include <vector>
#include "product.h"

class EditDialog : public QDialog
{
    Q_OBJECT
public:
    EditDialog(const QString &barcode, const QString &name, const QString &brand, const QDate &expiryDate, QWidget *parent = nullptr);
    QString getBarcode() const { return barcodeEdit->text(); }
    QString getName() const { return nameEdit->text(); }
    QString getBrand() const { return brandEdit->text(); }
    QDate getExpiryDate() const { return expiryDateEdit->date(); }

private:
    QLineEdit *barcodeEdit;
    QLineEdit *nameEdit;
    QLineEdit *brandEdit;
    QDateEdit *expiryDateEdit;
};

class BulkEditDialog : public QDialog
{
    Q_OBJECT
public:
    BulkEditDialog(std::vector<Product> &products, QWidget *parent = nullptr);

private slots:
    void selectAll();
    void deselectAll();
    void updateSelectionCount();
    void applyChanges();

private:
    std::vector<Product> &products;
    QTableWidget *productTable;
    QDateEdit *expiryDateEdit;
    QPushButton *applyButton;
};

class BulkDeleteDialog : public QDialog
{
    Q_OBJECT
public:
    BulkDeleteDialog(const std::vector<Product> &products, QWidget *parent = nullptr);
    std::vector<size_t> getSelectedIndexes() const;

private slots:
    void selectAll();
    void deselectAll();
    void updateSelectionCount();

private:
    const std::vector<Product> &products;
    QTableWidget *productTable;
    QPushButton *deleteButton;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addProduct();
    void editSelectedProduct();
    void deleteSelectedProduct();
    void showBulkEditDialog();
    void showBulkDeleteDialog();
    void showLogViewerDialog();

private:
    void setupUI();
    void setupStyle();
    void setupConnections();
    void updateTable();
    void saveProducts();
    void loadProducts();
    QString getDataFilePath() const;
    QColor getExpiryColor(int daysUntilExpiry) const;

    QLineEdit *barcodeEdit;
    QLineEdit *nameEdit;
    QLineEdit *brandEdit;
    QDateEdit *expiryDateEdit;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *bulkEditButton;
    QPushButton *bulkDeleteButton;
    QPushButton *viewLogsButton;
    QTableWidget *productTable;
    std::vector<Product> products;
}; 