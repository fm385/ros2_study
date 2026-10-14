from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(
            package='demo_cpp_tf',
            executable='static_tf_broadcaster',
            output='screen',
        ),
        Node(
            package='demo_cpp_tf',
            executable='dynamic_tf_broadcaster',
            output='screen',
        ),
    ])