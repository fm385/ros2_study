#ifndef CUSTOM_CONTROLLER_HPP
#define CUSTOM_CONTROLLER_HPP

#include <memory>
#include <string>
#include <vector>
#include <nav2_core/controller.hpp>
#include <rclcpp/rclcpp.hpp>
#include <nav2_util/robot_utils.hpp>

namespace nav2_custom_controller
{
    class CustomController : public nav2_core::Controller
    {
    protected:
        std::string plugin_name_;
        std::shared_ptr<tf2_ros::Buffer> tf_;
        std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros_;
        nav2_util::LifecycleNode::SharedPtr node_;
        nav2_costmap_2d::Costmap2D *costmap_;
        nav_msgs::msg::Path global_plan_;
        double max_angular_speed_;
        double max_linear_speed_;

        geometry_msgs::msg::PoseStamped getNearestTargetPose(const geometry_msgs::msg::PoseStamped &current_pose);
        double calculateAngleDifference(const geometry_msgs::msg::PoseStamped &current_pose, const geometry_msgs::msg::PoseStamped &target_pose);

    public:
        CustomController();
        ~CustomController();

        virtual void configure(
            const rclcpp_lifecycle::LifecycleNode::WeakPtr &,
            std::string name, std::shared_ptr<tf2_ros::Buffer>,
            std::shared_ptr<nav2_costmap_2d::Costmap2DROS>) override;
        virtual void cleanup() override;
        virtual void activate() override;
        virtual void deactivate() override;
        virtual void setPlan(const nav_msgs::msg::Path &path) override;
        virtual geometry_msgs::msg::TwistStamped computeVelocityCommands(
            const geometry_msgs::msg::PoseStamped &pose,
            const geometry_msgs::msg::Twist &velocity,
            nav2_core::GoalChecker *goal_checker) override;
        virtual void setSpeedLimit(const double &speed_limit, const bool &percentage) override;
    };
}

#endif // CUSTOM_CONTROLLER_HPP