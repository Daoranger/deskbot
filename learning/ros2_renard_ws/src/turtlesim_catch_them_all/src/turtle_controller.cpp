#include "rclcpp/rclcpp.hpp"
#include "my_robot_interfaces/msg/turtle.hpp"
#include "my_robot_interfaces/msg/turtle_array.hpp"
#include "my_robot_interfaces/srv/catch_turtle.hpp"
#include <turtlesim/msg/pose.hpp>
#include "geometry_msgs/msg/twist.hpp"
#include <string>
#include <chrono>
#include <cmath>
#include <limits>
#include <algorithm>

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

        pose_subscriber_ = this->create_subscription<turtlesim::msg::Pose>(
                "/turtle1/pose", 10,
                std::bind(&TurtleControllerNode::callbackPose, this, std::placeholders::_1)
        );

        cmd_vel_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);

        control_timer_ = this->create_wall_timer(0.01s, std::bind(&TurtleControllerNode::control_loop, this));
    }

    void callCatchTurtle(std::string name)
    {
        if (!catch_client_->service_is_ready())
        {
            RCLCPP_WARN(get_logger(), "Catch service not ready");
            return;
        }

        auto request = std::make_shared<my_robot_interfaces::srv::CatchTurtle::Request>();
        request->catch_name = name;

        catch_client_->async_send_request(
            request, std::bind(&TurtleControllerNode::callbackCatchTurtle, this, std::placeholders::_1)
        );
    }

private:

    void control_loop()
    {
        if (!turtle1_pose_ || !alive_turtles_ || alive_turtles_->turtles.empty())
            return;

        float min_dist = std::numeric_limits<float>::max();
        my_robot_interfaces::msg::Turtle closestTurtle;

        for (auto turtle : alive_turtles_->turtles)
        {
            float dist = distance(turtle.x, turtle.y);
            if (min_dist > dist)
            {
                min_dist = dist;
                closestTurtle = turtle;
            }
        }


        if (min_dist > 0.1)
        {
            float linear = 2.5 * min_dist;
            float angular = 6.0 * angleDiff(closestTurtle.x, closestTurtle.y);
            publishCmdVel(angular, linear);
        }
        else
        {
            publishCmdVel(0.0, 0.0);
            if (!catching_)
            {
                catching_ = true;
                callCatchTurtle(closestTurtle.name);
            }
        }

    }

    float distance(float x_target, float y_target)
    {
        float dx = x_target - turtle1_pose_->x;
        float dy = y_target - turtle1_pose_->y;
        return std::sqrt(dx * dx + dy * dy);
    }

    float angleDiff(float x_target, float y_target)
    {
        float dx = x_target - turtle1_pose_->x;
        float dy = y_target - turtle1_pose_->y;

        float diff = std::atan2(dy, dx) - turtle1_pose_->theta;

        while (diff > M_PI)  diff -= 2 * M_PI;
        while (diff < -M_PI) diff += 2 * M_PI;

        return diff; 

    }

    void callbackCatchTurtle(rclcpp::Client<my_robot_interfaces::srv::CatchTurtle>::SharedFuture future)
    {
        catching_ = false;
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

    void callbackTurtleArrayPublisher(const my_robot_interfaces::msg::TurtleArray::SharedPtr msg)
    {
        alive_turtles_ = msg;
    }

    void callbackPose(const turtlesim::msg::Pose::SharedPtr msg)
    {
        turtle1_pose_ = msg;
    }

    void publishCmdVel(float angular, float linear)
    {
        auto msg = geometry_msgs::msg::Twist();
        msg.linear.x = linear;
        msg.angular.z = angular;
        cmd_vel_publisher_->publish(msg);
    }

    rclcpp::Subscription<my_robot_interfaces::msg::TurtleArray>::SharedPtr turtle_array_subscriber_;
    rclcpp::Client<my_robot_interfaces::srv::CatchTurtle>::SharedPtr catch_client_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_subscriber_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_publisher_;
    rclcpp::TimerBase::SharedPtr control_timer_;
    turtlesim::msg::Pose::SharedPtr turtle1_pose_;
    my_robot_interfaces::msg::TurtleArray::SharedPtr alive_turtles_;
    bool catching_ = false; 

};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleControllerNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}