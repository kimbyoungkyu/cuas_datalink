#include <memory>
#include <string>
#include <mutex>
#include <atomic>
#include <nats/nats.h>
#include <rclcpp/rclcpp.hpp>
#include <nlohmann/json.hpp>
#include "cuas_datalink/qos_profiles.hpp"
#include "cuas_datalink/topic_names.hpp"
#include "cuas_datalink/jsonConverters.hpp"

class InterceptorReportNode : public rclcpp::Node {
public:
  InterceptorReportNode() : Node("cuas_downlink")
  {
    ConnectNats();

    interceptor_status_sub_ =
      this->create_subscription<cuas_msgs::msg::InterceptorStatus>(
        "/cuas/interceptor/status",
        cuas_datalink::ReliableControlQoS(),
        std::bind(
          &InterceptorReportNode::OnInterceptorStatus,
          this,
          std::placeholders::_1));

    intercept_progress_sub_ =
      this->create_subscription<cuas_msgs::msg::InterceptProgress>(
        "/cuas/interceptor/progress",
        cuas_datalink::ReliableControlQoS(),
        std::bind(
          &InterceptorReportNode::OnInterceptProgress,
          this,
          std::placeholders::_1));


    mission_ack_sub_ =
      this->create_subscription<cuas_msgs::msg::MissionAck>(
        "/cuas/interceptor/ack",
        cuas_datalink::ReliableControlQoS(),
        std::bind(
          &InterceptorReportNode::OnMissionAck,
          this,
          std::placeholders::_1));

    
    engagement_result_sub_ = this->create_subscription<cuas_msgs::msg::EngagementResult>(
        "/cuas/interceptor/result",
        cuas_datalink::ReliableControlQoS(),
        std::bind(&InterceptorReportNode::OnEngagementResult,this,std::placeholders::_1));

    fault_report_sub_ =
      this->create_subscription<cuas_msgs::msg::FaultReport>(
        "/cuas/interceptor/fault",
        cuas_datalink::ReliableControlQoS(),
        std::bind(
          &InterceptorReportNode::OnFaultReport,
          this,
          std::placeholders::_1));




    RCLCPP_INFO(this->get_logger(), "CUAS Downlink Started");
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
    nats_url_ = this->declare_parameter<std::string>("nats_url","nats://127.0.0.1:4222");
    natsOptions * opts = nullptr;
    natsOptions_Create(&opts);
    natsOptions_SetURL(opts, nats_url_.c_str());
    natsOptions_SetReconnectWait(opts, 500);
    natsOptions_SetMaxReconnect(opts, -1);
    natsStatus status = natsConnection_Connect(&conn_, opts);
    natsOptions_Destroy(opts);
    if (status != NATS_OK) {
      const char * err = natsStatus_GetText(status);
      RCLCPP_FATAL(this->get_logger(),"NATS connect failed: %s",err);
      throw std::runtime_error(err);
    }
    running_ = true;
    RCLCPP_INFO(this->get_logger(), "NATS connected successfully");
  }
  void PublishNatsJson(const std::string& subject,const std::string& json)
  {
    std::lock_guard<std::mutex> lock(nats_mutex_);
    if (!running_ || conn_ == nullptr) {
      return;
    }
    natsStatus status = natsConnection_PublishString(conn_,subject.c_str(),json.c_str());
    if (status != NATS_OK) {
      RCLCPP_ERROR(this->get_logger(),"NATS publish failed: subject=%s error=%s",subject.c_str(),natsStatus_GetText(status));
    }
  }
  void OnEngagementResult(const cuas_msgs::msg::EngagementResult::SharedPtr msg)
  {
    RCLCPP_INFO(this->get_logger(), "OnEngagementResult");
    json j = interceptor_report::EngagementResultToJson(msg);
    std::string json_string = j.dump(4);
    RCLCPP_INFO(this->get_logger(), json_string.c_str());
    PublishNatsJson(cuas_datalink::nats_subjects::ENGAGEMENT_RESULT, json_string);
  }
  void OnFaultReport(const cuas_msgs::msg::FaultReport::SharedPtr msg)
  {
    (void)msg;
    RCLCPP_INFO(this->get_logger(), "OnFaultReport");
    json j = interceptor_report::FaultReportToJson(msg);
    std::string json_string = j.dump(4);
    RCLCPP_INFO(this->get_logger(), json_string.c_str());
    PublishNatsJson(cuas_datalink::nats_subjects::FAULT_REPORT, json_string);
  }
  void OnInterceptorStatus(const cuas_msgs::msg::InterceptorStatus::SharedPtr msg)
  {
    (void)msg;
    RCLCPP_INFO(this->get_logger(), "OnInterceptorStatus");
    //std::string json = R"({"type":"interceptor_status"})";
    
    json j = interceptor_report::InterceptorStatusToJson(msg);
    std::string json_string = j.dump(4);
    RCLCPP_INFO(this->get_logger(), json_string.c_str());
    PublishNatsJson(cuas_datalink::nats_subjects::INTERCEPTOR_STATUS, json_string);
  }
  void OnInterceptProgress(const cuas_msgs::msg::InterceptProgress::SharedPtr msg)
  {
    (void)msg;
    RCLCPP_INFO(this->get_logger(), "OnInterceptProgress");
    //std::string json = R"({"type":"intercept_progress"})";

    json j = interceptor_report::InterceptorMissionStatusToJson(msg);
    std::string json_string = j.dump(4);
    RCLCPP_INFO(this->get_logger(), json_string.c_str());
    PublishNatsJson(cuas_datalink::nats_subjects::INTERCEPT_PROGRESS, json_string);
  }
  void OnMissionAck(const cuas_msgs::msg::MissionAck::SharedPtr msg)
  {
    (void)msg;
    RCLCPP_INFO(this->get_logger(), "OnMissionAck");
    json j = interceptor_report::IntercepterMissionAckToJson(msg);
    std::string json_string = j.dump(4);
    RCLCPP_INFO(this->get_logger(), json_string.c_str());
    PublishNatsJson(cuas_datalink::nats_subjects::MISSION_ACK , json_string);
  }

private:
  std::string nats_url_;
  natsConnection * conn_ = nullptr;
  std::mutex nats_mutex_;
  std::atomic_bool running_{false};
  rclcpp::Subscription<cuas_msgs::msg::EngagementResult>::SharedPtr engagement_result_sub_;
  rclcpp::Subscription<cuas_msgs::msg::FaultReport>::SharedPtr fault_report_sub_;
  rclcpp::Subscription<cuas_msgs::msg::InterceptMission>::SharedPtr interceptor_mission_sub_;
  rclcpp::Subscription<cuas_msgs::msg::InterceptorStatus>::SharedPtr interceptor_status_sub_;
  rclcpp::Subscription<cuas_msgs::msg::InterceptProgress>::SharedPtr intercept_progress_sub_;
  rclcpp::Subscription<cuas_msgs::msg::MissionAck>::SharedPtr mission_ack_sub_;
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