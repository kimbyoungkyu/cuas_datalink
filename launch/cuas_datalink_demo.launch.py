from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(package='cuas_datalink', executable='c2_command_node', name='c2_command_node', output='screen'),
        Node(package='cuas_datalink', executable='interceptor_report_node', name='interceptor_report_node', output='screen'),
    ])
 