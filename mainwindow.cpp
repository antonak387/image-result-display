#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->splitter->hide();
    ui->pushButton_back->hide();
    ui->pushButton_next->hide();
    MainWindow::showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_open_clicked()
{
    itemIndex = 0;
    QString fileName;
    QFile file;
    QJsonParseError jsonDocError;

    fileName = QFileDialog::getOpenFileName(this, "Открыть файл", "C:\\Users\\tohag\\Desktop\\", "JSON files (*.json)");
    if (fileName.isEmpty())
    {
        //qDebug()<<"Файл не выбран";
        QMessageBox::information(this, "Ошибка", "Файл не выбран",QMessageBox::Ok);
    }
    else
    {
        file.setFileName(fileName);
        if(file.open(QIODevice::ReadOnly|QFile::Text))
        {
            jsonDoc = QJsonDocument::fromJson(QByteArray(file.readAll()),&jsonDocError);

            if(jsonDocError.error == 0)//QJsonParseError::NoError
            {
                if (jsonDoc.isArray())
                {
                    jsonArray = jsonDoc.array();
                    ui->pushButton_open->hide();
                    ui->splitter->show();
                    widget_update();
                }
                else
                {
                    QMessageBox::information(this, "Ошибка", "JSON Файл не является массивом, пожалуйста измените файл", QMessageBox::Ok);
                }
            }
            else
            {
                QMessageBox::information(this, "Ошибка", "JSON Файл содержит ошибку", QMessageBox::Ok);
            }
        }
        else
        {
            QMessageBox::information(this, "Ошибка", "Не получилось открыть файл",QMessageBox::Ok);
        }
        file.close();
    }
}

void MainWindow::widget_update()
{
    if(jsonArray.size()==itemIndex+1)
    {
        ui->pushButton_next->hide();
    }
    else
    {
        ui->pushButton_next->show();
    }

    if(itemIndex==0)
    {
        ui->pushButton_back->hide();
    }
    else
    {
        ui->pushButton_back->show();
    }

    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);

    QJsonObject jsonObjectFull = jsonArray.at(itemIndex).toObject();
    QJsonObject jsonObject = jsonObjectFull["objects"].toObject();

    //QString type = jsonObject["template"].toObject()["type"].toString();
    //QString authority = jsonObject["objects"].toObject()["authority"].toObject()["value"].toString();
    //QString image = jsonObject["image"].toString();
    //qDebug()<<type;
    //qDebug()<<image;

    ui->tableWidget->setColumnCount(4); // Настройте количество столбцов
    ui->tableWidget->setHorizontalHeaderLabels({"Object Name", "Value", "Confidence", "Quad"}); // Настройте заголовки столбцов

    QImage image("C:/Users/tohag/Desktop/" + jsonObjectFull["image"].toString());

    bool imageOpen;



    if (image.isNull())
    {
        QMessageBox::information(this, "Ошибка", "Изображение не открывается", QMessageBox::Ok);
        imageOpen = false;
    }
    else
    {
        imageOpen = true;

        ui->widget_image->setMinimumHeight(image.height());

        ui->widget_image->create_image(image);

        QList<int> sizes;
        sizes << image.width() << ui->splitter->width() - image.width();
        ui->splitter->setSizes(sizes);
    }

    int row = 0;
    for (const QString &objectName : jsonObject.keys())
    {
        QJsonObject object = jsonObject[objectName].toObject();

        QString value = object["value"].toString();
        double confidence = object["confidence"].toDouble();
        QString quad = object.contains("quad") ? object["quad"].toString() : "";

        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(objectName));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(value));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(confidence)));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(quad));

        if(imageOpen)
        {

            QPainter painter(&ui->widget_image->im);
            //painter.drawLine(0,200,200,200);
            QJsonArray quadArray = object.value("quad").toArray();
            QPolygon polygon;

            for (const QJsonValue &quadValue : quadArray)
            {
                QJsonArray pointArray = quadValue.toArray();
                if (pointArray.size() == 2)
                {
                    for (int i = 0; i < 4; i++)
                    {
                        QPoint point;
                        point.setX(pointArray.at(0).toDouble());
                        point.setY(pointArray.at(1).toDouble());
                        polygon<<point;
                    }
                }

            }
            painter.drawPolygon(polygon);
        }
        ++row;
        ui->widget_image->update();
    }
}

void MainWindow::on_pushButton_back_clicked()
{
    itemIndex--;
    widget_update();
}

void MainWindow::on_pushButton_next_clicked()
{
    itemIndex++;
    widget_update();
}


