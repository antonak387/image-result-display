#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->hide();
    ui->pushButton_back->hide();
    ui->pushButton_next->hide();
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
                //qDebug()<<"JSON Файл не содержит ошибок";
                if (jsonDoc.isArray())
                {
                    jsonArray = jsonDoc.array();
                    widget_update();



                    ui->pushButton_open->hide();
                    ui->tableWidget->show();




                    //qDebug()<<"все ок";
                    //здесь будет функция которая разбирается с массивом
                }
                else
                {
                    QMessageBox::information(this, "Ошибка", "JSON Файл не является массивом, пожалуйста измените файл", QMessageBox::Ok);
                    //qDebug()<<"JSON Файл не является массивом";
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

    ui->label->clear();

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

        ++row;
    }


    QImage image("C:/Users/tohag/Desktop/" + jsonObjectFull["image"].toString()); // Замените на путь к вашему изображению

    qDebug()<<jsonObjectFull["image"].toString();

    if (image.isNull())
    {
        QMessageBox::information(this, "Ошибка", "Изображение не открывается", QMessageBox::Ok);
    }
    else
    {
        QPixmap pixmap = QPixmap::fromImage(image.scaled(ui->label->size(), Qt::KeepAspectRatio));
        ui->label->setPixmap(pixmap);
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

