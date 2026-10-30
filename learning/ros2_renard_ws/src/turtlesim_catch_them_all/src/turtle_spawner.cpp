#include "rclcpp/rclcpp.hpp"
#include <turtlesim/srv/spawn.hpp>
#include <turtlesim/srv/kill.hpp>
#include "my_robot_interfaces/msg/turtle.hpp"
#include "my_robot_interfaces/msg/turtle_array.hpp"
#include "my_robot_interfaces/srv/catch_turtle.hpp"
#include <chrono>
#include <random>

using namespace std::chrono_literals;

class TurtleSpawnerNode : public rclcpp::Node
{
public:
    TurtleSpawnerNode()
        : Node("turtle_spawner_node")
    {
        spawn_client_ = this->create_client<turtlesim::srv::Spawn>("spawn");
        kill_client_ = this->create_client<turtlesim::srv::Kill>("kill");
        timer_ = this->create_wall_timer(std::chrono::duration<double>(2s), 
                                         std::bind(&TurtleSpawnerNode::spawnTurtle, this));
        turtle_array_publisher_ = this->create_publisher<my_robot_interfaces::msg::TurtleArray>("alive_turtles", 10);

        catch_server_ = this->create_service<my_robot_interfaces::srv::CatchTurtle>(
            "catch_turtle", 
            std::bind(&TurtleSpawnerNode::callbackCatchTurtle, this, std::placeholders::_1, std::placeholders::_2)
        );
    
    }

    void callSpawn(float x, float y)
    {
        while (!spawn_client_->wait_for_service(1s))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for the spawn server");
        }

        auto request = std::make_shared<turtlesim::srv::Spawn::Request>();
        request->x = x;
        request->y = y;

        spawn_client_->async_send_request(
            request,
            [this](rclcpp::Client<turtlesim::srv::Spawn>::SharedFutureWithRequest future)
            {
                callbackSpawn(future);
            });
    }

    void callKill(const std::string name)
    {
        while (!kill_client_->wait_for_service(1s))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for the kill server");
        }

        auto request = std::make_shared<turtlesim::srv::Kill::Request>();
        request->name = name;

        kill_client_->async_send_request(
            request,
            [this](rclcpp::Client<turtlesim::srv::Kill>::SharedFutureWithRequest future)
            {
                callbackKill(future);
            });
    }

private:

    void callbackSpawn(rclcpp::Client<turtlesim::srv::Spawn>::SharedFutureWithRequest future)
    {
        auto pair = future.get();
        auto request = pair.first;
        auto response = pair.second;

        RCLCPP_INFO(this->get_logger(), "Turtle named \"%s\" is spawned", response->name.c_str());
        addTurtle(request->x, request->y, response->name);
    }

    void callbackKill(rclcpp::Client<turtlesim::srv::Kill>::SharedFutureWithRequest future)
    {
        auto request = future.get().first;
        RCLCPP_INFO(this->get_logger(), "Turtle named \"%s\" is killed", request->name.c_str());
    }

    void spawnTurtle()
    {
        // if (turtleCounter >= 3)
        // {
        //     RCLCPP_INFO_ONCE(this->get_logger(), "Reached maximum limit of 3 turtles. Stopping spawn.");
        //     return; 
        // }

        std::random_device rd; 
        std::mt19937 gen(rd()); 

        std::uniform_real_distribution<> float_dist(0.0, 11.0);

        double random_x = float_dist(gen);
        double random_y = float_dist(gen);

        callSpawn(random_x, random_y);
        turtleCounter++;
    }

    void addTurtle(float x, float y, const std::string& name)
    {
        auto turtle_msg = my_robot_interfaces::msg::Turtle();
        turtle_msg.x = x;
        turtle_msg.y = y;
        turtle_msg.name = name;
        alive_turtles_.push_back(turtle_msg); 

        publishAliveTurtles();
    }

    void publishAliveTurtles()
    {
        auto turtle_array_msg = my_robot_interfaces::msg::TurtleArray();
        turtle_array_msg.turtles = alive_turtles_; 
        turtle_array_publisher_->publish(turtle_array_msg);
    }

    void callbackCatchTurtle(const my_robot_interfaces::srv::CatchTurtle::Request::SharedPtr request,
                            const my_robot_interfaces::srv::CatchTurtle::Response::SharedPtr response)
    {
        for (auto it = alive_turtles_.begin(); it != alive_turtles_.end(); ++it)
        {
            if (it->name == request->catch_name)
            {
                callKill(it->name);
                alive_turtles_.erase(it);
                publishAliveTurtles();
                response->catch_success = true; 
                return; // return early found
            }
        }
        response->catch_success = false; // not found, return false
    }   


    rclcpp::Client<turtlesim::srv::Spawn>::SharedPtr spawn_client_;
    rclcpp::Client<turtlesim::srv::Kill>::SharedPtr kill_client_;
    
    rclcpp::Publisher<my_robot_interfaces::msg::TurtleArray>::SharedPtr turtle_array_publisher_;

    rclcpp::TimerBase::SharedPtr timer_;

    int turtleCounter = 0;
    std::vector<my_robot_interfaces::msg::Turtle> alive_turtles_; 

    rclcpp::Service<my_robot_interfaces::srv::CatchTurtle>::SharedPtr catch_server_;

    

};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleSpawnerNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}