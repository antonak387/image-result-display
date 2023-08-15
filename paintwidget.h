#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

class paintWidget : public QWidget
{
    Q_OBJECT
public:
    explicit paintWidget(QWidget *parent = nullptr);
    QImage im;

public slots:
    void create_image(QImage image);

private:
    void paintEvent(QPaintEvent *event);
    bool imCreated;

};

#endif // PAINTWIDGET_H
