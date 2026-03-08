#include "emulatordownloader.h"
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QCoreApplication>

EmulatorDownloader::EmulatorDownloader(const QString &downloadPath, QObject *parent)
    : QObject(parent), networkManager(new QNetworkAccessManager(this)),
    currentReply(nullptr), downloadPath(downloadPath) {
}

void EmulatorDownloader::downloadEmulator(const QString &emulatorName, const QString &url) {
    if (currentReply) {
        emit statusMessage("Download already in progress");
        return;
    }

    currentEmulatorName = emulatorName;

    QString extension;
    if (emulatorName.contains('.')) {
        currentFilePath = downloadPath + "/" + emulatorName;
    } else if (url.contains(".zip")) {
        currentFilePath = downloadPath + "/" + emulatorName + ".zip";
    } else {
        QUrl qurl(url);
        QString path = qurl.path();
        extension = QFileInfo(path).suffix();
        currentFilePath = downloadPath + "/" + emulatorName + "." + extension;
    }

    QDir().mkpath(downloadPath);

    emit statusMessage("Starting download: " + emulatorName);

    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);

    currentReply = networkManager->get(request);

    connect(currentReply, &QNetworkReply::downloadProgress,
            this, &EmulatorDownloader::onDownloadProgress);
    connect(currentReply, &QNetworkReply::finished,
            this, &EmulatorDownloader::onDownloadFinished);
    connect(currentReply, &QNetworkReply::errorOccurred,
            this, &EmulatorDownloader::onDownloadError);
}

void EmulatorDownloader::cancelDownload() {
    if (currentReply) {
        currentReply->abort();
        currentReply->deleteLater();
        currentReply = nullptr;
        emit statusMessage("Download cancelled");
    }
}

void EmulatorDownloader::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    emit downloadProgress(bytesReceived, bytesTotal);
}

void EmulatorDownloader::onDownloadFinished() {
    if (!currentReply) return;

    if (currentReply->error() == QNetworkReply::NoError) {
        QFile file(currentFilePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(currentReply->readAll());
            file.close();

            if (currentFilePath.endsWith(".zip", Qt::CaseInsensitive)) {
                emit statusMessage("Download complete.");

                QString extractPath = downloadPath + "/" + currentEmulatorName;
                QDir().mkpath(extractPath);
                extractZip(currentFilePath, extractPath);
            } else {
                emit statusMessage("Download complete!");
                emit downloadFinished(true, currentFilePath);
                emit extractionFinished(true);
            }
        } else {
            emit statusMessage("Failed to save file");
            emit downloadFinished(false, "");
        }
    }

    currentReply->deleteLater();
    currentReply = nullptr;
}

void EmulatorDownloader::onDownloadError(QNetworkReply::NetworkError error) {
    QString errorMsg = "Download error: " + currentReply->errorString();
    emit statusMessage(errorMsg);
    emit downloadFinished(false, "");

    if (currentReply) {
        currentReply->deleteLater();
        currentReply = nullptr;
    }
}

void EmulatorDownloader::extractZip(const QString &zipPath, const QString &destPath) {
    emit statusMessage("Extracting files...");

    QProcess *extractProcess = new QProcess(this);

    connect(extractProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, zipPath, extractProcess](int exitCode, QProcess::ExitStatus exitStatus) {
                if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
                    // Delete the zip file after successful extraction
                    QFile::remove(zipPath);
                    emit statusMessage("Installation complete!");
                    emit extractionFinished(true);
                } else {
                    emit statusMessage("Extraction failed. Please extract manually.");
                    emit extractionFinished(false);
                }
                extractProcess->deleteLater();
            });

    QString command = QString("powershell.exe");
    QStringList args;
    args << "-Command"
         << QString("Expand-Archive -Path '%1' -DestinationPath '%2' -Force")
                .arg(zipPath).arg(destPath);

    extractProcess->start(command, args);
}
