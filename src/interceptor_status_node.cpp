#include <chrono>
#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

#include "cuas_datalink/qos_profiles.hpp"
#include "cuas_datalink/topic_names.hpp"

class InterceptorStatusNode : public rclcpp::Node
{
public:
  InterceptorStatusNode()
  : Node("interceptor_status_node"), sequence_(0)
  {
    publish_period_ms_ = this->declare_parameter<int>("publish_period_ms", 500);
    interceptor_id_ = this->declare_parameter<std::string>("interceptor_id", "interceptor_001");

    publisher_ = this->create_publisher<std_msgs::msg::String>(
      cuas_datalink::topics::INTERCEPTOR_STATUS,
      cuas_datalink::BestEffortTelemetryQoS());

    timer_ = this->create_wall_timer(
      std::chrono::milliseconds(publish_period_ms_),
      std::bind(&InterceptorStatusNode::PublishStatus, this));

    RCLCPP_INFO(this->get_logger(), "interceptor_status_node started");
  }

private:
  void PublishStatus()
  {
    std_msgs::msg::String msg;
    msg.data = interceptor_id_ + " status=READY seq=" + std::to_string(sequence_++);
    publisher_->publish(msg);
  }

  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  int publish_period_ms_;
  std::string interceptor_id_;
  uint64_t sequence_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<InterceptorStatusNode>());
  rclcpp::shutdown();
  return 0;
}
