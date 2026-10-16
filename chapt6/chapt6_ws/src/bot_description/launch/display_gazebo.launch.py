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

    spawn_entity_node = launch.actions.ExecuteProcess(
        cmd=[
            'ros2', 'run', 'ros_gz_sim', 'create',
            '-world', 'room',
            '-topic', '/robot_description',
            '-name', 'fishbot',
        ],
        output='screen',
    )
    load_joint_state_controller = launch.actions.ExecuteProcess(
        cmd=['ros2', 'control', 'load_controller', '--set-state',
             'active', 'fishbot_joint_state_broadcaster'],
        output='screen'
    )
    # 这个和 diff_drive_controller 互斥，留一个即可
    # 如果开启，下面 RegisterEventHandler 同步修改
    # load_fishbot_effort_controller = launch.actions.ExecuteProcess(
    #     cmd=['ros2', 'control', 'load_controller', '--set-state',
    #          'active', 'fishbot_effort_controller'],
    #     output='screen'
    # )
    load_fishbot_diff_drive_controller = launch.actions.ExecuteProcess(
            cmd=['ros2', 'control', 'load_controller', '--set-state',
                 'active', 'fishbot_diff_drive_controller'],
            output='screen'
        )

    return launch.LaunchDescription([
        # 设置 PRIME Render Offload 环境变量
        launch.actions.SetEnvironmentVariable('__NV_PRIME_RENDER_OFFLOAD', '1'),
        launch.actions.SetEnvironmentVariable('__GLX_VENDOR_LIBRARY_NAME', 'nvidia'),
        action_declare_arg_mode_path,
        action_declare_arg_world_sdf_path,
        launch.actions.RegisterEventHandler(
            event_handler=launch.event_handlers.OnProcessExit(
                target_action=spawn_entity_node,
                on_exit=[load_joint_state_controller]
            )
        ),
        launch.actions.RegisterEventHandler(
            event_handler=launch.event_handlers.OnProcessExit(
                target_action=load_joint_state_controller,
                on_exit=[load_fishbot_diff_drive_controller]
            )
        ),
        launch_ros.actions.Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            parameters=[{'robot_description': robot_description},
                        {'use_sim_time': True},
                    ]
        ),
        launch.actions.IncludeLaunchDescription(
            PythonLaunchDescriptionSource(os.path.join(
                get_package_share_directory('ros_gz_sim'), 
                'launch',
                'gz_sim.launch.py'
            )),
            launch_arguments=[('gz_args', launch.substitutions.LaunchConfiguration('world'))]
        ),
        spawn_entity_node,
        launch_ros.actions.Node(
            package='ros_gz_bridge',
            executable='parameter_bridge',
            arguments=['--ros-args', 
                       '-p', 
                       'config_file:=' + os.path.join(urdf_tutorial_path, 'config', 'bridge_config.yaml'),
                       ],
            parameters=[{'use_sim_time': True}],
            output='screen'
        ),
    ])