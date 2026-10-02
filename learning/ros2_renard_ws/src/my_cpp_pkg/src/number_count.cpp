#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/msg/int64.hpp"

class NumberCountNode : public rclcpp::Node
{
public:
    NumberCountNode()
        : Node("number_count")
        , counter_(0)
    {
        subscriber_ = this->create_subscription<example_interfaces::msg::Int64>(
            "number",
            10,
            std::bind(&NumberCountNode::callbackNumberPublisher, this, std::placeholders::_1)
        );

        publisher_ = this->create_publisher<example_interfaces::msg::Int64>("number_count", 10);
    }

private:

    void callbackNumberPublisher(const example_interfaces::msg::Int64::SharedPtr msg)
    {
        RCLCPP_INFO(this->get_logger(), "%ld", msg->data);
        counter_ += msg->data;
        
        auto counter_msg = example_interfaces::msg::Int64();
        counter_msg.data = counter_;
        publisher_->publish(counter_msg);
    }


    int counter_;
    rclcpp::Subscription<example_interfaces::msg::Int64>::SharedPtr subscriber_;
    rclcpp::Publisher<example_interfaces::msg::Int64>::SharedPtr publisher_;
    
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<NumberCountNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}