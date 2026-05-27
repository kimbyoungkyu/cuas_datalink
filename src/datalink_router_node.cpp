#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

#include "cuas_datalink/qos_profiles.hpp"
#include "cuas_datalink/topic_names.hpp"

class DataLinkRouterNode : public rclcpp::Node
{
public:
  DataLinkRouterNode()
  : Node("datalink_router_node")
  {
    c2_command_sub_ = this->create_subscription<std_msgs::msg::String>(
      cuas_datalink::topics::C2_COMMAND,
      cuas_datalink::ReliableControlQoS(),
      std::bind(&DataLinkRouterNode::OnC2Command, this, std::placeholders::_1));

    interceptor_status_sub_ = this->create_subscription<std_msgs::msg::String>(
      cuas_datalink::topics::INTERCEPTOR_STATUS,
      cuas_datalink::BestEffortTelemetryQoS(),
      std::bind(&DataLinkRouterNode::OnInterceptorStatus, this, std::placeholders::_1));

    interceptor_command_pub_ = this->create_publisher<std_msgs::msg::String>(
      cuas_datalink::topics::INTERCEPTOR_COMMAND,
      cuas_datalink::ReliableControlQoS());

    c2_status_pub_ = this->create_publisher<std_msgs::msg::String>(
      cuas_datalink::topics::C2_STATUS,
      cuas_datalink::BestEffortTelemetryQoS());

    RCLCPP_INFO(this->get_logger(), "datalink_router_node started");
  }

private:
  void OnC2Command(const std_msgs::msg::String::SharedPtr msg)
  {
    interceptor_command_pub_->publish(*msg);
  }

  void OnInterceptorStatus(const std_msgs::msg::String::SharedPtr msg)
  {
    c2_status_pub_->publish(*msg);
  }

  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr c2_command_sub_;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr interceptor_status_sub_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr interceptor_command_pub_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr c2_status_pub_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::executors::MultiThreadedExecutor executor;
  auto node = std::make_shared<DataLinkRouterNode>();
  executor.add_node(node);
  executor.spin();
  rclcpp::shutdown();
  return 0;
}
