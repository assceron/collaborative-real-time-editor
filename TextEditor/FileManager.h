//
// 22/07/2019.
//

#ifndef TEXTEDITOR_FILEMANAGER_H
#define TEXTEDITOR_FILEMANAGER_H

class Document;
class Settings;

#include "Document.h"
#include <QtCore/QString>
#include "MetaDocument.h"
#include "Settings.h"
#include <QJsonObject>

// see meyers/alexandrescu
class FileManager {
public:
    //static FileManager &getInstance();
    explicit FileManager(const QString &appPath);
    FileManager() = delete;
    ~FileManager();
    void storeDocument(const Document &document);
    void storeMetaData(const QString &docId, const MetaDocument &metaDocument);
    Document *loadDocument(const QString &docId, const unsigned int siteId, QObject *parent = nullptr);
    MetaDocument loadMetaData(const QString &docId, const unsigned int siteId);
    void storeSettings(const Settings &settings);
    QJsonObject loadSettings();
    //void setAppPath(const QString &path);
    FileManager(FileManager const&) = delete;
    void operator=(FileManager const&) = delete;
private:
    QString _app_path = QString();
};


#endif //TEXTEDITOR_FILEMANAGER_H
