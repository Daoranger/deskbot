import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node


def generate_launch_description():
    gazebo = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('brobot_bringup'),
                'launch',
                'gazebo.launch.py'
            )
        )
    )

    face = Node(
        package='brobot_face',
        executable='brobot_face_display',
        output='screen'
    )

    behavior_tree = Node(
        package='brobot_behavior',
        executable='brobot_behavior_tree',
        output='screen'
    )

    delayed_behavior_tree = TimerAction(
        period=75.0,
        actions=[behavior_tree]
    )

    return LaunchDescription([
        gazebo,
        face,
        delayed_behavior_tree
    ])