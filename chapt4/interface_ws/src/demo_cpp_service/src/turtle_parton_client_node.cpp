#include <rclcpp/rclcpp.hpp>
#include <human_recongnize_interface/srv/parton.hpp>
#include <chrono>
#include <random>
#include <rcl_interfaces/msg/parameter.hpp>
#include <rcl_interfaces/msg/parameter_value.hpp>
#include <rcl_interfaces/msg/parameter_type.hpp>
#include <rcl_interfaces/srv/set_parameters.hpp>

using namespace std::chrono_literals;

class TurtlePartonClientNode : public rclcpp::Node
{
public:
    TurtlePartonClientNode(const std::string &node_name) 
        : Node(node_name), gen_(std::random_device{}()), dis_(0, 15)
    {
        client_ = this->create_client<human_recongnize_interface::srv::Parton>("cpp/parton_service");
        timer_ = this->create_wall_timer(10s, 
            [this]() {
                if (!client_->wait_for_service(1s)) {
                    RCLCPP_WARN(this->get_logger(), "Service not available, waiting again...");
                    return;
                }

                if (!rclcpp::ok()) {
                    RCLCPP_ERROR(this->get_logger(), "ROS 2 is not ok, shutting down.");
                    return;
                }

                auto request = std::make_shared<human_recongnize_interface::srv::Parton::Request>();
                // random target
                int random_x = dis_(gen_);
                int random_y = dis_(gen_);
                request->target_x = random_x; // Random target x position
                request->target_y = random_y; // Random target y position

                auto future = client_->async_send_request(request,
                    [this, random_x, random_y](rclcpp::Client<human_recongnize_interface::srv::Parton>::SharedFuture response_future) {
                        auto response = response_future.get();
                        if (response->result == human_recongnize_interface::srv::Parton::Response::SUCCESS) {
                            RCLCPP_INFO(this->get_logger(), "Service call succeeded. x: %d, y: %d", random_x, random_y);
                        } else {
                            RCLCPP_ERROR(this->get_logger(), "Service call failed. x: %d, y: %d", random_x, random_y);
                        }
                    });
            });
    }

    std::shared_ptr<rcl_interfaces::srv::SetParameters::Response> call_set_parameters(std::vector<rcl_interfaces::msg::Parameter>& parameters)
    {
        auto client = this->create_client<rcl_interfaces::srv::SetParameters>("/turtle_control_node/set_parameters");
        while (!client->wait_for_service(1s)) {
            if (!rclcpp::ok()) {
                RCLCPP_ERROR(this->get_logger(), "Interrupted while waiting for the service. Exiting.");
                return nullptr;
            }
            RCLCPP_INFO(this->get_logger(), "Service not available, waiting again...");
        }

        auto request = std::make_shared<rcl_interfaces::srv::SetParameters::Request>();
        request->parameters = parameters;
        auto future = client->async_send_request(request);
        rclcpp::spin_until_future_complete(this->get_node_base_interface(), future);
        return future.get();
    }

    void update_k(double new_k)
    {
        rcl_interfaces::msg::Parameter param;
        param.name = "k";
        rcl_interfaces::msg::ParameterValue value;
        value.type = rcl_interfaces::msg::ParameterType::PARAMETER_DOUBLE;
        value.double_value = new_k;
        param.value = value;

        // test max_speed parameter
        auto max_speed_param = rclcpp::Parameter("max_speed", 100.0).to_parameter_msg();

        std::vector<rcl_interfaces::msg::Parameter> parameters;
        parameters.push_back(param);
        parameters.push_back(max_speed_param);

        auto response = call_set_parameters(parameters);
        if (response) {
            for (const auto & result : response->results) {
                if (result.successful) {
                    RCLCPP_INFO(this->get_logger(), "Parameter set successfully.");
                } else {
                    RCLCPP_ERROR(this->get_logger(), "Failed to set parameter: %s", result.reason.c_str());
                }
            }
        } else {
            RCLCPP_ERROR(this->get_logger(), "Failed to call set_parameters service.");
        }
    }

private:
    rclcpp::Client<human_recongnize_interface::srv::Parton>::SharedPtr client_;
    rclcpp::TimerBase::SharedPtr timer_;
    std::mt19937 gen_;
    std::uniform_int_distribution<> dis_; // Random distribution for target positions
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtlePartonClientNode>("turtle_parton_client_node");
    node->update_k(100.0); // Example of updating parameter k
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}