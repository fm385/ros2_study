#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <turtlesim/msg/pose.hpp>
#include <human_recongnize_interface/srv/parton.hpp>
#include <rcl_interfaces/msg/set_parameters_result.hpp>

class TurtleControlNode : public rclcpp::Node
{
public:
    TurtleControlNode(const std::string &node_name, 
        double target_x, double target_y) : Node(node_name), target_x_(target_x), target_y_(target_y)
    {
        this->declare_parameter("k", 1.0);
        this->declare_parameter("max_speed", 2.0);
        this->get_parameter("k", k_);
        this->get_parameter("max_speed", max_speed_);

        on_set_param_callback_handle_ = this->add_on_set_parameters_callback([this](const std::vector<rclcpp::Parameter>& params) {
            rcl_interfaces::msg::SetParametersResult result;
            result.successful = true;
            for (const auto &param : params) {
                if (param.get_name() == "k") {
                    auto k = param.get_value<double>();
                    RCLCPP_INFO(this->get_logger(), "About to update k to: %.2f", k);
                    if (k <= 0 || k >= 5) {
                        RCLCPP_WARN(this->get_logger(), "Invalid value for k: %.2f. It must be positive.", k);
                        result.successful = false;
                        result.reason = "Invalid value for k. It must be positive.";
                        break;
                    }
                } else if (param.get_name() == "max_speed") {
                    auto max_speed = param.get_value<double>();
                    RCLCPP_INFO(this->get_logger(), "About to update max_speed to: %.2f", max_speed);
                    if (max_speed <=0 || max_speed >= 10) {
                        RCLCPP_WARN(this->get_logger(), "Invalid value for max_speed: %.2f. It must be between 0 and 10.", max_speed);
                        result.successful = false;
                        result.reason = "Invalid value for max_speed. It must be between 0 and 10.";
                        break;
                    }
                }
            }
            return result;
        });

        post_set_param_callback_handle_ = this->add_post_set_parameters_callback([this](const std::vector<rclcpp::Parameter> &params) {
            for (const auto &param : params) {
                if (param.get_name() == "k") {
                    auto k = param.get_value<double>();
                    RCLCPP_INFO(this->get_logger(), "Updated k to: %.2f", k);
                    k_ = k;
                } else if (param.get_name() == "max_speed") {
                    auto max_speed = param.get_value<double>();
                    RCLCPP_INFO(this->get_logger(), "Updated max_speed to: %.2f", max_speed);
                    max_speed_ = max_speed;
                }
            }
        });

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
        service_ = this->create_service<human_recongnize_interface::srv::Parton>(
            "cpp/parton_service",
            [this](const human_recongnize_interface::srv::Parton::Request::SharedPtr request,
                   human_recongnize_interface::srv::Parton::Response::SharedPtr response) {
                RCLCPP_INFO(this->get_logger(), "Received service request: x=%.2f, y=%.2f", 
                request->target_x, request->target_y);

                if (request->target_x > 0 && request->target_x < 12.0 && request->target_y > 0 && request->target_y < 12.0)
                {
                    target_x_ = request->target_x;
                    target_y_ = request->target_y;
                    response->result = human_recongnize_interface::srv::Parton::Response::SUCCESS;
                }
                else
                {
                    response->result = human_recongnize_interface::srv::Parton::Response::FAIL;
                    RCLCPP_INFO(this->get_logger(), "Invalid target position received.");
                }
            });
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscription_;
    rclcpp::Service<human_recongnize_interface::srv::Parton>::SharedPtr service_;
    rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr on_set_param_callback_handle_;
    rclcpp::node_interfaces::PostSetParametersCallbackHandle::SharedPtr post_set_param_callback_handle_;
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