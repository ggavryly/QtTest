#pragma once

#include "ScreenThread.h"
#include <QGridLayout>
#include <QPushButton>
#include <QScreen>
#include <QDir>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

class Window : public QWidget
{
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
    ~Window();

private slots:
    void handleScreenshotTaken(const QPixmap &screenshot, double similarity);
    void toggleScreenThread();

private:
    bool createDatabase();
    void saveScreenshotToDatabase(const QPixmap &screenshot, double similarity);
    void loadSavedScreenshots();
    void addNewLabel(const QPixmap & label, double similarity);

    QLabel *m_imageLabel;
    QLabel *m_similarityLabel;

    QGridLayout *m_gridLayout;

    QPushButton *m_startButton;

    ScreenThread *m_screenshotThread;
};
