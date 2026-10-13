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
        RCLCPP_INFO(this->get_logger(), "Battery node started.");
        timer_ = this->create_wall_timer(1s, std::bind(&BatteryNode::checkBattery, this));
    }

    void checkBattery()
    {
        seconds_++;

        if (battery_state_ == "full" && seconds_ >= 4)
        {
            battery_state_ = "empty";
            seconds_ = 0;
            RCLCPP_INFO(this->get_logger(), "Battery empty -> LED on");
            callSetLed(true);
        }
        else if (battery_state_ == "empty" && seconds_ >= 6)
        {
            battery_state_ = "full";
            seconds_ = 0;
            RCLCPP_INFO(this->get_logger(), "Battery full -> LED off");
            callSetLed(false);
        }
    }

    void callSetLed(bool status)
    {
        if (!client_->wait_for_service(1s))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for Led panel server.");
            return;
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
    rclcpp::TimerBase::SharedPtr timer_;
    std::string battery_state_ = "full";
    int seconds_ = 0;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<BatteryNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}