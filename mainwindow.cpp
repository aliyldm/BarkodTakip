#include "mainwindow.h"
#include <QMessageBox>
#include <QStyle>
#include <QStyleFactory>
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QHeaderView>

EditDialog::EditDialog(const QString &barcode, const QString &brand, const QDate &expiryDate, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Ürün Düzenle");
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QLabel *barcodeLabel = new QLabel("Barkod:", this);
    barcodeEdit = new QLineEdit(barcode, this);
    
    QLabel *brandLabel = new QLabel("Marka:", this);
    brandEdit = new QLineEdit(brand, this);
    
    QLabel *expiryLabel = new QLabel("Son Kullanma Tarihi:", this);
    expiryDateEdit = new QDateEdit(expiryDate, this);
    expiryDateEdit->setCalendarPopup(true);
    
    QPushButton *okButton = new QPushButton("Tamam", this);
    QPushButton *cancelButton = new QPushButton("İptal", this);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    
    layout->addWidget(barcodeLabel);
    layout->addWidget(barcodeEdit);
    layout->addWidget(brandLabel);
    layout->addWidget(brandEdit);
    layout->addWidget(expiryLabel);
    layout->addWidget(expiryDateEdit);
    layout->addLayout(buttonLayout);
    
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

BulkEditDialog::BulkEditDialog(std::vector<Product> &products, QWidget *parent)
    : QDialog(parent), products(products)
{
    setWindowTitle("Toplu Düzenleme");
    setMinimumWidth(600);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // Tablo
    productTable = new QTableWidget(this);
    productTable->setColumnCount(4); // Seçim, Barkod, Marka, Son Kullanma Tarihi
    productTable->setHorizontalHeaderLabels({"Seç", "Barkod", "Marka", "Son Kullanma Tarihi"});
    productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    productTable->verticalHeader()->setVisible(false);
    
    // Ürünleri tabloya ekle
    productTable->setRowCount(products.size());
    for (size_t i = 0; i < products.size(); ++i) {
        QTableWidgetItem *checkItem = new QTableWidgetItem();
        checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        checkItem->setCheckState(Qt::Unchecked);
        
        QTableWidgetItem *barcodeItem = new QTableWidgetItem(products[i].getBarcode());
        QTableWidgetItem *brandItem = new QTableWidgetItem(products[i].getBrand());
        QTableWidgetItem *dateItem = new QTableWidgetItem(products[i].getExpiryDate().toString("dd.MM.yyyy"));
        
        productTable->setItem(i, 0, checkItem);
        productTable->setItem(i, 1, barcodeItem);
        productTable->setItem(i, 2, brandItem);
        productTable->setItem(i, 3, dateItem);
    }
    
    // Düzenleme kontrolleri
    QGroupBox *editGroup = new QGroupBox("Toplu Düzenleme Seçenekleri", this);
    QVBoxLayout *editLayout = new QVBoxLayout(editGroup);
    
    updateExpiryCheckBox = new QCheckBox("Son Kullanma Tarihini Güncelle", this);
    newExpiryDateEdit = new QDateEdit(QDate::currentDate(), this);
    newExpiryDateEdit->setCalendarPopup(true);
    newExpiryDateEdit->setEnabled(false);
    
    updateBrandCheckBox = new QCheckBox("Markayı Güncelle", this);
    newBrandEdit = new QLineEdit(this);
    newBrandEdit->setEnabled(false);
    
    editLayout->addWidget(updateExpiryCheckBox);
    editLayout->addWidget(newExpiryDateEdit);
    editLayout->addWidget(updateBrandCheckBox);
    editLayout->addWidget(newBrandEdit);
    
    // Butonlar
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    applyButton = new QPushButton("Uygula", this);
    QPushButton *cancelButton = new QPushButton("İptal", this);
    
    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(cancelButton);
    
    // Ana layout'a ekle
    layout->addWidget(productTable);
    layout->addWidget(editGroup);
    layout->addLayout(buttonLayout);
    
    // Bağlantılar
    connect(updateExpiryCheckBox, &QCheckBox::toggled, newExpiryDateEdit, &QDateEdit::setEnabled);
    connect(updateBrandCheckBox, &QCheckBox::toggled, newBrandEdit, &QLineEdit::setEnabled);
    connect(applyButton, &QPushButton::clicked, this, &BulkEditDialog::applyChanges);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(productTable, &QTableWidget::itemChanged, this, &BulkEditDialog::updateSelectionCount);
    
    productTable->resizeColumnsToContents();
}

void BulkEditDialog::updateSelectionCount()
{
    int selectedCount = 0;
    for (int i = 0; i < productTable->rowCount(); ++i) {
        if (productTable->item(i, 0)->checkState() == Qt::Checked) {
            selectedCount++;
        }
    }
    applyButton->setText(QString("Uygula (%1 ürün seçili)").arg(selectedCount));
}

void BulkEditDialog::applyChanges()
{
    bool madeChanges = false;
    
    for (int i = 0; i < productTable->rowCount(); ++i) {
        if (productTable->item(i, 0)->checkState() == Qt::Checked) {
            if (updateExpiryCheckBox->isChecked()) {
                products[i].setExpiryDate(newExpiryDateEdit->date());
                madeChanges = true;
            }
            if (updateBrandCheckBox->isChecked() && !newBrandEdit->text().isEmpty()) {
                products[i].setBrand(newBrandEdit->text());
                madeChanges = true;
            }
        }
    }
    
    if (madeChanges) {
        accept();
    } else {
        QMessageBox::warning(this, "Uyarı", "Hiçbir değişiklik yapılmadı.");
    }
}

// Bulk Delete Dialog Implementation
BulkDeleteDialog::BulkDeleteDialog(std::vector<Product> &products, QWidget *parent)
    : QDialog(parent), products(products)
{
    setWindowTitle("Toplu Silme");
    setMinimumWidth(600);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // Tablo
    productTable = new QTableWidget(this);
    productTable->setColumnCount(5); // Seçim, Barkod, Marka, Son Kullanma Tarihi, Kalan Gün
    productTable->setHorizontalHeaderLabels({"Seç", "Barkod", "Marka", "Son Kullanma Tarihi", "Kalan Gün"});
    productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    productTable->verticalHeader()->setVisible(false);
    
    // Ürünleri tabloya ekle
    productTable->setRowCount(products.size());
    for (size_t i = 0; i < products.size(); ++i) {
        QTableWidgetItem *checkItem = new QTableWidgetItem();
        checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        checkItem->setCheckState(Qt::Unchecked);
        
        QTableWidgetItem *barcodeItem = new QTableWidgetItem(products[i].getBarcode());
        QTableWidgetItem *brandItem = new QTableWidgetItem(products[i].getBrand());
        QTableWidgetItem *dateItem = new QTableWidgetItem(products[i].getExpiryDate().toString("dd.MM.yyyy"));
        QTableWidgetItem *daysItem = new QTableWidgetItem(QString::number(products[i].getDaysUntilExpiry()));
        
        productTable->setItem(i, 0, checkItem);
        productTable->setItem(i, 1, barcodeItem);
        productTable->setItem(i, 2, brandItem);
        productTable->setItem(i, 3, dateItem);
        productTable->setItem(i, 4, daysItem);
    }
    
    // Butonlar
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    
    selectAllButton = new QPushButton("Tümünü Seç", this);
    deselectAllButton = new QPushButton("Seçimi Kaldır", this);
    deleteButton = new QPushButton("Seçilenleri Sil", this);
    QPushButton *cancelButton = new QPushButton("İptal", this);
    
    buttonLayout->addWidget(selectAllButton);
    buttonLayout->addWidget(deselectAllButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(cancelButton);
    
    layout->addWidget(productTable);
    layout->addLayout(buttonLayout);
    
    // Bağlantılar
    connect(selectAllButton, &QPushButton::clicked, this, &BulkDeleteDialog::selectAll);
    connect(deselectAllButton, &QPushButton::clicked, this, &BulkDeleteDialog::deselectAll);
    connect(deleteButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(productTable, &QTableWidget::itemChanged, this, &BulkDeleteDialog::updateSelectionCount);
    
    productTable->resizeColumnsToContents();
    updateSelectionCount();
}

void BulkDeleteDialog::selectAll()
{
    for (int i = 0; i < productTable->rowCount(); ++i) {
        productTable->item(i, 0)->setCheckState(Qt::Checked);
    }
}

void BulkDeleteDialog::deselectAll()
{
    for (int i = 0; i < productTable->rowCount(); ++i) {
        productTable->item(i, 0)->setCheckState(Qt::Unchecked);
    }
}

void BulkDeleteDialog::updateSelectionCount()
{
    int selectedCount = 0;
    for (int i = 0; i < productTable->rowCount(); ++i) {
        if (productTable->item(i, 0)->checkState() == Qt::Checked) {
            selectedCount++;
        }
    }
    deleteButton->setText(QString("Seçilenleri Sil (%1)").arg(selectedCount));
    deleteButton->setEnabled(selectedCount > 0);
}

std::vector<size_t> BulkDeleteDialog::getSelectedIndexes() const
{
    std::vector<size_t> selectedIndexes;
    for (int i = 0; i < productTable->rowCount(); ++i) {
        if (productTable->item(i, 0)->checkState() == Qt::Checked) {
            selectedIndexes.push_back(i);
        }
    }
    return selectedIndexes;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setupStyle();
    loadProducts(); // Uygulama başladığında ürünleri yükle
}

MainWindow::~MainWindow()
{
    saveProducts(); // Uygulama kapanırken ürünleri kaydet
}

void MainWindow::setupStyle()
{
    // Set modern style
    qApp->setStyle(QStyleFactory::create("Fusion"));
    
    // Set color palette
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(25, 25, 25));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    
    qApp->setPalette(palette);
}

QColor MainWindow::getExpiryColor(int daysUntilExpiry) const
{
    if (daysUntilExpiry < 10) {
        return QColor(200, 0, 0); // Koyu kırmızı (biraz daha koyu)
    } else if (daysUntilExpiry < 30) {
        return QColor(255, 255, 150); // Açık sarı
    }
    return QColor(); // Varsayılan renk
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Input area
    QGridLayout *inputLayout = new QGridLayout();
    
    QLabel *barcodeLabel = new QLabel("Barkod:", this);
    barcodeEdit = new QLineEdit(this);
    
    QLabel *brandLabel = new QLabel("Marka:", this);
    brandEdit = new QLineEdit(this);
    
    QLabel *expiryLabel = new QLabel("Son Kullanma Tarihi:", this);
    expiryDateEdit = new QDateEdit(this);
    expiryDateEdit->setCalendarPopup(true);
    expiryDateEdit->setDate(QDate::currentDate());

    addButton = new QPushButton("Ekle", this);
    
    inputLayout->addWidget(barcodeLabel, 0, 0);
    inputLayout->addWidget(barcodeEdit, 0, 1);
    inputLayout->addWidget(brandLabel, 0, 2);
    inputLayout->addWidget(brandEdit, 0, 3);
    inputLayout->addWidget(expiryLabel, 1, 0);
    inputLayout->addWidget(expiryDateEdit, 1, 1);
    inputLayout->addWidget(addButton, 1, 3);

    // Table
    productTable = new QTableWidget(this);
    productTable->setColumnCount(4);
    productTable->setHorizontalHeaderLabels({"Barkod", "Marka", "Son Kullanma Tarihi", "Kalan Gün"});
    productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    productTable->setSelectionMode(QAbstractItemView::SingleSelection);
    productTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    editButton = new QPushButton("Düzenle", this);
    deleteButton = new QPushButton("Sil", this);
    bulkEditButton = new QPushButton("Toplu Düzenle", this);
    bulkDeleteButton = new QPushButton("Toplu Sil", this);
    
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(bulkEditButton);
    buttonLayout->addWidget(bulkDeleteButton);

    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(productTable);
    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addProduct);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedProduct);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::editSelectedProduct);
    connect(bulkEditButton, &QPushButton::clicked, this, &MainWindow::showBulkEditDialog);
    connect(bulkDeleteButton, &QPushButton::clicked, this, &MainWindow::showBulkDeleteDialog);

    setWindowTitle("Barkod Takip Sistemi");
    resize(1000, 600);
}

