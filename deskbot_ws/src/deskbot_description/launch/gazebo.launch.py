import os

from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node


def generate_launch_description():

    pkg_path = get_package_share_directory('deskbot_description')
    urdf_path = os.path.join(pkg_path, 'urdf', 'deskbot.urdf')
    gz_sim_path = os.path.join(get_package_share_directory('ros_gz_sim'), 'launch', 'gz_sim.launch.py')

    with open(urdf_path, 'r') as f:
        robot_description = f.read()

    return LaunchDescription([
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            parameters=[{'robot_description': robot_description}],
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(gz_sim_path),
            launch_arguments={'gz_args': 'empty.sdf'}.items(),
        ),
        Node(
            package='ros_gz_sim',
            executable='create',
            arguments=['-topic', 'robot_description', '-name', 'deskbot', '-z', '0.35'],
        ),
    ])