import launch
import launch_ros
from ament_index_python import get_package_share_directory
import os
from launch.launch_description_sources import PythonLaunchDescriptionSource

def generate_launch_description():

    fishbot_navigation2_dir = get_package_share_directory('fishbot_navigation2')
    nav2_bringup_dir = get_package_share_directory('nav2_bringup')
    rviz_config_dir = os.path.join(nav2_bringup_dir, 'rviz', 'nav2_default_view.rviz')

    use_sim_time = launch.actions.DeclareLaunchArgument(
        name='use_sim_time', default_value='true')
    map_yaml_path = launch.actions.DeclareLaunchArgument(
        name='map', default_value=os.path.join(fishbot_navigation2_dir, 'maps', 'room.yaml'))
    nav2_param_path = launch.actions.DeclareLaunchArgument(
        name='params_file', default_value=os.path.join(fishbot_navigation2_dir, 'config', 'nav2_params.yaml'))

    return launch.LaunchDescription([
        # 设置 PRIME Render Offload 环境变量
        launch.actions.SetEnvironmentVariable('__NV_PRIME_RENDER_OFFLOAD', '1'),
        launch.actions.SetEnvironmentVariable('__GLX_VENDOR_LIBRARY_NAME', 'nvidia'),
        use_sim_time,
        map_yaml_path,
        nav2_param_path,
        launch.actions.IncludeLaunchDescription(
            PythonLaunchDescriptionSource(os.path.join(
                nav2_bringup_dir, 
                'launch',
                'bringup_launch.py'
            )),
            launch_arguments={
                'map': launch.substitutions.LaunchConfiguration('map'),
                'use_sim_time': launch.substitutions.LaunchConfiguration('use_sim_time'),
                'params_file': launch.substitutions.LaunchConfiguration('params_file'),
            }.items()
        ),
        launch_ros.actions.Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', rviz_config_dir],
            parameters=[{'use_sim_time': launch.substitutions.LaunchConfiguration('use_sim_time')}],
            output='screen'
        ),
    ])