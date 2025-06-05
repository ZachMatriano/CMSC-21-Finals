#ifndef CUSTOMSHAPE_H
#define CUSTOMSHAPE_H

#include <QWidget>
#include <QColor>

enum class ShapeType { Circle, RoundedRect };

struct ShapeStyle {
    ShapeType type;
    QColor fillColor;
    int cornerRadius; // used only for rounded rectangles
};

class CustomShape : public QWidget {
    Q_OBJECT

public:
    explicit CustomShape(QWidget *parent = nullptr);

    QSize sizeHint() const override;
    void setShapeStyle(const ShapeStyle& style);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    ShapeStyle shapeStyle;
};

#endif // CUSTOMSHAPE_H

