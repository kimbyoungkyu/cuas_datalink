from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(package='cuas_datalink', executable='cuas_uplink_node', name='cuas_uplink', output='screen'),
        Node(package='cuas_datalink', executable='cuas_downlink_node', name='cuas_downlink', output='screen'),
    ])
 