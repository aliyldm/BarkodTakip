#pragma once

#include <QString>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QTextStream>
#include <QDebug>

class Logger {
public:
    enum LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };

    static Logger& instance();
    void log(LogLevel level, const QString& message);
    void setLogDirectory(const QString& dir);

private:
    Logger();
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    QString getCurrentLogFileName() const;
    QString getLevelString(LogLevel level) const;
    void ensureLogDirectoryExists();
    void rotateLogFiles();

    QString logDirectory;
    static const int MAX_LOG_FILES = 30; // Maksimum log dosyası sayısı
    static const qint64 MAX_LOG_SIZE = 10 * 1024 * 1024; // Maksimum log dosyası boyutu (10MB)
}; 