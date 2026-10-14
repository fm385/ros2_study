#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <turtlesim/msg/pose.hpp>

class TurtleControlNode : public rclcpp::Node
{
public:
    TurtleControlNode(const std::string &node_name, 
        double target_x, double target_y) : Node(node_name), target_x_(target_x), target_y_(target_y)
    {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);
        subscription_ = this->create_subscription<turtlesim::msg::Pose>("turtle1/pose", 10, 
            [this](turtlesim::msg::Pose::SharedPtr pose) {
                RCLCPP_INFO(this->get_logger(), "Turtle Position - x: %.2f, y: %.2f, theta: %.2f", 
                            pose->x, pose->y, pose->theta);
                double distance = std::sqrt(std::pow(target_x_ - pose->x, 2) + std::pow(target_y_ - pose->y, 2));
                if (distance > 0.1) {
                    geometry_msgs::msg::Twist msg;
                    double angle = std::atan2(target_y_ - pose->y, target_x_ - pose->x) - pose->theta;
                    angle = std::atan2(std::sin(angle), std::cos(angle)); // Normalize angle to [-pi, pi]
                    if (fabs(angle) > 0.2)
                    {
                        msg.angular.z = angle;
                    }
                    else
                    {
                        msg.linear.x = std::min(k_ * distance, max_speed_);
                    }
                    publisher_->publish(msg);
                }
            });
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscription_;
    double target_x_{1.0};
    double target_y_{1.0};
    double k_{1.0};
    double max_speed_{2.0};
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleControlNode>("turtle_control_node", 1.0, 1.0);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}