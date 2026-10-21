#include "motion_control_system/motion_control_interface.hpp"
#include <pluginlib/class_loader.hpp>

int main(int argc, char **argv)
{
    if (argc != 2)
        return -1;

    std::string controller_name = argv[1];
    // 通过功能包名称和基类名称创建控制器加载器
    pluginlib::ClassLoader<motion_control_system::MotionController> controller_loader("motion_control_system", "motion_control_system::MotionController");
    // 加载指定名称的插件
    auto controller{controller_loader.createSharedInstance(controller_name)};

    controller->start();
    controller->stop();
    return 0;
}