QString MainWindow::getDataFilePath() const
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return dir.filePath("products.json");
}

void MainWindow::saveProducts()
{
    QJsonArray productsArray;
    for (const auto &product : products) {
        productsArray.append(product.toJson());
    }

    QJsonDocument document(productsArray);
    QFile file(getDataFilePath());
    
    if (file.open(QIODevice::WriteOnly)) {
        file.write(document.toJson());
        file.close();
    } else {
        QMessageBox::warning(this, "Hata", "Ürünler kaydedilirken bir hata oluştu.");
    }
}

void MainWindow::loadProducts()
{
    QFile file(getDataFilePath());
    if (!file.exists()) {
        return; // İlk çalıştırma, dosya henüz yok
    }

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument document = QJsonDocument::fromJson(data);
        file.close();

        if (document.isArray()) {
            products.clear();
            QJsonArray productsArray = document.array();
            for (const QJsonValue &value : productsArray) {
                if (value.isObject()) {
                    products.emplace_back(value.toObject());
                }
            }
            updateTable();
        }
    } else {
        QMessageBox::warning(this, "Hata", "Ürünler yüklenirken bir hata oluştu.");
    }
}

void MainWindow::addProduct()
{
    QString barcode = barcodeEdit->text().trimmed();
    QString brand = brandEdit->text().trimmed();
    
    if (barcode.isEmpty()) {
        QMessageBox::warning(this, "Hata", "Lütfen bir barkod numarası girin.");
        return;
    }

    QDate expiryDate = expiryDateEdit->date();
    products.emplace_back(barcode, brand, expiryDate);
    
    updateTable();
    saveProducts();
    
    barcodeEdit->clear();
    brandEdit->clear();
    barcodeEdit->setFocus();
}

