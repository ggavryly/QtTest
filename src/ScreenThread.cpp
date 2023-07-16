#include "ScreenThread.h"

ScreenThread::ScreenThread(QObject *parent) : QThread(parent)
{

}

void ScreenThread::run()
{
    while (!isInterruptionRequested())
    {
        QScreen *screen = QGuiApplication::primaryScreen();
        QPixmap screenshot = screen->grabWindow(0);

        screenshot = screenshot.scaled(300, 200);

        QPixmap previousScreenshot;
        if (!m_screenshots.isEmpty())
            previousScreenshot = m_screenshots.last();

        if (!previousScreenshot.isNull())
        {
            double similarity = compareScreenshots(screenshot, previousScreenshot);
            emit screenshotTaken(screenshot, similarity);
        }
        else
        {
            emit screenshotTaken(screenshot, 0);
        }

        m_screenshots.append(screenshot);

        QThread::msleep(60000); // Take a screenshot every minute
    }
}

double ScreenThread::compareScreenshots(const QPixmap &screenshot1, const QPixmap &screenshot2)
{
    const QImage & currentScreenshot = screenshot1.toImage();
    const QImage & previousScreenshot = screenshot2.toImage();

    double percentage = 0.f;
    QSize size = {currentScreenshot.width(), currentScreenshot.height()};

    for (int i = 0; i < currentScreenshot.width(); i++)
    {
        for (int j = 0; j < currentScreenshot.height(); j++)
        {
            QRgb currentRgb = currentScreenshot.pixel(i, j);
            QRgb previousRgb = previousScreenshot.pixel(i, j);

            double diffRed = qFabs(QColor(currentRgb).red() - QColor(previousRgb).red()) / 255;
            double diffGreen = qFabs(QColor(currentRgb).green() - QColor(previousRgb).green()) / 255;
            double diffBlue = qFabs(QColor(currentRgb).blue() - QColor(previousRgb).blue()) / 255;

            double diffRGB = (diffRed + diffGreen + diffBlue) / 3 * 100;

            percentage += diffRGB;

        }
    }

    percentage = 100.0 - (percentage / (size.width() * size.height()));

    return percentage;
}
