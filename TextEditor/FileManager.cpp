//
// 22/07/2019.
//

#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QFile>
#include <QtCore/QTextStream>
#include "FileManager.h"
#include <stdexcept>
#include <QJsonObject>
#include <QByteArray>
#include <QJsonDocument>
#include <iostream>

#define documents_folder_qs QString("documents")
#define data_folder_qs QString("data")
#define doc_file_suffix_qs QString("-doc")
#define meta_file_suffix_qs QString("-meta")
#define doc_format_qs QString(".json")
#define meta_doc_format_qs QString(".json")
#define settings_file_qs QString("settings")
//#define account_file_qs QString("account")
#define settings_format_qs QString(".json")
//#define account_format_qs QString(".json")

//const QString FileManager::doc_format = QString(".pdsdoc");
//const QString FileManager::meta_doc_format = QString(".pdsmeta");

/*FileManager& FileManager::getInstance() {
    static FileManager instance{};
    return instance;
}*/

FileManager::FileManager(const QString &appPath) {
    _app_path = QString(appPath);
}

/*void FileManager::setAppPath(const QString &path) {
    _app_path = QString(path);
}*/

Document *FileManager::loadDocument(const QString &docId, const unsigned int siteId, QObject *parent) {
    QString documents_path = _app_path;
    QDir dir(documents_path);
    if (!dir.exists())
        dir.mkpath(documents_path);
    if (!dir.exists(documents_folder_qs))
        dir.mkdir(documents_folder_qs);

    dir.cd(documents_folder_qs);
    documents_path = dir.absoluteFilePath(docId + doc_file_suffix_qs + doc_format_qs);
    if (!QFile::exists(documents_path))
        throw std::invalid_argument("file not found");

    QFile file(documents_path);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        throw std::invalid_argument("cannot open the file");
    }

    QTextStream in(&file);
    QString text = in.readAll();
    file.close();

    QJsonDocument qjd = QJsonDocument::fromJson(text.toUtf8());
    QJsonObject qjo = qjd.object();
    return new Document(qjo, siteId, parent);
}

MetaDocument FileManager::loadMetaData(const QString &docId, const unsigned int siteId) {
    QString documents_path = _app_path;
    QDir dir(documents_path);
    if (!dir.exists())
        dir.mkpath(documents_path);
    if (!dir.exists(documents_folder_qs))
        dir.mkdir(documents_folder_qs);

    dir.cd(documents_folder_qs);
    documents_path = dir.absoluteFilePath(docId + meta_file_suffix_qs + meta_doc_format_qs);
    if (!QFile::exists(documents_path))
        throw std::invalid_argument("file not found");

    QFile file(documents_path);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        throw std::invalid_argument("cannot open the file");
    }

    QTextStream in(&file);
    QString text = in.readAll();
    file.close();

    QJsonDocument qjd = QJsonDocument::fromJson(text.toUtf8());
    QJsonObject qjo = qjd.object();
    return MetaDocument(qjo);
}

void FileManager::storeMetaData(const QString &docId, const MetaDocument &metaDocument) {
    QString documents_path = _app_path;
    QDir dir(documents_path);
    if (!dir.exists())
        dir.mkpath(documents_path);
    if (!dir.exists(documents_folder_qs))
        dir.mkdir(documents_folder_qs);

    dir.cd(documents_folder_qs);
    documents_path = dir.absoluteFilePath(docId + meta_file_suffix_qs + meta_doc_format_qs);
    if (QFile::exists(documents_path))
        QFile::remove(documents_path);

    QFile file(documents_path);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        return;
    }
    QTextStream out(&file);
    out << metaDocument.toBytes();
    file.close();
}

void FileManager::storeDocument(const Document &document) {
    QString documents_path = _app_path;
    QDir dir(documents_path);
    if (!dir.exists())
        dir.mkpath(documents_path);
    if (!dir.exists(documents_folder_qs))
        dir.mkdir(documents_folder_qs);

    dir.cd(documents_folder_qs);
    documents_path = dir.absoluteFilePath(document.getId() + doc_file_suffix_qs + doc_format_qs);
    if (QFile::exists(documents_path))
        QFile::remove(documents_path);

    QFile file(documents_path);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        return;
    }
    QTextStream out(&file);
    out << document.toBytes();
    file.close();
}

FileManager::~FileManager() {
    //std::cout << "~FileManager()" << std::endl;
}

void FileManager::storeSettings(const Settings &settings) {
    QString settings_path = _app_path;
    QDir dir(settings_path);
    if (!dir.exists())
        dir.mkpath(settings_path);
    if (!dir.exists(data_folder_qs))
        dir.mkdir(data_folder_qs);

    dir.cd(data_folder_qs);
    settings_path = dir.absoluteFilePath(settings_file_qs + settings_format_qs);
    if (QFile::exists(settings_path))
        QFile::remove(settings_path);

    QFile file(settings_path);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        return;
    }
    QTextStream out(&file);
    out << settings.toBytes();
    file.close();
}

QJsonObject FileManager::loadSettings() {
    QString settings_path = _app_path;
    QDir dir(settings_path);
    if (!dir.exists())
        dir.mkpath(settings_path);
    if (!dir.exists(data_folder_qs))
        dir.mkdir(data_folder_qs);

    dir.cd(data_folder_qs);
    settings_path = dir.absoluteFilePath(settings_file_qs + settings_format_qs);
    if (!QFile::exists(settings_path)) {
        //throw std::invalid_argument("file not found");
        return QJsonObject{};//std::make_shared<Settings>();
    }
    QFile file(settings_path);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        //throw std::invalid_argument("cannot open the file");
        return QJsonObject{};//std::make_shared<Settings>();
    }

    QTextStream in(&file);
    QString text = in.readAll();
    file.close();

    QJsonDocument qjd = QJsonDocument::fromJson(text.toUtf8());
    QJsonObject qjo = qjd.object();
    return qjo;//std::make_shared<Settings>(qjo);
}