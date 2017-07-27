#include "normalize.h"

Normalize::Normalize(int id, QObject *parent) : QObject(parent) {
    this->id = id;
}

void Normalize::start() {
    _errno = 1;
    if (fileName.length()) {
        QDomDocument doc("mydocument");
        QFile originalFile(fileName);
        QFileInfo originalFileInfo(fileName);
        if (!originalFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Не удалось открыть файл";
            emit sendMessage("Не удалось открыть файл");
            return;
        }
        // Parse file
        emit sendLabel("Парсинг файла");
        emit(sendProgress(0));
        if (!doc.setContent(&originalFile)) {
            qDebug() << "Не удалось распознать содержимое";
            emit sendMessage("Не удалось распознать содержимое");
            originalFile.close();
            return;
        }
        emit(sendProgress(100));
        originalFile.close();

        // rename "dataroot" ro "root"
        QDomNodeList roots = doc.elementsByTagName("dataroot");
        if (roots.size() > 0) {
            QDomElement root = roots.at(0).toElement();
            root.setTagName("root");
        }
        else {
            qDebug() << "Возможно, указанный файл уже обрабатывался";
            emit sendMessage("Возможно, указанный файл уже обрабатывался");
            return;
        }

        // remove "ApplicationID" tags // TODO: Разделить цикл на 4 потока
        emit sendLabel("Удаление ApplicationID");
        QDomNodeList applicationIDs = doc.elementsByTagName("ApplicationID");
        int summary = applicationIDs.size();
        for (int i = applicationIDs.size() - 1; i >= 0; i--) {
            applicationIDs.at(i).toElement().parentNode().removeChild(applicationIDs.at(i));
            emit(sendProgress(((summary - i) * 100) / summary));
        }

        // insert login pass info to "root"
        emit sendLabel("Добавление данных авторизации");
        emit(sendProgress(0));
        QDomElement root = doc.elementsByTagName("root").at(0).toElement();
        QDomElement authData = doc.createElement("AuthData");
        authData.setAttribute("xmlns:xs", "http://www.w3.org/2001/XMLSchema");
        root.appendChild(authData);
        emit(sendProgress(25));
        QDomElement login = doc.createElement("Login");
        QDomText loginText = doc.createTextNode("dgma-pk@mail.ru");
        login.appendChild(loginText);
        emit(sendProgress(50));
        QDomElement pass = doc.createElement("Pass");
        QDomText passText = doc.createTextNode("Fath1436");
        pass.appendChild(passText);
        emit(sendProgress(75));
        authData.appendChild(login);
        authData.appendChild(pass);
        emit(sendProgress(100));

        emit sendLabel("Добавление элемента PackageData");
        emit(sendProgress(0));
        QDomElement packageData = doc.createElement("PackageData");
        packageData.setAttribute("xmlns:xs", "http://www.w3.org/2001/XMLSchema");
        root.appendChild(packageData);
        emit(sendProgress(100));

        emit sendLabel("Оборачивание заявлений в тег Applications");
        emit(sendProgress(0));
        QDomElement applications = doc.createElement("Applications");
        packageData.appendChild(applications);
        QDomNodeList application = doc.elementsByTagName("Application");
        summary = application.size();
        for (int i = application.size() - 1; i >= 0; i--) {
            applications.appendChild(application.at(i));
            emit(sendProgress(((summary - i) * 100) / summary));
        }

        // replace "IdentityDocumentQQ" with "IdentityDocument"
        emit sendLabel("Замена IdentityDocumentQQ на IdentityDocument");
        emit(sendProgress(0));
        QDomNodeList identityDocumentQQ = doc.elementsByTagName("IdentityDocumentQQ");
        summary = identityDocumentQQ.size();
        for (int i = 0; i < identityDocumentQQ.size(); i++) {
            identityDocumentQQ.at(i).toElement().setTagName("IdentityDocument");
            emit(sendProgress((i * 100) / summary));
        }

        // delete T00:00:00 from DocumentDate
        emit sendLabel("Удаление T00:00:00 из DocumentDate");
        emit(sendProgress(0));
        QString timeToDelete("T00:00:00");
        QDomNodeList timeFromDocumentDate = doc.elementsByTagName("DocumentDate");
        QString time;
        summary = timeFromDocumentDate.size();
        for (int i = 0; i < timeFromDocumentDate.size(); i++) {
            time = timeFromDocumentDate.at(i).firstChild().nodeValue();
            time.remove(QRegExp(timeToDelete));
            timeFromDocumentDate.at(i).firstChild().setNodeValue(time);
            emit(sendProgress((i * 100) / summary));
        }

        // delete T00:00:00 from BirthDate
        emit sendLabel("Удаление T00:00:00 из BirthDate");
        emit(sendProgress(0));
        QDomNodeList timeFromBirthDate = doc.elementsByTagName("BirthDate");
        summary = timeFromBirthDate.size();
        for (int i = 0; i < timeFromBirthDate.size(); i++) {
            time = timeFromBirthDate.at(i).firstChild().nodeValue();
            time.remove(QRegExp(timeToDelete));
            timeFromBirthDate.at(i).firstChild().setNodeValue(time);
            emit(sendProgress((i * 100) / summary));
        }

        // delete T00:00:00 from OriginalReceivedDate
        emit sendLabel("Удаление T00:00:00 из OriginalReceivedDate");
        emit(sendProgress(0));
        QDomNodeList timeFromOriginalReceivedDate = doc.elementsByTagName("OriginalReceivedDate");
        summary = timeFromOriginalReceivedDate.size();
        for (int i = 0; i < timeFromOriginalReceivedDate.size(); i++) {
            time = timeFromOriginalReceivedDate.at(i).firstChild().nodeValue();
            time.remove(QRegExp(timeToDelete));
            timeFromOriginalReceivedDate.at(i).firstChild().setNodeValue(time);
            emit(sendProgress((i * 100) / summary));
        }

        // save xml changes
        emit sendLabel("Сохранение изменений");
        emit(sendProgress(0));
        QFile newFile(originalFileInfo.path() + "/" + originalFileInfo.baseName() + "_new." + originalFileInfo.completeSuffix());
        if (!newFile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
            _errno = 0;
        }
        emit(sendProgress(30));
        QByteArray xml = doc.toByteArray();
        emit(sendProgress(50));
        if (newFile.write(xml) == -1) {
            _errno = 0;
        }
        emit(sendProgress(90));
        newFile.close();
        emit(sendProgress(100));

//        // exchange QQ and T00:00:00 symbols
//        QByteArray fileData;
//        if (newFile.open(QIODevice::ReadWrite)) {
//            fileData = newFile.readAll();

//            QString sFileData(fileData);
//            QFile testFile(originalFileInfo.path() + "/" + "test.xml");
//            testFile.open(QIODevice::Truncate | QIODevice::ReadWrite);
//            QTextStream dstream(&testFile);
//            for (int i = 0; i < sFileData.size(); i++) {
//                dstream << sFileData.at(i);
//            }
//            testFile.close();

//            newFile.close();
//        }
    }
    emit finished();
}

void Normalize::onFinished() {
    if (_errno) {
        qDebug() << "Файл обработан";
        emit sendMessage("Файл обработан");
    }
    else {
        qDebug() << "Не удалось обработать файл";
        emit sendMessage("Не удалось обработать файл");
    }
}
