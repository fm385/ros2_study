import launch
import launch_ros
from ament_index_python import get_package_share_directory
import os

def generate_launch_description():

    urdf_tutorial_path = get_package_share_directory('fishbot_description')
    default_model_path = os.path.join(urdf_tutorial_path, 'urdf', 'first_robot.urdf')
    default_rviz_config_path = os.path.join(urdf_tutorial_path, 'config', 'rviz', 'display_model.rviz')

    action_declare_arg_mode_path = launch.actions.DeclareLaunchArgument(
        name='model', default_value=str(default_model_path), description='URDF absolute path')
    robot_description = launch_ros.descriptions.ParameterValue(launch.substitutions.Command(
        ['xacro ', launch.substitutions.LaunchConfiguration('model')]), value_type=str)

    return launch.LaunchDescription([
        action_declare_arg_mode_path,
        launch_ros.actions.Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            parameters=[{'robot_description': robot_description}]
        ),
        launch_ros.actions.Node(
            package='joint_state_publisher',
            executable='joint_state_publisher',
        ),
        launch_ros.actions.Node(
            package='rviz2',
            executable='rviz2',
            arguments=['-d', default_rviz_config_path]
        ),
    ])