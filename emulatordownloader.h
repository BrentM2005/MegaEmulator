#ifndef EMULATORDOWNLOADER_H
#define EMULATORDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>

class EmulatorDownloader : public QObject {
    Q_OBJECT

public:
    explicit EmulatorDownloader(const QString &downloadPath, QObject *parent = nullptr);

    void downloadEmulator(const QString &emulatorName, const QString &url);
    void cancelDownload();

signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished(bool success, const QString &filePath);
    void extractionProgress(int percentage);
    void extractionFinished(bool success);
    void statusMessage(const QString &message);

private slots:
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadFinished();
    void onDownloadError(QNetworkReply::NetworkError error);

private:
    void extractZip(const QString &zipPath, const QString &destPath);

    QNetworkAccessManager *networkManager;
    QNetworkReply *currentReply;
    QString downloadPath;
    QString currentEmulatorName;
    QString currentFilePath;
};

#endif // EMULATORDOWNLOADER_H
