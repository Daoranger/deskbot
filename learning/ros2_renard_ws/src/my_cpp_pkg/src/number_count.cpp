#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/msg/int64.hpp"
#include "example_interfaces/srv/set_bool.hpp"

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
        
        server_ = this->create_service<example_interfaces::srv::SetBool>(
            "reset_counter",
            std::bind(&NumberCountNode::callbackResetCounter, this, std::placeholders::_1, std::placeholders::_2)
        );
    }

private:

    void callbackResetCounter(const example_interfaces::srv::SetBool::Request::SharedPtr request,
                              const example_interfaces::srv::SetBool::Response::SharedPtr response)
    {
        if (request->data)  // anything but 0
        {
            counter_ = 0;
            response->success = true;
            response->message = "Counter had been reset sucessfully";
        }
        else
        {
            response->success = false;
            response->message = "Counter had not been reset";
        }
    }

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
    rclcpp::Service<example_interfaces::srv::SetBool>::SharedPtr server_;
    
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<NumberCountNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}