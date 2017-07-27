#include "normalize.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lCurrentProcess->hide();
    ui->pbNormalize->hide();
    isNotRunning = 1;
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
    if (isNotRunning) {
        isNotRunning = 0;

        ui->lCurrentProcess->show();
        ui->pbNormalize->show();

        QThread *thread = new QThread();
        Normalize *normalize = new Normalize(0);

        normalize->moveToThread(thread);

        normalize->fileName = ui->lineEditFileBrowse->text();

        connect(thread, SIGNAL(started()), normalize, SLOT(start()));
        connect(normalize, SIGNAL(finished()), thread, SLOT(quit()));
        connect(thread, SIGNAL(finished()), normalize, SLOT(onFinished()));
        connect(thread, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(normalize, SIGNAL(sendLabel(QString)), this, SLOT(setLabel(QString)));
        connect(normalize, SIGNAL(sendProgress(int)), this, SLOT(setProgress(int)));
        connect(normalize, SIGNAL(sendMessage(QString)), this, SLOT(showMessage(QString)));

        thread->start();
    }
    else {
        QMessageBox::critical(this, "", "Процесс уже запущен");
    }
}

void MainWindow::setLabel(QString label) {
    ui->lCurrentProcess->setText(label);
}

void MainWindow::setProgress(int progress) {
    ui->pbNormalize->setValue(progress);
}

void MainWindow::showMessage(QString message) {
    QMessageBox::information(this, "", message);
}

void MainWindow::onFinished() {
    isNotRunning = 1;
}


