import launch
import launch_ros
import launch.launch_description_sources
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

    action_run_topic_list = launch.actions.DeclareLaunchArgument(
        'run_topic_list',
        default_value='False'
    )
    run_topic_list = launch.substitutions.LaunchConfiguration('run_topic_list', default='False')

    multi_path = [get_package_share_directory('turtlesim'), '/launch/', 'multisim.launch.py']
    action_include_launch = launch.actions.IncludeLaunchDescription(
        launch.launch_description_sources.PythonLaunchDescriptionSource(
            multi_path
        )
    )

    action_log = launch.actions.LogInfo(msg=str(multi_path))

    action_topic_list = launch.actions.ExecuteProcess(
        condition=launch.conditions.IfCondition(run_topic_list),
        cmd=['ros2', 'topic', 'list'],
        output='screen'
    )

    action_group = launch.actions.GroupAction([
        launch.actions.TimerAction(
            period=2.0,
            actions=[action_include_launch]
        ),
        launch.actions.TimerAction(
            period=4.0,
            actions=[action_topic_list]
        )
    ])

    return launch.LaunchDescription([
        action_run_topic_list,
        action_log,
        action_group
    ])
