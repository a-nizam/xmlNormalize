#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonOpenFile_clicked();
    void on_pushButtonNormilize_clicked();
    void setLabel(QString label);
    void setProgress(int progress);
    void showMessage(QString message);
    void onFinished();

private:
    Ui::MainWindow *ui;
    int isNotRunning;
};

#endif // MAINWINDOW_H
