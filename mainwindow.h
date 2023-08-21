#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>

#include <QFile>
#include <QDir>

#include<QJsonObject>
#include<QJsonParseError>
#include<QJsonArray>

#include <QImage>
#include <QPainter>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_open_clicked();

    void on_pushButton_back_clicked();

    void on_pushButton_next_clicked();

    void widget_update();

    void on_action_open_triggered();

private:
    Ui::MainWindow *ui;
    QJsonDocument jsonDoc;
    QJsonArray jsonArray;
    QPixmap pixmap;
    int itemIndex;
    QString pathToImages;
};
#endif // MAINWINDOW_H
