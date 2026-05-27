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
            executable='nats_bridge_node',
            name='nats_bridge_node',
            output='screen',
            parameters=[{
                'nats_url': 'nats://127.0.0.1:4222',
                'subject_c2_command': 'cuas.c2.command',
                'subject_c2_status': 'cuas.c2.status',
                'subject_interceptor_command': 'cuas.interceptor.command',
                'subject_interceptor_status': 'cuas.interceptor.status',
                'subject_heartbeat': 'cuas.datalink.heartbeat'
            }]
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
