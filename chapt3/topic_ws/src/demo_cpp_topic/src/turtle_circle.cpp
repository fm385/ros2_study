#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <chrono>

using namespace std::chrono_literals;

class TurtleCircleNode : public rclcpp::Node
{
public:
    TurtleCircleNode(const std::string & node_name) : Node(node_name)
    {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);
        timer_ = this->create_wall_timer(1s, [this]() {
            geometry_msgs::msg::Twist msg;
            msg.linear.x = 1.0;  // Set linear velocity
            msg.angular.z = 0.5; // Set angular velocity
            publisher_->publish(msg);
        });
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node{ std::make_shared<TurtleCircleNode>("turtle_circle_node")};
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}