void MainWindow::editSelectedProduct()
{
    int row = productTable->currentRow();
    if (row >= 0 && row < products.size()) {
        EditDialog dialog(products[row].getBarcode(), products[row].getBrand(), 
                        products[row].getExpiryDate(), this);
        if (dialog.exec() == QDialog::Accepted) {
            products[row].setBarcode(dialog.getBarcode());
            products[row].setBrand(dialog.getBrand());
            products[row].setExpiryDate(dialog.getExpiryDate());
            updateTable();
            saveProducts();
        }
    } else {
        QMessageBox::warning(this, "Hata", "Lütfen düzenlenecek bir ürün seçin.");
    }
}

void MainWindow::deleteSelectedProduct()
{
    int row = productTable->currentRow();
    if (row >= 0 && row < products.size()) {
        products.erase(products.begin() + row);
        updateTable();
        saveProducts(); // Ürün silindiğinde kaydet
    }
}

void MainWindow::showBulkEditDialog()
{
    if (products.empty()) {
        QMessageBox::warning(this, "Uyarı", "Düzenlenecek ürün bulunmamaktadır.");
        return;
    }

    BulkEditDialog dialog(products, this);
    if (dialog.exec() == QDialog::Accepted) {
        updateTable();
        saveProducts();
    }
}

