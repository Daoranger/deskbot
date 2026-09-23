#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

enum class Expression { Neutral, Happy, Sleepy };

class FaceWidget : public QWidget
{
public:
    FaceWidget()
    {
        resize(400, 300);
        setWindowTitle("Brobot");
    }

    void setExpression(Expression e)
    {
        expression_ = e;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.fillRect(rect(), Qt::black);

        painter.setBrush(Qt::white);
        painter.setPen(Qt::NoPen);

        const int eye_width = width() / 5;
        const int eye_height = height() * 2 / 5;
        const int eye_y = height() / 2 - eye_height / 2;
        const int left_x = width() / 3 - eye_width / 2;
        const int right_x = width() * 2 / 3 - eye_width / 2;

        switch (expression_)
        {
        case Expression::Neutral:
        {
            painter.drawEllipse(left_x, eye_y, eye_width, eye_height);
            painter.drawEllipse(right_x, eye_y, eye_width, eye_height);
            break;
        }
        case Expression::Happy:
        {
            painter.setPen(QPen(Qt::white, eye_width / 6, Qt::SolidLine, Qt::RoundCap));
            painter.setBrush(Qt::NoBrush);
            const int arc_y = eye_y + eye_height / 4;
            const int arc_height = eye_height / 2;
            painter.drawArc(left_x, arc_y, eye_width, arc_height, 0, 180 * 16);
            painter.drawArc(right_x, arc_y, eye_width, arc_height, 0, 180 * 16);
            break;
        }
        case Expression::Sleepy:
        {
            painter.drawEllipse(left_x, eye_y, eye_width, eye_height);
            painter.drawEllipse(right_x, eye_y, eye_width, eye_height);
            painter.fillRect(left_x, eye_y, eye_width, eye_height / 2, Qt::black);
            painter.fillRect(right_x, eye_y, eye_width, eye_height / 2, Qt::black);
            break;
        }
        }
    }

private:
    Expression expression_ = Expression::Neutral;

};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    FaceWidget face;
    face.show();

    // Temporary test: cycle expressions every 4 seconds
    QTimer cycle_timer;
    int index = 0;
    QObject::connect(&cycle_timer, &QTimer::timeout, [&]()
    {
        index = (index + 1) % 3;
        face.setExpression(static_cast<Expression>(index));
    });
    cycle_timer.start(4000);

    return app.exec();
}
