#include "mainwindow.h"
#include <QMessageBox>
#include <QStyle>
#include <QStyleFactory>
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QHeaderView>
#include <QFile>
#include <QIcon>
#include "logger.h"
#include "logviewerdialog.h"
#include <algorithm>

EditDialog::EditDialog(const QString &barcode, const QString &name, const QString &brand, const QDate &expiryDate, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Ürün Düzenle");
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QLabel *barcodeLabel = new QLabel("Barkod:", this);
    barcodeEdit = new QLineEdit(barcode, this);
    
    QLabel *nameLabel = new QLabel("Ürün Adı:", this);
    nameEdit = new QLineEdit(name, this);
    
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
    layout->addWidget(nameLabel);
    layout->addWidget(nameEdit);
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
    
    // Düzenleme seçenekleri
    QGroupBox *editOptionsGroup = new QGroupBox("Düzenleme Seçenekleri", this);
    QVBoxLayout *editOptionsLayout = new QVBoxLayout(editOptionsGroup);
    
    QLabel *expiryLabel = new QLabel("Yeni Son Kullanma Tarihi:", this);
    expiryDateEdit = new QDateEdit(QDate::currentDate(), this);
    expiryDateEdit->setCalendarPopup(true);
    
    // Butonlar
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *selectAllButton = new QPushButton("Tümünü Seç", this);
    QPushButton *deselectAllButton = new QPushButton("Seçimi Kaldır", this);
    applyButton = new QPushButton("Değişiklikleri Uygula", this);
    QPushButton *cancelButton = new QPushButton("İptal", this);
    
    buttonLayout->addWidget(selectAllButton);
    buttonLayout->addWidget(deselectAllButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(cancelButton);
    
    editOptionsLayout->addWidget(expiryLabel);
    editOptionsLayout->addWidget(expiryDateEdit);
    
    layout->addWidget(productTable);
    layout->addWidget(editOptionsGroup);
    layout->addLayout(buttonLayout);
    
    // Bağlantılar
    connect(selectAllButton, &QPushButton::clicked, this, &BulkEditDialog::selectAll);
    connect(deselectAllButton, &QPushButton::clicked, this, &BulkEditDialog::deselectAll);
    connect(applyButton, &QPushButton::clicked, this, &BulkEditDialog::applyChanges);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    // Özel seçim davranışı
    connect(productTable, &QTableWidget::itemClicked, this, [this](QTableWidgetItem *item) {
        if (item->column() == 0) { // Checkbox sütunu için
            bool isChecked = item->checkState() == Qt::Checked;
            item->setCheckState(isChecked ? Qt::Unchecked : Qt::Checked);
            updateSelectionCount();
        }
    });
    
    productTable->resizeColumnsToContents();
    updateSelectionCount();
}

void BulkEditDialog::selectAll()
{
    for (int i = 0; i < productTable->rowCount(); ++i) {
        productTable->item(i, 0)->setCheckState(Qt::Checked);
    }
}

void BulkEditDialog::deselectAll()
{
    for (int i = 0; i < productTable->rowCount(); ++i) {
        productTable->item(i, 0)->setCheckState(Qt::Unchecked);
    }
}

void BulkEditDialog::updateSelectionCount()
{
    int selectedCount = 0;
    for (int i = 0; i < productTable->rowCount(); ++i) {
        if (productTable->item(i, 0)->checkState() == Qt::Checked) {
            selectedCount++;
        }
    }
    applyButton->setText(QString("Değişiklikleri Uygula (%1)").arg(selectedCount));
    applyButton->setEnabled(selectedCount > 0);
}

void BulkEditDialog::applyChanges()
{
    QDate newExpiryDate = expiryDateEdit->date();
    
    for (int i = 0; i < productTable->rowCount(); ++i) {
        if (productTable->item(i, 0)->checkState() == Qt::Checked) {
            products[i].setExpiryDate(newExpiryDate);
        }
    }
    
    accept();
}

// Bulk Delete Dialog Implementation
BulkDeleteDialog::BulkDeleteDialog(const std::vector<Product> &products, QWidget *parent)
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
    
    QPushButton *selectAllButton = new QPushButton("Tümünü Seç", this);
    QPushButton *deselectAllButton = new QPushButton("Seçimi Kaldır", this);
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

// Özel sıralama sınıfı
class CustomTableWidgetItem : public QTableWidgetItem {
public:
    CustomTableWidgetItem(const QString &text) : QTableWidgetItem(text) {}
    
    bool operator<(const QTableWidgetItem &other) const override {
        return text().toInt() < other.text().toInt();
    }
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    Logger::instance().log(Logger::INFO, "Uygulama başlatıldı");
    setupUI();
    setupConnections();
    loadProducts();
    setupStyle();
}

MainWindow::~MainWindow()
{
    Logger::instance().log(Logger::INFO, "Uygulama kapatılıyor...");
    saveProducts();
}

void MainWindow::setupStyle()
{
    // Set modern style
    qApp->setStyle(QStyleFactory::create("Fusion"));
    
    // Load and apply stylesheet
    QFile styleFile(":/styles.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString style = QLatin1String(styleFile.readAll());
        qApp->setStyleSheet(style);
        styleFile.close();
    }
    
    // Set window icon
    setWindowIcon(QIcon(":/icons/app.ico"));
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
    
    QLabel *nameLabel = new QLabel("Ürün Adı:", this);
    nameEdit = new QLineEdit(this);
    
    QLabel *brandLabel = new QLabel("Marka:", this);
    brandEdit = new QLineEdit(this);
    
    QLabel *expiryLabel = new QLabel("Son Kullanma Tarihi:", this);
    expiryDateEdit = new QDateEdit(this);
    expiryDateEdit->setCalendarPopup(true);
    expiryDateEdit->setDate(QDate::currentDate());

    addButton = new QPushButton("Ekle", this);
    
    inputLayout->addWidget(barcodeLabel, 0, 0);
    inputLayout->addWidget(barcodeEdit, 0, 1);
    inputLayout->addWidget(nameLabel, 0, 2);
    inputLayout->addWidget(nameEdit, 0, 3);
    inputLayout->addWidget(brandLabel, 1, 0);
    inputLayout->addWidget(brandEdit, 1, 1);
    inputLayout->addWidget(expiryLabel, 1, 2);
    inputLayout->addWidget(expiryDateEdit, 1, 3);
    inputLayout->addWidget(addButton, 2, 3);

    // Table
    productTable = new QTableWidget(this);
    productTable->setColumnCount(5);
    productTable->setHorizontalHeaderLabels({"Barkod", "Ürün Adı", "Marka", "Son Kullanma Tarihi", "Kalan Gün"});
    productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    productTable->setSelectionMode(QAbstractItemView::SingleSelection);
    productTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Tablo başlıklarını düzenle
    QHeaderView *horizontalHeader = productTable->horizontalHeader();
    horizontalHeader->setSectionResizeMode(QHeaderView::Interactive);
    horizontalHeader->setStretchLastSection(true);
    horizontalHeader->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    horizontalHeader->setSectionsMovable(false);
    
    // Minimum sütun genişliklerini ayarla
    productTable->setColumnWidth(0, 120); // Barkod
    productTable->setColumnWidth(1, 150); // Ürün Adı
    productTable->setColumnWidth(2, 120); // Marka
    productTable->setColumnWidth(3, 150); // Son Kullanma Tarihi
    productTable->setColumnWidth(4, 100); // Kalan Gün
    
    // Sıralama özelliğini aktif et
    productTable->setSortingEnabled(true);
    horizontalHeader->setSectionsClickable(true);
    horizontalHeader->setSortIndicatorShown(true);
    
    // Seçim davranışını özelleştir
    connect(productTable, &QTableWidget::itemClicked, this, [this](QTableWidgetItem *item) {
        static int lastClickedRow = -1;
        int currentRow = item->row();
        
        if (currentRow == lastClickedRow) {
            productTable->clearSelection();
            lastClickedRow = -1;
        } else {
            lastClickedRow = currentRow;
            productTable->selectRow(currentRow);
        }
    });

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    editButton = new QPushButton("Düzenle", this);
    deleteButton = new QPushButton("Sil", this);
    bulkEditButton = new QPushButton("Toplu Düzenle", this);
    bulkDeleteButton = new QPushButton("Toplu Sil", this);
    viewLogsButton = new QPushButton("Logları Görüntüle", this);
    
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(bulkEditButton);
    buttonLayout->addWidget(bulkDeleteButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(viewLogsButton);

    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(productTable);
    mainLayout->addLayout(buttonLayout);

    setWindowTitle("Barkod Takip Sistemi");
    resize(1200, 600);
}

void MainWindow::setupConnections()
{
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addProduct);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::editSelectedProduct);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedProduct);
    connect(bulkEditButton, &QPushButton::clicked, this, &MainWindow::showBulkEditDialog);
    connect(bulkDeleteButton, &QPushButton::clicked, this, &MainWindow::showBulkDeleteDialog);
    connect(viewLogsButton, &QPushButton::clicked, this, &MainWindow::showLogViewerDialog);
    
    // Enter tuşuna basıldığında ürün ekleme
    connect(barcodeEdit, &QLineEdit::returnPressed, this, &MainWindow::addProduct);
    connect(brandEdit, &QLineEdit::returnPressed, this, &MainWindow::addProduct);
    
    // Tablo çift tıklama ile düzenleme
    connect(productTable, &QTableWidget::doubleClicked, this, &MainWindow::editSelectedProduct);
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
        Logger::instance().log(Logger::INFO, QString("%1 ürün başarıyla kaydedildi.").arg(products.size()));
    } else {
        Logger::instance().log(Logger::ERROR, "Ürünler kaydedilirken hata oluştu.");
        QMessageBox::warning(this, "Hata", "Ürünler kaydedilirken bir hata oluştu.");
    }
}

