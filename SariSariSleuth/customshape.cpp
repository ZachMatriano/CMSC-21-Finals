#include "customshape.h"
#include <QPainter>
#include <QPainterPath>

CustomShape::CustomShape(QWidget *parent): QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    shapeStyle = { ShapeType::Circle, QColor(255,0,255), 0 }; // default color
}

QSize CustomShape::sizeHint() const {
    return QSize(100, 100); // default size
}

void CustomShape::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    if (shapeStyle.type == ShapeType::Circle)
        path.addEllipse(rect());
    else
        path.addRoundedRect(rect(), shapeStyle.cornerRadius, shapeStyle.cornerRadius);

    painter.fillPath(path, shapeStyle.fillColor);
}

void CustomShape::setShapeStyle(const ShapeStyle &style) {
    shapeStyle = style;
    update(); // repaint with new style
}
