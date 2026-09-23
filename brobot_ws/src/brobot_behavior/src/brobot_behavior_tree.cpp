#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "behaviortree_cpp/bt_factory.h"

#include "ament_index_cpp/get_package_share_directory.hpp"
#include <thread>
#include <chrono>

class MoveFoward : public BT::SyncActionNode
{
public:
    MoveFoward(
        const std::string& name,
        const BT::NodeConfig& config,
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher)
        : BT::SyncActionNode(name, config)
        , publisher_(publisher)
    {
    }

    static BT::PortsList providedPorts()
    {
        return {};
    }

    BT::NodeStatus tick() override
    {
        auto msg = geometry_msgs::msg::Twist();
        msg.linear.x = 0.1;
        publisher_->publish(msg);
        return BT::NodeStatus::SUCCESS;
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
};

class Stop : public BT::SyncActionNode
{
public:
    Stop(
        const std::string& name,
        const BT::NodeConfig& config,
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher)
        : BT::SyncActionNode(name, config)
        , publisher_(publisher)
    {
    }

    static BT::PortsList providedPorts()
    {
        return {};
    }

    BT::NodeStatus tick() override
    {
        auto msg = geometry_msgs::msg::Twist();
        msg.linear.x = 0.0;
        publisher_->publish(msg);
        return BT::NodeStatus::SUCCESS;
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
};

class Wait : public BT::StatefulActionNode
{
public:
    Wait(const std::string& name, const BT::NodeConfig& config, std::chrono::milliseconds duration)
        : BT::StatefulActionNode(name, config)
        , duration_(duration)
    {
    }

    static BT::PortsList providedPorts()
    {
        return {};
    }

    BT::NodeStatus onStart() override
    {
        start_time_ = std::chrono::steady_clock::now();
        return BT::NodeStatus::RUNNING;
    }

    BT::NodeStatus onRunning() override
    {
        auto elapsed = std::chrono::steady_clock::now() - start_time_;
        if (elapsed >= duration_)
        {
            return BT::NodeStatus::SUCCESS;
        }
        return BT::NodeStatus::RUNNING;
    }

    void onHalted() override
    {
    }

private:
    std::chrono::milliseconds duration_;
    std::chrono::steady_clock::time_point start_time_;
};

class BrobotBehaviorTreeNode : public rclcpp::Node
{
public:
    BrobotBehaviorTreeNode()
        : Node("brobot_behavior_tree")
    {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr getPublisher()
    {
        return publisher_;
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<BrobotBehaviorTreeNode>();

    BT::BehaviorTreeFactory factory;

    BT::NodeBuilder move_forward_builder = [&](const std::string& name, const BT::NodeConfig& config)
    {
        return std::make_unique<MoveFoward>(name, config, node->getPublisher());
    };

    BT::NodeBuilder stop_builder = [&](const std::string& name, const BT::NodeConfig& config)
    {
        return std::make_unique<Stop>(name, config, node->getPublisher());
    };

    BT::NodeBuilder wait_builder = [&](const std::string& name, const BT::NodeConfig& config)
    {
        return std::make_unique<Wait>(name, config, std::chrono::milliseconds(10000));
    };




    factory.registerBuilder<MoveFoward>("MoveFoward", move_forward_builder);
    factory.registerBuilder<Stop>("Stop", stop_builder);
    factory.registerBuilder<Wait>("Wait", wait_builder);

    std::string tree_path = ament_index_cpp::get_package_share_directory("brobot_behavior")
        + "/behavior_trees/brobot_tree.xml";
    auto tree = factory.createTreeFromFile(tree_path);


    while (rclcpp::ok())
    {
        tree.tickOnce();
        rclcpp::spin_some(node);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    rclcpp::shutdown();
    return 0;
}
