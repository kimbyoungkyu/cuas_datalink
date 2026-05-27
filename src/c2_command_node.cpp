#include <chrono>
#include <memory>
#include <string>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include "cuas_datalink/qos_profiles.hpp"
#include "cuas_datalink/topic_names.hpp"
#include <px4_msgs/msg/offboard_control_mode.hpp>
//#include <cuas_msgs/msg/c2_command.hpp>

#include <nats/nats.h>

class C2CommandNode : public rclcpp::Node
{
public:
  C2CommandNode() : Node("c2_command_node"), sequence_(0)
  {
    publish_period_ms_ = this->declare_parameter<int>("publish_period_ms", 1000);
    command_prefix_ = this->declare_parameter<std::string>("command_prefix", "INTERCEPT");
    publisher_ = this->create_publisher<std_msgs::msg::String>(cuas_datalink::topics::C2_COMMAND,cuas_datalink::ReliableControlQoS());
    timer_ = this->create_wall_timer(std::chrono::milliseconds(publish_period_ms_),std::bind(&C2CommandNode::PublishCommand, this));
    RCLCPP_INFO(this->get_logger(), "c2_command_node started");

    ConnectNats();
  }

private:

  void ConnectNats()
  {

    nats_url_ = this->declare_parameter<std::string>("nats_url", "nats://127.0.0.1:4222");

    natsOptions * opts = nullptr;
    natsOptions_Create(&opts);
    natsOptions_SetURL(opts, nats_url_.c_str());
    natsOptions_SetReconnectWait(opts, 500);
    natsOptions_SetMaxReconnect(opts, -1);

    natsStatus status = natsConnection_Connect(&conn_, opts);
    natsOptions_Destroy(opts);

    if (status != NATS_OK) {
      const char * err = natsStatus_GetText(status);
      RCLCPP_FATAL(this->get_logger(), "NATS connect failed: %s", err);
      throw std::runtime_error(err);
    }

    running_ = true;
  }



  void PublishCommand()
  {
    std_msgs::msg::String msg;
    msg.data = command_prefix_ + " seq=" + std::to_string(sequence_++);
    publisher_->publish(msg);
  }


  std::string nats_url_;
  std::string subject_c2_command_;
  std::string subject_c2_status_;
  std::string subject_interceptor_command_;
  std::string subject_interceptor_status_;
  std::string subject_heartbeat_;
  natsConnection * conn_ = nullptr;
  std::vector<natsSubscription *> subscriptions_;
  //std::vector<std::unique_ptr<NatsCallbackContext>> callback_contexts_;
  std::mutex nats_mutex_;
  std::atomic_bool running_;


  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  int publish_period_ms_;
  std::string command_prefix_;
  uint64_t sequence_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<C2CommandNode>());
  rclcpp::shutdown();
  return 0;
}
