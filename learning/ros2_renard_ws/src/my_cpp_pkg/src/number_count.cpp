#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/msg/int64.hpp"

class NumberCountNode : public rclcpp::Node
{
public:
    NumberCountNode()
        : Node("number_count")
    {
        subscriber_ = this->create_subscription<example_interfaces::msg::Int64>(
            "number",
            10,
            std::bind(&NumberCountNode::callbackNumberPublisher, this, std::placeholders::_1)
        );
    }

private:

    void callbackNumberPublisher(const example_interfaces::msg::Int64::SharedPtr msg)
    {
        RCLCPP_INFO(this->get_logger(), "%ld", msg->data);
    }

    rclcpp::Subscription<example_interfaces::msg::Int64>::SharedPtr subscriber_;
    
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<NumberCountNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}