#include <memory>
#include <string>
#include <mutex>
#include <atomic>
#include <nats/nats.h>
#include <rclcpp/rclcpp.hpp>
#include <nlohmann/json.hpp>
#include "cuas_datalink/qos_profiles.hpp"
#include "cuas_datalink/topic_names.hpp"
#include "cuas_msgs/msg/engagement_result.hpp"
#include "cuas_msgs/msg/fault_report.hpp"
#include "cuas_msgs/msg/interceptor_status.hpp"
#include "cuas_msgs/msg/intercept_progress.hpp"
#include "cuas_msgs/msg/mission_ack.hpp"



using json = nlohmann::json;

class InterceptorReportNode : public rclcpp::Node
{
public:
  InterceptorReportNode() : Node("interceptor_report_node")
  {
    ConnectNats();

    engagement_result_sub_ =
      this->create_subscription<cuas_msgs::msg::EngagementResult>(
        
        cuas_datalink::topics::ENGAGEMENT_RESULT,
        cuas_datalink::ReliableControlQoS(),
        std::bind(
          &InterceptorReportNode::OnEngagementResult,
          this,
          std::placeholders::_1));

    fault_report_sub_ =
      this->create_subscription<cuas_msgs::msg::FaultReport>(
        cuas_datalink::topics::FAULT_REPORT,
        cuas_datalink::ReliableControlQoS(),
        std::bind(
          &InterceptorReportNode::OnFaultReport,
          this,
          std::placeholders::_1));

    interceptor_status_sub_ =
      this->create_subscription<cuas_msgs::msg::InterceptorStatus>(
        cuas_datalink::topics::INTERCEPTOR_STATUS,
        cuas_datalink::ReliableControlQoS(),
        std::bind(
          &InterceptorReportNode::OnInterceptorStatus,
          this,
          std::placeholders::_1));

    intercept_progress_sub_ =
      this->create_subscription<cuas_msgs::msg::InterceptProgress>(
        cuas_datalink::topics::INTERCEPT_PROGRESS,
        cuas_datalink::ReliableControlQoS(),
        std::bind(
          &InterceptorReportNode::OnInterceptProgress,
          this,
          std::placeholders::_1));

    mission_ack_sub_ =
      this->create_subscription<cuas_msgs::msg::MissionAck>(
        cuas_datalink::topics::MISSION_ACK,
        cuas_datalink::ReliableControlQoS(),
        std::bind(
          &InterceptorReportNode::OnMissionAck,
          this,
          std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "interceptor_report_node started");
  }

  ~InterceptorReportNode()
  {
    running_ = false;

    if (conn_ != nullptr) {
      natsConnection_Destroy(conn_);
      conn_ = nullptr;
    }

    nats_Close();
  }

private:
  void ConnectNats()
  {
    nats_url_ =
      this->declare_parameter<std::string>(
        "nats_url",
        "nats://127.0.0.1:4222");

    natsOptions * opts = nullptr;

    natsOptions_Create(&opts);
    natsOptions_SetURL(opts, nats_url_.c_str());
    natsOptions_SetReconnectWait(opts, 500);
    natsOptions_SetMaxReconnect(opts, -1);

    natsStatus status = natsConnection_Connect(&conn_, opts);

    natsOptions_Destroy(opts);

    if (status != NATS_OK) {
      const char * err = natsStatus_GetText(status);

      RCLCPP_FATAL(
        this->get_logger(),
        "NATS connect failed: %s",
        err);

      throw std::runtime_error(err);
    }

    running_ = true;

    RCLCPP_INFO(this->get_logger(), "NATS connected successfully");
  }

  void PublishNatsJson(
    const std::string& subject,
    const std::string& json)
  {
    std::lock_guard<std::mutex> lock(nats_mutex_);

    if (!running_ || conn_ == nullptr) {
      return;
    }

    natsStatus status =
      natsConnection_PublishString(
        conn_,
        subject.c_str(),
        json.c_str());

    if (status != NATS_OK) {
      RCLCPP_ERROR(
        this->get_logger(),
        "NATS publish failed: subject=%s error=%s",
        subject.c_str(),
        natsStatus_GetText(status));
    }
  }

  void OnEngagementResult(
    const cuas_msgs::msg::EngagementResult::SharedPtr msg)
  {
    (void)msg;

    std::string json =
      R"({"type":"engagement_result"})";

    PublishNatsJson(cuas_datalink::nats_subjects::ENGAGEMENT_RESULT, json);
  }

  void OnFaultReport(
    const cuas_msgs::msg::FaultReport::SharedPtr msg)
  {
    (void)msg;

    std::string json =
      R"({"type":"fault_report"})";

    PublishNatsJson(cuas_datalink::nats_subjects::FAULT_REPORT, json);
  }

  void OnInterceptorStatus(
    const cuas_msgs::msg::InterceptorStatus::SharedPtr msg)
  {
    (void)msg;

    std::string json =
      R"({"type":"interceptor_status"})";

    PublishNatsJson(cuas_datalink::nats_subjects::INTERCEPTOR_STATUS, json);
  }

  void OnInterceptProgress(
    const cuas_msgs::msg::InterceptProgress::SharedPtr msg)
  {
    (void)msg;

    std::string json =
      R"({"type":"intercept_progress"})";

    PublishNatsJson(cuas_datalink::nats_subjects::INTERCEPT_PROGRESS, json);
  }

  void OnMissionAck(
    const cuas_msgs::msg::MissionAck::SharedPtr msg)
  {
    (void)msg;

    std::string json =
      R"({"type":"mission_ack"})";

    PublishNatsJson(cuas_datalink::nats_subjects::MISSION_ACK , json);
  }

private:
  std::string nats_url_;

  natsConnection * conn_ = nullptr;

  std::mutex nats_mutex_;

  std::atomic_bool running_{false};

  rclcpp::Subscription<cuas_msgs::msg::EngagementResult>::SharedPtr
    engagement_result_sub_;

  rclcpp::Subscription<cuas_msgs::msg::FaultReport>::SharedPtr
    fault_report_sub_;

  rclcpp::Subscription<cuas_msgs::msg::InterceptorStatus>::SharedPtr
    interceptor_status_sub_;

  rclcpp::Subscription<cuas_msgs::msg::InterceptProgress>::SharedPtr
    intercept_progress_sub_;

  rclcpp::Subscription<cuas_msgs::msg::MissionAck>::SharedPtr
    mission_ack_sub_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  rclcpp::executors::MultiThreadedExecutor executor;

  auto node = std::make_shared<InterceptorReportNode>();

  executor.add_node(node);
  executor.spin();

  rclcpp::shutdown();

  return 0;
}