#pragma once

#include <QtWidgets>
#include <QApplication>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QScreen>
#include <QThread>
#include <QtMath>

class ScreenThread : public QThread
{
    Q_OBJECT

public:
    ScreenThread(QObject *parent = nullptr);

signals:
    void screenshotTaken(const QPixmap &screenshot, double similarity);

protected:
    void run() override;

private:
    double compareScreenshots(const QPixmap &screenshot1, const QPixmap &screenshot2);

    QVector<QPixmap> m_screenshots;
};
