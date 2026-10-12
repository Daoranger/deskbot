#include "rclcpp/rclcpp.hpp"
#include "my_robot_interfaces/srv/set_led.hpp"

#include <chrono>

using namespace std::chrono_literals;

class BatteryNode : public rclcpp::Node
{
public:
    BatteryNode()
        : Node("battery_node") // battery node is a client
    {
        client_ = this->create_client<my_robot_interfaces::srv::SetLed>("set_led");
    }

    void callSetLed(bool status)
    {
        while (!client_->wait_for_service(1s))
        {
            if (!rclcpp::ok())
            {
                RCLCPP_ERROR(this->get_logger(), "Interrupted while waiting for the service.");
                return;
            }
            
            RCLCPP_WARN(this->get_logger(), "Waiting for Led panel server.");
        }

        auto request = std::make_shared<my_robot_interfaces::srv::SetLed::Request>();
        request->led_number = 3;
        request->is_on = status;

        client_->async_send_request(
            request, std::bind(&BatteryNode::callbackCallSetLed, this, std::placeholders::_1)
        );
    }

private:
    void callbackCallSetLed(rclcpp::Client<my_robot_interfaces::srv::SetLed>::SharedFuture future)
    {
        auto response = future.get();
        RCLCPP_INFO(this->get_logger(), "Set Led Success: %s", response->success ? "true" : "false");
    }

    rclcpp::Client<my_robot_interfaces::srv::SetLed>::SharedPtr client_;
    
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<BatteryNode>();
    node->callSetLed(true);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}