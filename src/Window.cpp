#include "Window.h"

Window::Window(QWidget *parent) : QWidget(parent)
{
    m_screenshotThread = new ScreenThread(this);
    connect(m_screenshotThread, &ScreenThread::screenshotTaken, this, &Window::handleScreenshotTaken);

    m_gridLayout = new QGridLayout(this);
    m_imageLabel = new QLabel(this);
    m_similarityLabel = new QLabel(this);
    m_similarityLabel->setAlignment(Qt::AlignCenter);
    m_startButton = new QPushButton("Start", this);

    m_gridLayout->addWidget(m_imageLabel, 0, 0);
    m_gridLayout->addWidget(m_similarityLabel, 0, 1);
    m_gridLayout->addWidget(m_startButton, 1, 0, 1, 2);

    connect(m_startButton, &QPushButton::clicked, this, &Window::toggleScreenThread);

    if (!createDatabase())
        QMessageBox::critical(this, "Error", "Failed to create or open the database.");

    loadSavedScreenshots();

    setLayout(m_gridLayout);
}

Window::~Window() {
    if (m_screenshotThread->isRunning())
        m_screenshotThread->requestInterruption();

    m_screenshotThread->wait();
}

void Window::handleScreenshotTaken(const QPixmap &screenshot, double similarity)
{
    m_similarityLabel->setText(QString::number(similarity, 'f', 2)  + "%");

    m_imageLabel->setPixmap(screenshot.scaled(300, 200));
    m_imageLabel->setAlignment(Qt::AlignCenter);

    addNewLabel(screenshot.scaled(300, 200), similarity);

    // Save the screenshot, hash sum, and similarity percentage in the database
    saveScreenshotToDatabase(m_imageLabel->pixmap(Qt::ReturnByValue).scaled(300, 200), similarity);
}

void Window::toggleScreenThread()
{
    if (m_screenshotThread->isRunning()) {
        m_screenshotThread->requestInterruption();
        m_startButton->setText("Start");
    } else {
        m_screenshotThread->start();
        m_startButton->setText("Stop");
    }
}

bool Window::createDatabase()
{
    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("screenshots.db");

    if (!database.open())
        return false;

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS screenshots ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "timestamp TEXT,"
               "hash_sum TEXT,"
               "similarity REAL,"
               "imagedata BLOB)");

    return !query.lastError().isValid();
}

void Window::saveScreenshotToDatabase(const QPixmap &screenshot, double similarity)
{
    QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);

    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    screenshot.save(&buffer, "PNG");
    QString hashSum = QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex();

    QSqlQuery query;
    query.prepare("INSERT INTO screenshots (timestamp, hash_sum, similarity, imagedata) VALUES (?, ?, ?, ?)");
    query.addBindValue(timestamp);
    query.addBindValue(hashSum);
    query.addBindValue(similarity);
    query.addBindValue(bytes);
    query.exec();
}

void Window::loadSavedScreenshots()
{
    QSqlQuery query;
    query.exec("SELECT * FROM screenshots ORDER BY id DESC");

    while (query.next()) {
        QString timestamp = query.value("timestamp").toString();
        QString hashSum = query.value("hash_sum").toString();
        double similarity = query.value("similarity").toDouble();
        QByteArray bytes = query.value("imagedata").toByteArray();

        // Load the saved screenshots and display them in the grid view
        QPixmap screenshot;
        screenshot.loadFromData(bytes, "PNG");

        addNewLabel(screenshot, similarity);
    }
}

void Window::addNewLabel(const QPixmap & screenshot, double similarity)
{
    static int rowCaclucation = 0;

    QLabel *imageLabel = new QLabel(this);
    imageLabel->setPixmap(screenshot);

    QLabel *similarityLabel = new QLabel(QString::number(similarity, 'f', 2) + "%", this);
    similarityLabel->setAlignment(Qt::AlignBottom);

    int rowCount = (!rowCaclucation) ? (m_gridLayout->rowCount()) : (m_gridLayout->rowCount() - 1);

    if ((rowCaclucation % 3) == 0)
        rowCount += 1;

    int columnCount = rowCaclucation % 3;

    m_gridLayout->addWidget(imageLabel, rowCount, columnCount);
    m_gridLayout->addWidget(similarityLabel, rowCount, columnCount);

    rowCaclucation++;
}
