#include <chrono>
#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

#include "cuas_datalink/qos_profiles.hpp"
#include "cuas_datalink/topic_names.hpp"

class HeartbeatNode : public rclcpp::Node
{
public:
  HeartbeatNode()
  : Node("heartbeat_node"), sequence_(0)
  {
    publish_period_ms_ = this->declare_parameter<int>("publish_period_ms", 1000);
    link_id_ = this->declare_parameter<std::string>("link_id", "cuas_datalink");

    publisher_ = this->create_publisher<std_msgs::msg::String>(
      cuas_datalink::topics::HEARTBEAT,
      cuas_datalink::BestEffortTelemetryQoS());

    timer_ = this->create_wall_timer(
      std::chrono::milliseconds(publish_period_ms_),
      std::bind(&HeartbeatNode::PublishHeartbeat, this));

    RCLCPP_INFO(this->get_logger(), "heartbeat_node started");
  }

private:
  void PublishHeartbeat()
  {
    std_msgs::msg::String msg;
    msg.data = link_id_ + " alive seq=" + std::to_string(sequence_++);
    publisher_->publish(msg);
  }

  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  int publish_period_ms_;
  std::string link_id_;
  uint64_t sequence_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<HeartbeatNode>());
  rclcpp::shutdown();
  return 0;
}
