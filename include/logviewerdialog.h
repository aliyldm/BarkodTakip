#pragma once

#include <QDialog>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QFile>
#include <QDir>

class LogViewerDialog : public QDialog {
    Q_OBJECT
public:
    explicit LogViewerDialog(QWidget *parent = nullptr);

private slots:
    void refreshLogs();
    void loadSelectedLog();
    void clearLogs();

private:
    void setupUI();
    void loadLogFiles();
    QString getLogDirectory() const;

    QComboBox *logFileCombo;
    QTextEdit *logViewer;
    QPushButton *refreshButton;
    QPushButton *clearButton;
}; 