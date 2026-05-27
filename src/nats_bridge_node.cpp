#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

#include <nats/nats.h>

#include "cuas_datalink/qos_profiles.hpp"
#include "cuas_datalink/topic_names.hpp"

class NatsBridgeNode : public rclcpp::Node
{
public:
  NatsBridgeNode()
  : Node("nats_bridge_node"), running_(false)
  {
    nats_url_ = this->declare_parameter<std::string>("nats_url", "nats://127.0.0.1:4222");

    subject_c2_command_ = this->declare_parameter<std::string>(
      "subject_c2_command", cuas_datalink::nats_subjects::C2_COMMAND);
    subject_c2_status_ = this->declare_parameter<std::string>(
      "subject_c2_status", cuas_datalink::nats_subjects::C2_STATUS);
    subject_interceptor_command_ = this->declare_parameter<std::string>(
      "subject_interceptor_command", cuas_datalink::nats_subjects::INTERCEPTOR_COMMAND);
    subject_interceptor_status_ = this->declare_parameter<std::string>(
      "subject_interceptor_status", cuas_datalink::nats_subjects::INTERCEPTOR_STATUS);
    subject_heartbeat_ = this->declare_parameter<std::string>(
      "subject_heartbeat", cuas_datalink::nats_subjects::HEARTBEAT);

    ConnectNats();

    c2_command_pub_ = this->create_publisher<std_msgs::msg::String>(
      cuas_datalink::topics::C2_COMMAND,
      cuas_datalink::ReliableControlQoS());

    interceptor_status_pub_ = this->create_publisher<std_msgs::msg::String>(
      cuas_datalink::topics::INTERCEPTOR_STATUS,
      cuas_datalink::BestEffortTelemetryQoS());

    interceptor_command_sub_ = this->create_subscription<std_msgs::msg::String>(
      cuas_datalink::topics::INTERCEPTOR_COMMAND,
      cuas_datalink::ReliableControlQoS(),
      std::bind(&NatsBridgeNode::OnRosInterceptorCommand, this, std::placeholders::_1));

    c2_status_sub_ = this->create_subscription<std_msgs::msg::String>(
      cuas_datalink::topics::C2_STATUS,
      cuas_datalink::BestEffortTelemetryQoS(),
      std::bind(&NatsBridgeNode::OnRosC2Status, this, std::placeholders::_1));

    heartbeat_sub_ = this->create_subscription<std_msgs::msg::String>(
      cuas_datalink::topics::HEARTBEAT,
      cuas_datalink::BestEffortTelemetryQoS(),
      std::bind(&NatsBridgeNode::OnRosHeartbeat, this, std::placeholders::_1));

    SubscribeNats(subject_c2_command_, &NatsBridgeNode::OnNatsC2Command);
    SubscribeNats(subject_interceptor_status_, &NatsBridgeNode::OnNatsInterceptorStatus);

    RCLCPP_INFO(this->get_logger(), "nats_bridge_node started. url=%s", nats_url_.c_str());
  }

  ~NatsBridgeNode() override
  {
    running_ = false;

    for (auto * sub : subscriptions_) {
      if (sub != nullptr) {
        natsSubscription_Destroy(sub);
      }
    }

    if (conn_ != nullptr) {
      natsConnection_Drain(conn_);
      natsConnection_Destroy(conn_);
      conn_ = nullptr;
    }
  }

private:
  void ConnectNats()
  {
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

  using Handler = void (NatsBridgeNode::*)(const std::string & payload);

  static void NatsMessageCallback(natsConnection *, natsSubscription *, natsMsg * msg, void * closure)
  {
    auto * context = static_cast<NatsCallbackContext *>(closure);
    if (context != nullptr && context->node != nullptr) {
      const char * data = natsMsg_GetData(msg);
      int len = natsMsg_GetDataLength(msg);
      std::string payload(data, data + len);
      (context->node->*(context->handler))(payload);
    }
    natsMsg_Destroy(msg);
  }

  struct NatsCallbackContext
  {
    NatsBridgeNode * node;
    Handler handler;
  };

  void SubscribeNats(const std::string & subject, Handler handler)
  {
    natsSubscription * sub = nullptr;
    auto * context = new NatsCallbackContext{this, handler};
    callback_contexts_.emplace_back(context);

    natsStatus status = natsConnection_Subscribe(
      &sub,
      conn_,
      subject.c_str(),
      &NatsBridgeNode::NatsMessageCallback,
      context);

    if (status != NATS_OK) {
      RCLCPP_ERROR(this->get_logger(), "NATS subscribe failed: %s", subject.c_str());
      return;
    }

    subscriptions_.push_back(sub);
    RCLCPP_INFO(this->get_logger(), "NATS subscribed: %s", subject.c_str());
  }

  void PublishNats(const std::string & subject, const std::string & payload)
  {
    if (!running_ || conn_ == nullptr) {
      return;
    }

    std::lock_guard<std::mutex> lock(nats_mutex_);
    natsStatus status = natsConnection_Publish(
      conn_,
      subject.c_str(),
      payload.data(),
      static_cast<int>(payload.size()));

    if (status == NATS_OK) {
      natsConnection_FlushTimeout(conn_, 10);
    }
  }

  void OnNatsC2Command(const std::string & payload)
  {
    std_msgs::msg::String msg;
    msg.data = payload;
    c2_command_pub_->publish(msg);
  }

  void OnNatsInterceptorStatus(const std::string & payload)
  {
    std_msgs::msg::String msg;
    msg.data = payload;
    interceptor_status_pub_->publish(msg);
  }

  void OnRosInterceptorCommand(const std_msgs::msg::String::SharedPtr msg)
  {
    PublishNats(subject_interceptor_command_, msg->data);
  }

  void OnRosC2Status(const std_msgs::msg::String::SharedPtr msg)
  {
    PublishNats(subject_c2_status_, msg->data);
  }

  void OnRosHeartbeat(const std_msgs::msg::String::SharedPtr msg)
  {
    PublishNats(subject_heartbeat_, msg->data);
  }

private:
  std::string nats_url_;

  std::string subject_c2_command_;
  std::string subject_c2_status_;
  std::string subject_interceptor_command_;
  std::string subject_interceptor_status_;
  std::string subject_heartbeat_;

  natsConnection * conn_ = nullptr;
  std::vector<natsSubscription *> subscriptions_;
  std::vector<std::unique_ptr<NatsCallbackContext>> callback_contexts_;

  std::mutex nats_mutex_;
  std::atomic_bool running_;

  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr c2_command_pub_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr interceptor_status_pub_;

  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr interceptor_command_sub_;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr c2_status_sub_;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr heartbeat_sub_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  rclcpp::executors::MultiThreadedExecutor executor;
  auto node = std::make_shared<NatsBridgeNode>();
  executor.add_node(node);
  executor.spin();

  rclcpp::shutdown();
  return 0;
}
