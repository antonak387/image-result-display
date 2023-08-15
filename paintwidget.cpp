#include "paintwidget.h"

paintWidget::paintWidget(QWidget *parent)
    : QWidget{parent}
{
    imCreated = false;
}

void paintWidget::paintEvent(QPaintEvent *)
{
    if (imCreated)
    {
        QPainter painter (this);
        painter.drawImage(0,0,im);
    }
}

void paintWidget::create_image(QImage image)
{
    im = image;
    imCreated = true;
    repaint();
}
