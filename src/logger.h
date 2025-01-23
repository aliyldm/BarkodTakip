#pragma once

#include <QString>
#include <QFile>
#include <QDir>
#include <QDateTime>

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
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    QString logDirectory;
    const qint64 MAX_LOG_SIZE = 5 * 1024 * 1024; // 5MB
    const int MAX_LOG_FILES = 30;

    QString getCurrentLogFileName() const;
    QString getLevelString(LogLevel level) const;
    void ensureLogDirectoryExists();
    void rotateLogFiles();
}; 