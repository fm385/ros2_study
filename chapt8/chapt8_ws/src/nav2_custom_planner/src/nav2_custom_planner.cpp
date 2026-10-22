#include <nav2_util/node_utils.hpp>
#include <cmath>
#include <memory>
#include <string>
#include <nav2_core/route_exceptions.hpp>
#include "nav2_custom_planner/nav2_custom_planner.hpp"

namespace nav2_custom_planner
{
    CustomPlanner::CustomPlanner() : costmap_(nullptr), interpolation_resolution_(0.0)
    {
    }

    CustomPlanner::~CustomPlanner()
    {
    }

    void CustomPlanner::configure(
        const rclcpp_lifecycle::LifecycleNode::WeakPtr &parent,
        std::string name, std::shared_ptr<tf2_ros::Buffer> tf,
        std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros)
    {
        tf_ = tf;
        node_ = parent.lock();
        name_ = name;
        costmap_ = costmap_ros->getCostmap();
        global_frame_ = costmap_ros->getGlobalFrameID();
        
        nav2_util::declare_parameter_if_not_declared(node_, name_ + ".interpolation_resolution", rclcpp::ParameterValue(0.1));
        node_->get_parameter(name_ + ".interpolation_resolution", interpolation_resolution_);
    }

    void CustomPlanner::cleanup()
    {
        RCLCPP_INFO(node_->get_logger(), "cleanup plugin: %s", name_.c_str());
    }

    void CustomPlanner::activate()
    {
        RCLCPP_INFO(node_->get_logger(), "activate plugin: %s", name_.c_str());
    }

    void CustomPlanner::deactivate()
    {
        RCLCPP_INFO(node_->get_logger(), "deactivate plugin: %s", name_.c_str());
    }

    nav_msgs::msg::Path CustomPlanner::createPlan(
        const geometry_msgs::msg::PoseStamped &start,
        const geometry_msgs::msg::PoseStamped &goal,
        std::function<bool()> cancel_checker)
    {
        // no use cancel_checker, disable compile warning
        static_cast<void>(cancel_checker);

        RCLCPP_INFO(node_->get_logger(), "createPlan plugin: %s", name_.c_str());
        nav_msgs::msg::Path global_path;
        global_path.poses.clear();
        global_path.header.frame_id = global_frame_;
        global_path.header.stamp = node_->now();

        // check frame id
        if (start.header.frame_id != global_frame_)
        {
            RCLCPP_ERROR(node_->get_logger(), "frame id mismatch, start:%s, global:%s", start.header.frame_id.c_str(), global_frame_.c_str());
            return global_path;
        }
        if (goal.header.frame_id != global_frame_)
        {
            RCLCPP_ERROR(node_->get_logger(), "frame id mismatch, goal:%s, global:%s", goal.header.frame_id.c_str(), global_frame_.c_str());
            return global_path;
        }

        // compute step count and step value
        auto dx = goal.pose.position.x - start.pose.position.x;
        auto dy = goal.pose.position.y - start.pose.position.y;
        int total_number_of_loop = std::max(1.0, std::ceil(std::hypot(dx, dy) / interpolation_resolution_));
        double x_increment = dx / total_number_of_loop;
        double y_increment = dy / total_number_of_loop;

        // create path
        for (int i = 0; i < total_number_of_loop; i++)
        {
            double calc_pose_x = start.pose.position.x + x_increment * i;
            double calc_pose_y = start.pose.position.y + y_increment * i;
            // check cost
            unsigned int mx, my;
            if (costmap_->worldToMap(calc_pose_x, calc_pose_y, mx, my))
            {
                unsigned char cost = costmap_->getCost(mx, my);
                if (cost == nav2_costmap_2d::LETHAL_OBSTACLE)
                {
                    RCLCPP_WARN(node_->get_logger(), "can't reach (%f, %f)", calc_pose_x, calc_pose_y);
                    throw nav2_core::NoValidGraph("can't reach:" + std::to_string(calc_pose_x) + "," + std::to_string(calc_pose_y));
                }
            }

            // check success, add to path
            geometry_msgs::msg::PoseStamped p;
            p.pose.position.x = calc_pose_x;
            p.pose.position.y = calc_pose_y;
            p.pose.position.z = 0.0;
            p.header.stamp = node_->now();
            p.header.frame_id = global_frame_;
            global_path.poses.push_back(p);
        }

        // add last pose
        geometry_msgs::msg::PoseStamped g = goal;
        g.header.stamp = node_->now();
        g.header.frame_id = global_frame_;
        global_path.poses.push_back(g);

        return global_path;
    }
}

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(nav2_custom_planner::CustomPlanner, nav2_core::GlobalPlanner)