from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(
            package='cuas_datalink',
            executable='datalink_router_node',
            name='datalink_router_node',
            output='screen'
        ),
        Node(
            package='cuas_datalink',
            executable='heartbeat_node',
            name='heartbeat_node',
            output='screen',
            parameters=[{
                'publish_period_ms': 1000,
                'link_id': 'cuas_datalink'
            }]
        ),
    ])
