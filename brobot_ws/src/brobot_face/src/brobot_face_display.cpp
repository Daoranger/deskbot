#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

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

class BrobotFaceDisplayNode : public rclcpp::Node
{

public:
    BrobotFaceDisplayNode(FaceWidget* face)
        : Node("brobot_face_display")
        , face_(face)
    {
        subscriber_ = this->create_subscription<std_msgs::msg::String>(
            "/brobot/expression",
            10,
            std::bind(&BrobotFaceDisplayNode::callbackBrobotFaceDisplay, this, std::placeholders::_1)
        );
    }

private:

    void callbackBrobotFaceDisplay(const std_msgs::msg::String::SharedPtr msg)
    {
        RCLCPP_INFO(this->get_logger(), "Received expression: %s", msg->data.c_str());

        if (msg->data == "neutral")
        {
            face_->setExpression(Expression::Neutral);
        }
        else if (msg->data == "happy")
        {
            face_->setExpression(Expression::Happy);
        }
        else if (msg->data == "sleepy")
        {
            face_->setExpression(Expression::Sleepy);
        }
        else
        {
            RCLCPP_WARN(this->get_logger(), "Unknown expression: %s", msg->data.c_str());
        }
    }
    
    FaceWidget* face_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscriber_;

};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    QApplication app(argc, argv);

    FaceWidget face;
    face.show();

    auto node = std::make_shared<BrobotFaceDisplayNode>(&face);

    QTimer ros_timer;
    QObject::connect(&ros_timer, &QTimer::timeout, [&]()
    {
        if (!rclcpp::ok())
        {
            app.quit();
            return;
        }

        rclcpp::spin_some(node);
    });

    ros_timer.start(10);

    int result = app.exec();
    rclcpp::shutdown();
    return result;

}
