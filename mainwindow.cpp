#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonOpenFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString(), QString(), tr("XML file (*.xml)"));
    ui->lineEditFileBrowse->setText(fileName);
    ui->lineEditFileBrowse->setCursorPosition(0);
}

void MainWindow::on_pushButtonNormilize_clicked() {
    int _errno = 1;
    if (ui->lineEditFileBrowse->text().length()) {
        QDomDocument doc("mydocument");
        QFile originalFile(ui->lineEditFileBrowse->text());
        QFileInfo originalFileInfo(ui->lineEditFileBrowse->text());
        if (!originalFile.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, "", "Не удалось открыть файл");
            return;
        }
        // Parse file
        if (!doc.setContent(&originalFile)) {
            QMessageBox::critical(this, "", "Не удалось распознать содержимое");
            originalFile.close();
            return;
        }
        originalFile.close();

        // rename "dataroot" ro "root"
        QDomNodeList roots = doc.elementsByTagName("dataroot");
        if (roots.size() > 0) {
            QDomElement root = roots.at(0).toElement();
            root.setTagName("root");
        }
        else {
            QMessageBox::critical(this, "", "Возможно, указанный файл уже обрабатывался");
            return;
        }

        // remove "ApplicationID" tags
        QDomNodeList applicationIDs = doc.elementsByTagName("ApplicationID");
        for (int i = applicationIDs.size() - 1; i >= 0; i--) {
            applicationIDs.at(i).toElement().parentNode().removeChild(applicationIDs.at(i));
        }

        // insert login pass info to "root"
        QDomElement root = doc.elementsByTagName("root").at(0).toElement();
        QDomElement authData = doc.createElement("AuthData");
        authData.setAttribute("xmlns:xs", "http://www.w3.org/2001/XMLSchema");
        root.appendChild(authData);
        QDomElement login = doc.createElement("Login");
        QDomText loginText = doc.createTextNode("dgma-pk@mail.ru");
        login.appendChild(loginText);
        QDomElement pass = doc.createElement("Pass");
        QDomText passText = doc.createTextNode("Fath1436");
        pass.appendChild(passText);
        authData.appendChild(login);
        authData.appendChild(pass);

        QDomElement packageData = doc.createElement("PackageData");
        packageData.setAttribute("xmlns:xs", "http://www.w3.org/2001/XMLSchema");
        root.appendChild(packageData);

        QDomElement applications = doc.createElement("Applications");
        packageData.appendChild(applications);
        QDomNodeList application = doc.elementsByTagName("Application");
        for (int i = application.size() - 1; i >= 0; i--) {
            applications.appendChild(application.at(i));
        }

        // replace "IdentityDocumentQQ" with "IdentityDocument"
        QDomNodeList identityDocumentQQ = doc.elementsByTagName("IdentityDocumentQQ");
        for (int i = 0; i < identityDocumentQQ.size(); i++) {
            identityDocumentQQ.at(i).toElement().setTagName("IdentityDocument");
        }

        // delete T00:00:00 from DocumentDate
        QString timeToDelete("T00:00:00");
        QDomNodeList timeFromDocumentDate = doc.elementsByTagName("DocumentDate");
        QString time;
        for (int i = 0; i < timeFromDocumentDate.size(); i++) {
            time = timeFromDocumentDate.at(i).firstChild().nodeValue();
            time.remove(QRegExp(timeToDelete));
            timeFromDocumentDate.at(i).firstChild().setNodeValue(time);
        }

        // delete T00:00:00 from BirthDate
        QDomNodeList timeFromBirthDate = doc.elementsByTagName("BirthDate");
        for (int i = 0; i < timeFromBirthDate.size(); i++) {
            time = timeFromBirthDate.at(i).firstChild().nodeValue();
            time.remove(QRegExp(timeToDelete));
            timeFromBirthDate.at(i).firstChild().setNodeValue(time);
        }

        // delete T00:00:00 from OriginalReceivedDate
        QDomNodeList timeFromOriginalReceivedDate = doc.elementsByTagName("OriginalReceivedDate");
        for (int i = 0; i < timeFromOriginalReceivedDate.size(); i++) {
            time = timeFromOriginalReceivedDate.at(i).firstChild().nodeValue();
            time.remove(QRegExp(timeToDelete));
            timeFromOriginalReceivedDate.at(i).firstChild().setNodeValue(time);
        }

        // save xml changes
        QFile newFile(originalFileInfo.path() + "/" + originalFileInfo.baseName() + "_new." + originalFileInfo.completeSuffix());
        if (!newFile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
            _errno = 0;
        }
        QByteArray xml = doc.toByteArray();
        if (newFile.write(xml) == -1) {
            _errno = 0;
        }
        newFile.close();

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

        if (_errno) {
            QMessageBox::information(this, "", "Файл обработан");
        }
        else {
            QMessageBox::critical(this, "", "Не удалось обработать файл");
        }
    }
}
