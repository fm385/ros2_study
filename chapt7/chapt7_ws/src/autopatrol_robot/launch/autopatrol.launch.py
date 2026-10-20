import launch
import launch_ros
from ament_index_python import get_package_share_directory
import os
from launch.launch_description_sources import PythonLaunchDescriptionSource

def generate_launch_description():

    autopatrol_robot_dir = get_package_share_directory('autopatrol_robot')
    patrol_config_path = os.path.join(autopatrol_robot_dir, 'config', 'patrol_config.yaml')

    return launch.LaunchDescription([
        # 设置 PRIME Render Offload 环境变量
        launch.actions.SetEnvironmentVariable('__NV_PRIME_RENDER_OFFLOAD', '1'),
        launch.actions.SetEnvironmentVariable('__GLX_VENDOR_LIBRARY_NAME', 'nvidia'),
        launch_ros.actions.Node(
            package='autopatrol_robot',
            executable='patrol_node',
            parameters=[patrol_config_path],
        ),
        launch_ros.actions.Node(
            package='autopatrol_robot',
            executable='speaker',
        ),
    ])