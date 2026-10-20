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

6.4.5 更新雷达传感器写法，见：chapt6/chapt6_ws/src/bot_description/urdf/fishbot/plugins/gazebo_sensor_plugin.xacro。**注意：需要在 world 中声明 Sensors 插件，这个动作会导致 gazebo Harmonic 不加载其他默认插件，需要把所有插件都手动声明。**

6.4.6 更新惯性测量传感器写法，见：chapt6/chapt6_ws/src/bot_description/urdf/fishbot/plugins/gazebo_sensor_plugin.xacro。

6.4.5 更新深度相机传感器写法，见：chapt6/chapt6_ws/src/bot_description/urdf/fishbot/plugins/gazebo_sensor_plugin.xacro。除了 rviz2 和 rqt，gazebo 右上角可添加 Image Display，也能看图。
**注意：harmonic 深度相机 type 是 depth_camera，深度图像直接发布到了<topic>标签指定的名称中；harmonic 无需再创建 camera_optical_link 调整镜头，创建了反而有问题。**

6.5.2 主要是 plugin 标签中的 gazebo 改为 gz，整体改动不大。

6.5.3 之前 spawn robot 到 gazebo 用的是 LaunchDescription，注册事件监听器只支持 ExecuteProcess 或 Node，把 spawn 改成 ExecuteProcess。

6.5.4 抄书上的代码，无需改动。

6.5.5 yaml 中的 use_stamped_vel: false 配置无效，fishbot_diff_drive_controller 只有 cmd_vel 话题，没有 cmd_vel_unstamped 话题，映射配置修改后，用 ros2 run teleop_twist_keyboard teleop_twist_keyboard **--ros-args -p stamped:=true** 启动键盘模拟，否则发送的话题类型不匹配，带时间戳 geometry_msgs/msg/TwistStamped，不带时间戳 geometry_msgs/msg/Twist。

7.2.1 建图过程中发生漂移，是多种因素混合的结果，首先尝试降低车速（角速度、线速度都降），过快的车速更易引发偏移；其次提升激光雷达发布频率，提升相邻帧的重叠量，便于算法匹配修正；然后尝试在地图中空旷部分放置特征物体，有助于匹配修正。以上方案都无效的话，检查 tf 和 odom，复杂性更高，目前还没有遇到。

7.3.3 按书上配置，可正常显示全局和局部代价地图。

7.3.4 Nav2 Goal 指定目标后 bot 不移动，还是 6.5.5 中说的消息类型问题，use_stamped_vel: false 在 Jazzy 后已废弃，强制 geometry_msgs/msg/TwistStamped 消息，需要为 nav2 的配置添加 enable_stamped_cmd_vel: true。

7.3.6 ros2 jazzy 的 controller_server 的 FollowPath 默认类型为nav2_mppi_controller::MPPIController，最大速度和加速度配置变量与书中不同：最大角速度 wz_max，最大角加速度 az_max，最大线速度 vx_max，最大线加速度 ax_max。