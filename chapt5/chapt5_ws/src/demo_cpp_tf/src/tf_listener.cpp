#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2/LinearMath/Quaternion.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <chrono>
#include <tf2/utils.hpp>
#include <tf2_ros/buffer.hpp>
#include <tf2_ros/transform_listener.hpp>

using namespace std::chrono_literals;

class TFListener : public rclcpp::Node {
public:
    TFListener() : Node("tf_listener") {
        buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        listener_ = std::make_shared<tf2_ros::TransformListener>(*buffer_, this);
        timer_ = this->create_wall_timer(3s, [this]() {
            try {
                const auto result = buffer_->lookupTransform("base_link", "target_point", tf2::TimePointZero);
                const auto& translation = result.transform.translation;
                const auto& rotation = result.transform.rotation;

                double roll, pitch, yaw;
                tf2::getEulerYPR(rotation, yaw, pitch, roll);
                RCLCPP_INFO(this->get_logger(), "trans: %f, %f, %f", translation.x, translation.y, translation.z);
                RCLCPP_INFO(this->get_logger(), "quater: %f, %f, %f, %f", rotation.x, rotation.y, rotation.z, rotation.w);
                RCLCPP_INFO(this->get_logger(), "euler: %f, %f, %f", roll, pitch, yaw);
            } catch (tf2::TransformException& ex) {
                RCLCPP_INFO(this->get_logger(), "convert failed in listener: %s", ex.what());
            }
            
        });
    }
private:
    std::shared_ptr<tf2_ros::Buffer> buffer_;
    std::shared_ptr<tf2_ros::TransformListener> listener_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node{ std::make_shared<TFListener>() };
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}