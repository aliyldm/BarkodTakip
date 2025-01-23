#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
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
    EditDialog(const QString &barcode, const QString &brand, const QDate &expiryDate, QWidget *parent = nullptr);
    QString getBarcode() const { return barcodeEdit->text(); }
    QString getBrand() const { return brandEdit->text(); }
    QDate getExpiryDate() const { return expiryDateEdit->date(); }

private:
    QLineEdit *barcodeEdit;
    QLineEdit *brandEdit;
    QDateEdit *expiryDateEdit;
};

class BulkEditDialog : public QDialog
{
    Q_OBJECT
public:
    BulkEditDialog(std::vector<Product> &products, QWidget *parent = nullptr);

private slots:
    void applyChanges();
    void updateSelectionCount();

private:
    std::vector<Product> &products;
    QTableWidget *productTable;
    QDateEdit *newExpiryDateEdit;
    QLineEdit *newBrandEdit;
    QCheckBox *updateExpiryCheckBox;
    QCheckBox *updateBrandCheckBox;
    QPushButton *applyButton;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addProduct();
    void deleteSelectedProduct();
    void editSelectedProduct();
    void showBulkEditDialog();
    void showBulkDeleteDialog();

private:
    void setupUI();
    void updateTable();
    void setupStyle();
    QColor getExpiryColor(int daysUntilExpiry) const;
    
    void saveProducts();
    void loadProducts();
    QString getDataFilePath() const;

    QLineEdit *barcodeEdit;
    QLineEdit *brandEdit;
    QDateEdit *expiryDateEdit;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QPushButton *editButton;
    QPushButton *bulkEditButton;
    QPushButton *bulkDeleteButton;
    QTableWidget *productTable;
    std::vector<Product> products;
};

class BulkDeleteDialog : public QDialog
{
    Q_OBJECT
public:
    BulkDeleteDialog(std::vector<Product> &products, QWidget *parent = nullptr);
    std::vector<size_t> getSelectedIndexes() const;

private slots:
    void updateSelectionCount();
    void selectAll();
    void deselectAll();

private:
    std::vector<Product> &products;
    QTableWidget *productTable;
    QPushButton *deleteButton;
    QPushButton *selectAllButton;
    QPushButton *deselectAllButton;
};

#endif // MAINWINDOW_H 