void MainWindow::showBulkDeleteDialog()
{
    if (products.empty()) {
        QMessageBox::warning(this, "Uyarı", "Silinecek ürün bulunmamaktadır.");
        return;
    }

    BulkDeleteDialog dialog(products, this);
    if (dialog.exec() == QDialog::Accepted) {
        auto selectedIndexes = dialog.getSelectedIndexes();
        if (selectedIndexes.empty()) {
            return;
        }

        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Onay",
            QString("%1 ürünü silmek istediğinizden emin misiniz?").arg(selectedIndexes.size()),
            QMessageBox::Yes | QMessageBox::No
        );

        if (reply == QMessageBox::Yes) {
            // Sondan başa doğru silme işlemi yapılmalı
            for (auto it = selectedIndexes.rbegin(); it != selectedIndexes.rend(); ++it) {
                products.erase(products.begin() + *it);
            }
            updateTable();
            saveProducts();
        }
    }
}

void MainWindow::updateTable()
{
    productTable->setRowCount(products.size());
    
    for (size_t i = 0; i < products.size(); ++i) {
        QTableWidgetItem *barcodeItem = new QTableWidgetItem(products[i].getBarcode());
        QTableWidgetItem *brandItem = new QTableWidgetItem(products[i].getBrand());
        QTableWidgetItem *dateItem = new QTableWidgetItem(products[i].getExpiryDate().toString("dd.MM.yyyy"));
        
        int daysUntilExpiry = products[i].getDaysUntilExpiry();
        QTableWidgetItem *daysItem = new QTableWidgetItem(QString::number(daysUntilExpiry));
        
        QColor color = getExpiryColor(daysUntilExpiry);
        if (color.isValid()) {
            barcodeItem->setBackground(color);
            brandItem->setBackground(color);
            dateItem->setBackground(color);
            daysItem->setBackground(color);
            
            if (daysUntilExpiry < 10) {
                barcodeItem->setForeground(Qt::white);
                brandItem->setForeground(Qt::white);
                dateItem->setForeground(Qt::white);
                daysItem->setForeground(Qt::white);
            } else if (daysUntilExpiry < 30) {
                barcodeItem->setForeground(Qt::black);
                brandItem->setForeground(Qt::black);
                dateItem->setForeground(Qt::black);
                daysItem->setForeground(Qt::black);
            }
        }
        
        productTable->setItem(i, 0, barcodeItem);
        productTable->setItem(i, 1, brandItem);
        productTable->setItem(i, 2, dateItem);
        productTable->setItem(i, 3, daysItem);
    }

    productTable->resizeColumnsToContents();
} 