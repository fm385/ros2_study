仓库说明
=========
本仓库用于记录《ROS2机器人开发：从入门到实践》的学习代码，配套b站视频地址：https://space.bilibili.com/1940177928/lists

开发环境
--------
操作系统：Ubuntu 24.04.5 LTS

编辑器：VsCode

Agent：VsCode Copilot，自定义 Agent： ROS2学习助手

ros2版本：jazzy

gazebo版本：Harmonic

项目结构
--------
与书上保持一致，按章节存放到对应目录

进度
--------
完成1-5章

完成6.1-6.3

6.4.1 新版 gazebo Harmonic 和书上的 gazebo Classic 不一致，由 Agent 编写了 chapt6/chapt6_ws/src/bot_description/world/room.sdf 用于测试

6.4.2 更新 launch 的写法，见：chapt6/chapt6_ws/src/bot_description/launch/display_gazebo.launch.py

6.4.3 更新 gazebo 标签写法，见：chapt6/chapt6_ws/src/bot_description/urdf/fishbot/actuator/wheel.urdf.xacro

6.4.4 更新插件写法，见：chapt6/chapt6_ws/src/bot_description/urdf/fishbot/plugins/gazebo_control_plugin.xacro。**注意：ros2 launch bot_description display_gazebo.launch.py 启动之后，要在 gazebo 中点左下角的 Play 按钮，再发布 cmd_vel 控制消息，否则 robot 不会响应移动。**

6.4.5 更新雷达传感器写法，见：chapt6/chapt6_ws/src/bot_description/urdf/fishbot/plugins/gazebo_sensor_plugin.xacro。**注意：需要在 world 中声明 Sensors 插件，这个动作会导致 gazebo Harmonic 不加载其他默认插件，需要把所有插件都手动声明**

6.4.6 更新惯性测量传感器写法，见：chapt6/chapt6_ws/src/bot_description/urdf/fishbot/plugins/gazebo_sensor_plugin.xacro。