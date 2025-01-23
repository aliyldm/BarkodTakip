#include "logger.h"
#include <QStandardPaths>

Logger::Logger() {
    // Varsayılan log dizinini ayarla
    logDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    ensureLogDirectoryExists();
}

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::log(LogLevel level, const QString& message) {
    QString logFileName = getCurrentLogFileName();
    QFile logFile(logFileName);

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
        qDebug().noquote() << logEntry.trimmed();
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
        dir.mkpath(".");
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