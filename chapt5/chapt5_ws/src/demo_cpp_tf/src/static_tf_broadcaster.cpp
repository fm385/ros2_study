#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2/LinearMath/Quaternion.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tf2_ros/static_transform_broadcaster.hpp>

class StaticTFBroadcaster : public rclcpp::Node {
public:
    StaticTFBroadcaster() : Node("static_tf_broadcaster_node") {
        broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);

        geometry_msgs::msg::TransformStamped t;
        t.header.stamp = this->get_clock()->now();
        t.header.frame_id = "map";
        t.child_frame_id = "target_point";
        t.transform.translation.x = 5.0;
        t.transform.translation.y = 3.0;
        t.transform.translation.z = 0.0;
        tf2::Quaternion q;
        q.setRPY(0, 0, 60 * M_PI / 180);
        t.transform.rotation = tf2::toMsg(q);
        broadcaster_->sendTransform(t);
        RCLCPP_INFO(get_logger(), "static tf sended.");
    }
private:
    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> broadcaster_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node{ std::make_shared<StaticTFBroadcaster>() };
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}