void MainWindow::loadProducts()
{
    QFile file(getDataFilePath());
    if (!file.exists()) {
        Logger::instance().log(Logger::INFO, "İlk çalıştırma: Ürün dosyası henüz oluşturulmamış.");
        return;
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
            Logger::instance().log(Logger::INFO, QString("%1 ürün başarıyla yüklendi.").arg(products.size()));
            updateTable();
        }
    } else {
        Logger::instance().log(Logger::ERROR, "Ürünler yüklenirken hata oluştu.");
        QMessageBox::warning(this, "Hata", "Ürünler yüklenirken bir hata oluştu.");
    }
}

void MainWindow::addProduct()
{
    QString barcode = barcodeEdit->text().trimmed();
    QString name = nameEdit->text().trimmed();
    QString brand = brandEdit->text().trimmed();
    
    if (barcode.isEmpty()) {
        Logger::instance().log(Logger::WARNING, "Boş barkod ile ürün ekleme denemesi.");
        QMessageBox::warning(this, "Hata", "Lütfen bir barkod numarası girin.");
        return;
    }

    QDate expiryDate = expiryDateEdit->date();
    products.emplace_back(barcode, name, brand, expiryDate);
    
    Logger::instance().log(Logger::INFO, QString("Yeni ürün eklendi: Barkod=%1, Ad=%2, Marka=%3, SKT=%4")
        .arg(barcode)
        .arg(name)
        .arg(brand)
        .arg(expiryDate.toString("dd.MM.yyyy")));
    
    updateTable();
    saveProducts();
    
    barcodeEdit->clear();
    nameEdit->clear();
    brandEdit->clear();
    barcodeEdit->setFocus();
}

