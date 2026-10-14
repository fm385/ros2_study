import launch
import launch_ros
from ament_index_python import get_package_share_directory
import os
from launch.launch_description_sources import PythonLaunchDescriptionSource

def generate_launch_description():

    urdf_tutorial_path = get_package_share_directory('bot_description')
    default_model_path = os.path.join(urdf_tutorial_path, 'urdf', 'fishbot', 'fishbot.urdf.xacro')
    default_world_path = os.path.join(urdf_tutorial_path, 'world', 'room.sdf')

    action_declare_arg_mode_path = launch.actions.DeclareLaunchArgument(
        name='model', default_value=str(default_model_path), description='URDF absolute path')
    robot_description = launch_ros.descriptions.ParameterValue(launch.substitutions.Command(
        ['xacro ', launch.substitutions.LaunchConfiguration('model')]), value_type=str)

    action_declare_arg_world_sdf_path = launch.actions.DeclareLaunchArgument(
        name='world', default_value=str(default_world_path))

    return launch.LaunchDescription([
        action_declare_arg_mode_path,
        action_declare_arg_world_sdf_path,
        launch_ros.actions.Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            parameters=[{'robot_description': robot_description},
                        {'use_sim_time': True},
                    ]
        ),
        launch_ros.actions.Node(
            package='joint_state_publisher',
            executable='joint_state_publisher',
            parameters=[{'use_sim_time': True}],
        ),
        launch.actions.IncludeLaunchDescription(
            PythonLaunchDescriptionSource(os.path.join(
                get_package_share_directory('ros_gz_sim'), 
                'launch',
                'gz_sim.launch.py'
            )),
            launch_arguments=[('gz_args', launch.substitutions.LaunchConfiguration('world'))]
        ),
        launch.actions.IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(
                get_package_share_directory('ros_gz_sim'), 
                'launch',
                'gz_spawn_model.launch.py'
            )),
            launch_arguments={
                'world': 'room',
                'topic': '/robot_description',
                'entity_name': 'fishbot',
            }.items()
        ),
        launch_ros.actions.Node(
            package='ros_gz_bridge',
            executable='parameter_bridge',
            arguments=['--ros-args', 
                       '-p', 
                       'config_file:=' + os.path.join(urdf_tutorial_path, 'urdf', 'fishbot', 'bridge_config.yaml'),
                       ],
            parameters=[{'use_sim_time': True}],
            output='screen'
        ),
    ])