#include "rclcpp/rclcpp.hpp"
#include "my_robot_interfaces/msg/turtle.hpp"
#include "my_robot_interfaces/msg/turtle_array.hpp"
#include "my_robot_interfaces/srv/catch_turtle.hpp"
#include <string>
#include <chrono>

using namespace std::chrono_literals;

class TurtleControllerNode : public rclcpp::Node
{
public:
    TurtleControllerNode()
        : Node("turtle_controller_node")
    {
        turtle_array_subscriber_ = this->create_subscription<my_robot_interfaces::msg::TurtleArray>(
            "alive_turtles",
            10,
            std::bind(&TurtleControllerNode::callbackTurtleArrayPublisher, this, std::placeholders::_1)
        );

        catch_client_ = this->create_client<my_robot_interfaces::srv::CatchTurtle>("catch_turtle");
    }

    void callCatchTurtle(std::string name)
    {
        while (!catch_client_->wait_for_service(1s))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for the spawner server");
        }

        auto request = std::make_shared<my_robot_interfaces::srv::CatchTurtle::Request>();
        request->catch_name = name;

        catch_client_->async_send_request(
            request, std::bind(&TurtleControllerNode::callbackCatchTurtle, this, std::placeholders::_1)
        );
    }

private:

    void callbackCatchTurtle(rclcpp::Client<my_robot_interfaces::srv::CatchTurtle>::SharedFuture future)
    {
        auto response = future.get();

        if (response->catch_success)
        {
            RCLCPP_INFO(this->get_logger(), "Turtle caught successfully");
        }
        else
        {
            RCLCPP_INFO(this->get_logger(), "Unsuccessfully catch");
        }
    }

    void callbackTurtleArrayPublisher(const my_robot_interfaces::msg::TurtleArray msg)
    {
        for (auto turtle : msg.turtles)
        {
            RCLCPP_INFO(this->get_logger(), "%s", turtle.name.c_str());
        }
    }

    rclcpp::Subscription<my_robot_interfaces::msg::TurtleArray>::SharedPtr turtle_array_subscriber_;
    rclcpp::Client<my_robot_interfaces::srv::CatchTurtle>::SharedPtr catch_client_;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleControllerNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}