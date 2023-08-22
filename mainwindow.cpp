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
    ui->pushButton_scale->hide();
    MainWindow::showMaximized();
    MainWindow::setWindowTitle("Приложение для отображения результатов распознавания");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_open_clicked()
{
    MainWindow::on_action_open_triggered();
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

    QImage image(pathToImages+ "/" + jsonObjectFull["image"].toString());

    image = image.scaled(image.width()*scale,image.height()*scale);

    bool imageOpen;

    int row = 0;

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

        QJsonObject object = jsonObjectFull["template"].toObject();

        QString type = object["type"].toString();
        double confidence = object["confidence"].toDouble();
        QString quad = object.contains("quad") ? object["quad"].toString() : "";

        if (!type.isEmpty() || confidence != 0.0 || !quad.isEmpty())
        {

            ui->tableWidget->insertRow(row);
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem("template"));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(type));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(confidence)));

            if(quad == "")
            {

                QPainter painter(&ui->widget_image->im);

                painter.scale(scale, scale);

                QPen pen;
                pen.setColor(Qt::red);
                pen.setWidth(2);
                painter.setPen(pen);

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
                        quad.push_back(QString::number(pointArray.at(0).toDouble())+", "+QString::number(pointArray.at(1).toDouble())+"; ");
                    }
                }
                painter.drawPolygon(polygon);
            }
            ui->tableWidget->setItem(row, 3, new QTableWidgetItem(quad));
            ++row;
            ui->widget_image->update();
        }


        QList<int> sizes;
        sizes << image.width() << ui->splitter->width() - image.width();
        ui->splitter->setSizes(sizes);
    }



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

        if(imageOpen && quad == "")
        {

            QPainter painter(&ui->widget_image->im);

            painter.scale(scale, scale);

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
                    quad.push_back(QString::number(pointArray.at(0).toDouble())+", "+QString::number(pointArray.at(1).toDouble())+"; ");

                }

            }
            painter.drawPolygon(polygon);
        }
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(quad));
        ++row;
        ui->widget_image->update();
    }

    //ui->widget_image->setFixedSize(ui->widget_image->size() * 0.5);

    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();

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



void MainWindow::on_action_open_triggered()
{
    itemIndex = 0;
    QString fileName;
    QFile file;
    QJsonParseError jsonDocError;

    fileName = QFileDialog::getOpenFileName(this, "Открыть файл JSON", "../", "JSON files (*.json)");
    QDir dir(fileName);

    if (fileName.isEmpty())
    {
        //qDebug()<<"Файл не выбран";
        QMessageBox::information(this, "Ошибка", "JSON файл не выбран",QMessageBox::Ok);
    }
    else
    {
        pathToImages = dir.filePath("..");

        if(pathToImages.isEmpty())
        {
            QMessageBox::information(this, "Ошибка", "Файл с изображениями не выбран",QMessageBox::Ok);
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
                        ui->pushButton_scale->show();
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
                QMessageBox::information(this, "Ошибка", "Не получилось открыть JSON файл",QMessageBox::Ok);
            }
            file.close();
        }
    }
}


void MainWindow::on_pushButton_scale_clicked()
{
    if(scaleMode == 1)
    {
        scale = 1;
        scaleMode = 2;
        ui->pushButton_scale->setText("Отдалить изображение");
    }
    else
    {
        scale = 0.4;
        scaleMode = 1;
        ui->pushButton_scale->setText("Приблизить изображение");
    }

    widget_update();

}