void MainWindow::editSelectedProduct()
{
    int row = productTable->currentRow();
    if (row >= 0) {
        QString selectedBarcode = productTable->item(row, 0)->text();
        
        // Seçilen barkoda sahip ürünü bul
        auto it = std::find_if(products.begin(), products.end(),
            [&selectedBarcode](const Product& p) { return p.getBarcode() == selectedBarcode; });
            
        if (it != products.end()) {
            size_t index = std::distance(products.begin(), it);
            
            EditDialog dialog(products[index].getBarcode(), products[index].getName(), 
                            products[index].getBrand(), products[index].getExpiryDate(), this);
                            
            if (dialog.exec() == QDialog::Accepted) {
                QString oldBarcode = products[index].getBarcode();
                QString oldName = products[index].getName();
                QString oldBrand = products[index].getBrand();
                QDate oldDate = products[index].getExpiryDate();
                
                products[index].setBarcode(dialog.getBarcode());
                products[index].setName(dialog.getName());
                products[index].setBrand(dialog.getBrand());
                products[index].setExpiryDate(dialog.getExpiryDate());
                
                Logger::instance().log(Logger::INFO, QString("Ürün düzenlendi: Barkod: %1->%2, Ad: %3->%4, Marka: %5->%6, SKT: %7->%8")
                    .arg(oldBarcode)
                    .arg(dialog.getBarcode())
                    .arg(oldName)
                    .arg(dialog.getName())
                    .arg(oldBrand)
                    .arg(dialog.getBrand())
                    .arg(oldDate.toString("dd.MM.yyyy"))
                    .arg(dialog.getExpiryDate().toString("dd.MM.yyyy")));
                
                updateTable();
                saveProducts();
            }
        }
    } else {
        Logger::instance().log(Logger::WARNING, "Ürün seçilmeden düzenleme denemesi.");
        QMessageBox::warning(this, "Hata", "Lütfen düzenlenecek bir ürün seçin.");
    }
}

