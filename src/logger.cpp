#include "logger.h"
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDebug>

Logger::Logger() {
    // Log dizinini AppData klasörü altında oluştur
    logDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    qDebug() << "Log dizini:" << logDirectory;
    ensureLogDirectoryExists();
}

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::log(LogLevel level, const QString& message) {
    QString logFileName = getCurrentLogFileName();
    QFile logFile(logFileName);

    qDebug() << "Log dosyası yazılıyor:" << logFileName;

    // Log dosyasının boyutunu kontrol et
    if (logFile.exists() && logFile.size() > MAX_LOG_SIZE) {
        rotateLogFiles();
        logFile.setFileName(getCurrentLogFileName());
    }

    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&logFile);
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        QString logEntry = QString("[%1] [%2] %3\n")
            .arg(timestamp)
            .arg(getLevelString(level))
            .arg(message);
        
        stream << logEntry;
        logFile.close();

        // Konsola da yazdır (debug için)
        qDebug().noquote() << "Log yazıldı:" << logEntry.trimmed();
    } else {
        qDebug() << "Log dosyası açılamadı:" << logFile.errorString();
    }
}

void Logger::setLogDirectory(const QString& dir) {
    logDirectory = dir;
    ensureLogDirectoryExists();
}

QString Logger::getCurrentLogFileName() const {
    return QString("%1/app_%2.log")
        .arg(logDirectory)
        .arg(QDate::currentDate().toString("yyyy-MM-dd"));
}

QString Logger::getLevelString(LogLevel level) const {
    switch (level) {
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case WARNING: return "WARNING";
        case ERROR: return "ERROR";
        case CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

void Logger::ensureLogDirectoryExists() {
    QDir dir(logDirectory);
    if (!dir.exists()) {
        bool created = dir.mkpath(".");
        qDebug() << "Log dizini oluşturuldu:" << created;
        if (!created) {
            qDebug() << "Log dizini oluşturulamadı!";
        }
    }
}

void Logger::rotateLogFiles() {
    QDir dir(logDirectory);
    QStringList logFiles = dir.entryList(QStringList() << "app_*.log", QDir::Files, QDir::Time);

    // Eski log dosyalarını sil
    while (logFiles.size() >= MAX_LOG_FILES) {
        dir.remove(logFiles.last());
        logFiles.removeLast();
    }

    // Mevcut log dosyasını yeniden adlandır
    QString currentLog = getCurrentLogFileName();
    if (QFile::exists(currentLog)) {
        QString newName = QString("%1/app_%2_%3.log")
            .arg(logDirectory)
            .arg(QDate::currentDate().toString("yyyy-MM-dd"))
            .arg(QTime::currentTime().toString("hhmmss"));
        QFile::rename(currentLog, newName);
    }
} 