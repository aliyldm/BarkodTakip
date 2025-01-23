#include "logviewerdialog.h"
#include <QStandardPaths>
#include <QMessageBox>
#include <QCoreApplication>

LogViewerDialog::LogViewerDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Log Görüntüleyici");
    setupUI();
    loadLogFiles();
    resize(800, 600);
}

void LogViewerDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Üst kısım - Log dosyası seçimi ve butonlar
    QHBoxLayout *topLayout = new QHBoxLayout;
    
    QLabel *label = new QLabel("Log Dosyası:", this);
    logFileCombo = new QComboBox(this);
    refreshButton = new QPushButton("Yenile", this);
    clearButton = new QPushButton("Temizle", this);
    
    topLayout->addWidget(label);
    topLayout->addWidget(logFileCombo, 1);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(clearButton);

    // Log görüntüleyici
    logViewer = new QTextEdit(this);
    logViewer->setReadOnly(true);
    QFont font("Consolas", 10);
    logViewer->setFont(font);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(logViewer);

    // Bağlantılar
    connect(refreshButton, &QPushButton::clicked, this, &LogViewerDialog::refreshLogs);
    connect(clearButton, &QPushButton::clicked, this, &LogViewerDialog::clearLogs);
    connect(logFileCombo, &QComboBox::currentTextChanged, this, &LogViewerDialog::loadSelectedLog);
}

QString LogViewerDialog::getLogDirectory() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
}

void LogViewerDialog::loadLogFiles()
{
    QString currentSelection = logFileCombo->currentText();
    logFileCombo->clear();

    QDir logDir(getLogDirectory());
    if (!logDir.exists()) {
        logViewer->setText("Log dizini bulunamadı.");
        return;
    }

    QStringList logFiles = logDir.entryList(QStringList() << "app_*.log", QDir::Files, QDir::Time);
    logFileCombo->addItems(logFiles);

    // Önceki seçimi koru
    int index = logFileCombo->findText(currentSelection);
    if (index >= 0) {
        logFileCombo->setCurrentIndex(index);
    } else if (!logFiles.isEmpty()) {
        logFileCombo->setCurrentIndex(0); // En son log dosyasını seç
    }
}

void LogViewerDialog::loadSelectedLog()
{
    QString selectedFile = logFileCombo->currentText();
    if (selectedFile.isEmpty()) {
        logViewer->clear();
        return;
    }

    QFile logFile(getLogDirectory() + "/" + selectedFile);
    if (logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&logFile);
        QString content = in.readAll();
        logViewer->setText(content);
        logFile.close();

        // En son satıra kaydır
        QTextCursor cursor = logViewer->textCursor();
        cursor.movePosition(QTextCursor::End);
        logViewer->setTextCursor(cursor);
    } else {
        logViewer->setText("Log dosyası açılamadı.");
    }
}

void LogViewerDialog::refreshLogs()
{
    loadLogFiles();
    loadSelectedLog();
}

void LogViewerDialog::clearLogs()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Log Dosyalarını Temizle",
        "Tüm log dosyaları silinecek. Emin misiniz?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        QDir logDir(getLogDirectory());
        QStringList logFiles = logDir.entryList(QStringList() << "app_*.log", QDir::Files);
        
        for (const QString &file : logFiles) {
            logDir.remove(file);
        }
        
        logFileCombo->clear();
        logViewer->clear();
        logViewer->setText("Tüm log dosyaları temizlendi.");
    }
} 