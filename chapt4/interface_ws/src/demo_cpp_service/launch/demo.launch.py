import launch
import launch_ros
import launch.launch_description_sources
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    action_turtlesim_background_g = launch.actions.DeclareLaunchArgument(
        'b_g',
        default_value='150',
        description='Background green value of turtlesim'
    )

    action_max_speed = launch.actions.DeclareLaunchArgument(
        'max_speed',
        default_value='2.0',
        description='Max speed of turtle'
    )

    action_turtlesim_node = launch_ros.actions.Node(
        package='turtlesim',
        executable='turtlesim_node',
        parameters=[{'background_g': launch.substitutions.LaunchConfiguration('b_g', default='150')}],
        output='screen'
    )

    action_cpp_serice_node = launch_ros.actions.Node(
        package='demo_cpp_service',
        executable='turtle_control_node',
        parameters=[{'max_speed': launch.substitutions.LaunchConfiguration('max_speed', default='2.0')}],
        output='log'
    )

    action_cpp_client_node = launch_ros.actions.Node(
        package='demo_cpp_service',
        executable='turtle_parton_client_node',
        output='both'
    )

    return launch.LaunchDescription([
        action_max_speed,
        action_turtlesim_background_g,
        action_turtlesim_node, 
        action_cpp_serice_node, action_cpp_client_node,
        ])