void MainWindow::deleteSelectedProduct()
{
    int row = productTable->currentRow();
    if (row >= 0) {
        QString selectedBarcode = productTable->item(row, 0)->text();
        
        // Seçilen barkoda sahip ürünü bul
        auto it = std::find_if(products.begin(), products.end(),
            [&selectedBarcode](const Product& p) { return p.getBarcode() == selectedBarcode; });
            
        if (it != products.end()) {
            QString barcode = it->getBarcode();
            QString name = it->getName();
            QString brand = it->getBrand();
            QDate expiryDate = it->getExpiryDate();
            
            products.erase(it);
            
            Logger::instance().log(Logger::INFO, QString("Ürün silindi: Barkod=%1, Ad=%2, Marka=%3, SKT=%4")
                .arg(barcode)
                .arg(name)
                .arg(brand)
                .arg(expiryDate.toString("dd.MM.yyyy")));
            
            updateTable();
            saveProducts();
        }
    }
}

void MainWindow::showBulkEditDialog()
{
    if (products.empty()) {
        Logger::instance().log(Logger::WARNING, "Boş ürün listesi ile toplu düzenleme denemesi.");
        QMessageBox::warning(this, "Uyarı", "Düzenlenecek ürün bulunmamaktadır.");
        return;
    }

    BulkEditDialog dialog(products, this);
    if (dialog.exec() == QDialog::Accepted) {
        Logger::instance().log(Logger::INFO, "Toplu düzenleme yapıldı.");
        updateTable();
        saveProducts();
    }
}

void MainWindow::showBulkDeleteDialog()
{
    if (products.empty()) {
        Logger::instance().log(Logger::WARNING, "Boş ürün listesi ile toplu silme denemesi.");
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
            Logger::instance().log(Logger::INFO, QString("%1 ürün toplu olarak silindi.").arg(selectedIndexes.size()));
            
            // Sondan başa doğru silme işlemi yapılmalı
            for (auto it = selectedIndexes.rbegin(); it != selectedIndexes.rend(); ++it) {
                products.erase(products.begin() + *it);
            }
            updateTable();
            saveProducts();
        }
    }
}

void MainWindow::showLogViewerDialog()
{
    LogViewerDialog dialog(this);
    dialog.exec();
}

void MainWindow::updateTable()
{
    // Mevcut sıralama durumunu kaydet
    int sortColumn = productTable->horizontalHeader()->sortIndicatorSection();
    Qt::SortOrder sortOrder = productTable->horizontalHeader()->sortIndicatorOrder();
    
    productTable->setSortingEnabled(false);
    productTable->setRowCount(products.size());
    
    for (size_t i = 0; i < products.size(); ++i) {
        QTableWidgetItem *barcodeItem = new QTableWidgetItem(products[i].getBarcode());
        QTableWidgetItem *nameItem = new QTableWidgetItem(products[i].getName());
        QTableWidgetItem *brandItem = new QTableWidgetItem(products[i].getBrand());
        
        // Tarih sıralaması için özel veri rolü
        QTableWidgetItem *dateItem = new QTableWidgetItem;
        dateItem->setText(products[i].getExpiryDate().toString("dd.MM.yyyy"));
        
        // Kalan gün sıralaması için özel sıralama sınıfını kullan
        int daysUntilExpiry = products[i].getDaysUntilExpiry();
        CustomTableWidgetItem *daysItem = new CustomTableWidgetItem(QString::number(daysUntilExpiry));
        
        QColor color = getExpiryColor(daysUntilExpiry);
        if (color.isValid()) {
            barcodeItem->setBackground(color);
            nameItem->setBackground(color);
            brandItem->setBackground(color);
            dateItem->setBackground(color);
            daysItem->setBackground(color);
            
            if (daysUntilExpiry < 10) {
                barcodeItem->setForeground(Qt::white);
                nameItem->setForeground(Qt::white);
                brandItem->setForeground(Qt::white);
                dateItem->setForeground(Qt::white);
                daysItem->setForeground(Qt::white);
            } else if (daysUntilExpiry < 30) {
                barcodeItem->setForeground(Qt::black);
                nameItem->setForeground(Qt::black);
                brandItem->setForeground(Qt::black);
                dateItem->setForeground(Qt::black);
                daysItem->setForeground(Qt::black);
            }
        }
        
        productTable->setItem(i, 0, barcodeItem);
        productTable->setItem(i, 1, nameItem);
        productTable->setItem(i, 2, brandItem);
        productTable->setItem(i, 3, dateItem);
        productTable->setItem(i, 4, daysItem);
    }

    productTable->resizeColumnsToContents();
    productTable->setSortingEnabled(true);
    
    // Eğer daha önce bir sıralama yapılmışsa, aynı sıralamayı uygula
    if (sortColumn >= 0) {
        productTable->sortByColumn(sortColumn, sortOrder);
    }
} 
