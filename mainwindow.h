#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>

#include <QFile>

#include<QJsonObject>
#include<QJsonParseError>
#include<QJsonArray>

#include <QPixmap>
#include <QImage>
#include <QPainter>

#include<QStandardItem>

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

    void on_splitter_splitterMoved(int pos, int index);

    void widget_update();

    void resizeEvent(QResizeEvent *event) override;

    void painterQuad();

private:
    Ui::MainWindow *ui;
    QJsonDocument jsonDoc;
    QJsonArray jsonArray;
    QPixmap pixmap;
    int itemIndex;
};
#endif // MAINWINDOW_H
