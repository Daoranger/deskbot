#include "rclcpp/rclcpp.hpp"
#include "my_robot_interfaces/srv/set_led.hpp"
#include "my_robot_interfaces/msg/led_panel_state.hpp"

class LedPanelNode : public rclcpp::Node
{
public:
    LedPanelNode()
        : Node("led_panel")
    {
        server_ = this->create_service<my_robot_interfaces::srv::SetLed>(
            "set_led", 
            std::bind(&LedPanelNode::callbackSetLed, this, std::placeholders::_1, std::placeholders::_2)
        );

        publisher_ = this->create_publisher<my_robot_interfaces::msg::LedPanelState>("led_panel_state", 10);

        RCLCPP_INFO(this->get_logger(), "Set Led node started.");
    }

private:

    void publishStatus()
    {
        auto msg = my_robot_interfaces::msg::LedPanelState();
        msg.state_message = request->is_on ? "Status: on" : "Status: off";
        publisher_->publish(msg);
    }

    void callbackSetLed(const my_robot_interfaces::srv::SetLed::Request::SharedPtr request,
                            const my_robot_interfaces::srv::SetLed::Response::SharedPtr response)
    {
        RCLCPP_INFO(this->get_logger(), "LED %ld -> %s",
                request->led_number, request->is_on ? "on" : "off");

        response->success = true;
        //publishStatus();
    }

    rclcpp::Service<my_robot_interfaces::srv::SetLed>::SharedPtr server_;
    rclcpp::Publisher<my_robot_interfaces::msg::LedPanelState>::SharedPtr publisher_;
    
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<LedPanelNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}