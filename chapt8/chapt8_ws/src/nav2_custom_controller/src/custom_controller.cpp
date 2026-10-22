#include <algorithm>
#include <memory>
#include <string>
#include <chrono>
#include <memory>
#include <thread>
#include <nav2_core/route_exceptions.hpp>
#include <nav2_util/node_utils.hpp>
#include <nav2_util/geometry_utils.hpp>
#include "nav2_custom_controller/custom_controller.hpp"

namespace nav2_custom_controller
{
    CustomController::CustomController() : max_angular_speed_(0.0), max_linear_speed_(0.0)
    {
    }

    CustomController::~CustomController()
    {
    }

    void CustomController::configure(
        const rclcpp_lifecycle::LifecycleNode::WeakPtr &parent,
        std::string name, std::shared_ptr<tf2_ros::Buffer> tf,
        std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros)
    {
        node_ = parent.lock();
        costmap_ros_ = costmap_ros;
        tf_ = tf;
        plugin_name_ = name;

        nav2_util::declare_parameter_if_not_declared(node_, plugin_name_ + ".max_angular_speed", rclcpp::ParameterValue(1.0));
        node_->get_parameter(plugin_name_ + ".max_angular_speed", max_angular_speed_);
        nav2_util::declare_parameter_if_not_declared(node_, plugin_name_ + ".max_linear_speed", rclcpp::ParameterValue(0.1));
        node_->get_parameter(plugin_name_ + ".max_linear_speed", max_linear_speed_);
    }

    void CustomController::cleanup()
    {
        RCLCPP_INFO(node_->get_logger(), "cleanup plugin: %s", plugin_name_.c_str());
    }

    void CustomController::activate()
    {
        RCLCPP_INFO(node_->get_logger(), "activate plugin: %s", plugin_name_.c_str());
    }

    void CustomController::deactivate()
    {
        RCLCPP_INFO(node_->get_logger(), "deactivate plugin: %s", plugin_name_.c_str());
    }

    void CustomController::setPlan(const nav_msgs::msg::Path &path)
    {
        RCLCPP_INFO(node_->get_logger(), "setPlan plugin: %s", plugin_name_.c_str());
        global_plan_ = path;
    }

    geometry_msgs::msg::TwistStamped CustomController::computeVelocityCommands(
        const geometry_msgs::msg::PoseStamped &pose,
        const geometry_msgs::msg::Twist &velocity,
        nav2_core::GoalChecker *goal_checker)
    {
        static_cast<void>(velocity);
        static_cast<void>(goal_checker);

        if (global_plan_.poses.empty())
        {
            throw nav2_core::NoValidGraph("global plan is empty.");
        }

        geometry_msgs::msg::PoseStamped pose_in_globalframe;
        if (!nav2_util::transformPoseInTargetFrame(pose, pose_in_globalframe, *tf_, global_plan_.header.frame_id))
        {
            throw nav2_core::NoValidGraph("tf failed.");
        }

        auto target_pose = getNearestTargetPose(pose_in_globalframe);
        auto angle_diff = calculateAngleDifference(pose_in_globalframe, target_pose);

        geometry_msgs::msg::TwistStamped cmd_vel;
        cmd_vel.header.frame_id = pose_in_globalframe.header.frame_id;
        cmd_vel.header.stamp = node_->now();
        if (fabs(angle_diff) > M_PI / 10.0)
        {
            cmd_vel.twist.linear.x = 0.0;
            cmd_vel.twist.angular.z = fabs(angle_diff) / angle_diff * max_angular_speed_;
        }
        else
        {
            cmd_vel.twist.linear.x = max_linear_speed_;
            cmd_vel.twist.angular.z = 0.0;
        }
        RCLCPP_INFO(node_->get_logger(), "controller %s send speed (%f, %f)", plugin_name_.c_str(), cmd_vel.twist.linear.x, cmd_vel.twist.angular.z);
        return cmd_vel;
    }

    void CustomController::setSpeedLimit(const double &speed_limit, const bool &percentage)
    {
        static_cast<void>(speed_limit);
        static_cast<void>(percentage);
        
        RCLCPP_INFO(node_->get_logger(), "setSpeedLimit plugin: %s", plugin_name_.c_str());
    }

    geometry_msgs::msg::PoseStamped CustomController::getNearestTargetPose(const geometry_msgs::msg::PoseStamped &current_pose)
    {
        using nav2_util::geometry_utils::euclidean_distance;
        // get nearest iter directlly
        auto nearest_it = std::min_element(
            global_plan_.poses.begin(), global_plan_.poses.end(),
            [&current_pose](const auto &lhs, const auto &rhs)
            {
                return euclidean_distance(current_pose, lhs) <
                       euclidean_distance(current_pose, rhs);
            });

        global_plan_.poses.erase(global_plan_.poses.begin(), nearest_it);

        if (global_plan_.poses.size() == 1)
        {
            return global_plan_.poses.at(0);
        }
        return global_plan_.poses.at(1);
    }

    double CustomController::calculateAngleDifference(const geometry_msgs::msg::PoseStamped &current_pose, const geometry_msgs::msg::PoseStamped &target_pose)
    {
        float current_robot_yaw = tf2::getYaw(current_pose.pose.orientation);
        float target_angle = std::atan2(target_pose.pose.position.y - current_pose.pose.position.y,
                                        target_pose.pose.position.x - current_pose.pose.position.x);
        double angle_diff = target_angle - current_robot_yaw;
        if (angle_diff < -M_PI)
            angle_diff += 2.0 * M_PI;
        else if (angle_diff > M_PI)
            angle_diff -= 2.0 * M_PI;

        return angle_diff;
    }
}

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(nav2_custom_controller::CustomController, nav2_core::Controller)