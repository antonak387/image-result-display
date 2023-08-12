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

    ui->label_backgroundImage->clear();

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

    QImage image("C:/Users/tohag/Desktop/" + jsonObjectFull["image"].toString());

    if (image.isNull())
    {
        QMessageBox::information(this, "Ошибка", "Изображение не открывается", QMessageBox::Ok);
    }
    else
    {
        pixmap = QPixmap::fromImage(image);
        QPixmap tempPixmap =  pixmap.scaledToHeight(ui->scrollArea->height()-3);
        ui->label_backgroundImage->setPixmap(tempPixmap);

        MainWindow::painterQuad();

        QList<int> sizes;
        sizes << tempPixmap.width()+3 << ui->splitter->width() - tempPixmap.width()-3;
        ui->splitter->setSizes(sizes);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    if(!pixmap.isNull()&&ui->scrollArea->height()<pixmap.scaledToWidth(ui->scrollArea->width()-20).height())
    {
        ui->label_backgroundImage->setPixmap( pixmap.scaledToWidth(ui->scrollArea->width()-22));
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

void MainWindow::on_splitter_splitterMoved(int pos, int index)
{
    Q_UNUSED(pos);
    Q_UNUSED(index);
    if(!pixmap.isNull()&&ui->scrollArea->height()<pixmap.scaledToWidth(ui->scrollArea->width()-20).height())
    {
        ui->label_backgroundImage->setPixmap( pixmap.scaledToWidth(ui->scrollArea->width()-22));
    }
}

void MainWindow::painterQuad()
{

    //QJsonObject jsonObjectFull = jsonArray.at(itemIndex).toObject();
    //QJsonObject jsonObject = jsonObjectFull["objects"].toObject();
    QImage image;
    QPainter painter(&image);
    QJsonObject jsonObject = jsonArray.at(itemIndex).toObject()["objects"].toObject();

    for (const QString &objectName : jsonObject.keys())
    {
        QJsonArray quadArray = jsonObject[objectName].toObject().value("quad").toArray();
        QPoint arr[4];

        for (const QJsonValue &quadValue : quadArray)
        {
            QJsonArray pointArray = quadValue.toArray();
            if (pointArray.size() == 2)
            {
                for (int i = 0; i < 4; i++)
                {
                    arr[i].setX(pointArray.at(0).toDouble());
                    arr[i].setY(pointArray.at(1).toDouble());
                    }
            }

        }
        qDebug()<<arr[0];



        QPen pen;
        pen.setColor(Qt::white);
        pen.setWidth(10);//толщина
        painter.setPen(pen);



        painter.drawPolygon(arr, 4);
    }

    painter.drawImage(0,0,image);
    ui->label_backgroundImage->setPixmap(QPixmap::fromImage(image));

}

