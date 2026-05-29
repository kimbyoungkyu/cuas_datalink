#include <chrono>
#include <memory>
#include <string>
#include <mutex>
#include <atomic>
#include <vector>
#include <nats/nats.h>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include "cuas_datalink/qos_profiles.hpp"
#include "cuas_datalink/topic_names.hpp"
#include "cuas_datalink/jsonConverters.hpp"

//C2 -> Interceptor Command, Mission Assignment, Target Track
class C2CommandNode : public rclcpp::Node
{
public:
  C2CommandNode()  : Node("c2_command_node")
  {
    c2_command_pub_ = this->create_publisher<cuas_msgs::msg::C2Command>(cuas_datalink::topics::C2_COMMAND,cuas_datalink::ReliableControlQoS());
    intercept_mission_pub_ = this->create_publisher<cuas_msgs::msg::InterceptMission>(cuas_datalink::topics::INTERCEPT_MISSION,cuas_datalink::ReliableControlQoS());
    target_track_pub_ = this->create_publisher<cuas_msgs::msg::TargetTrack>("/cuas/c2/target_track",cuas_datalink::ReliableControlQoS());
    ConnectNats();
    SubscribeNats();
    RCLCPP_INFO(this->get_logger(), "c2_command_node started");
  }

  ~C2CommandNode()
  {
    running_ = false;
    for (auto * sub : subscriptions_) {
      if (sub != nullptr) {
        natsSubscription_Destroy(sub);
      }
    }
    if (conn_ != nullptr) {
      natsConnection_Destroy(conn_);
    }
    nats_Close();
  }

private:
  // =========================================================
  // NATS
  // =========================================================
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
      RCLCPP_FATAL(this->get_logger(), "NATS connect failed: %s",err);
      throw std::runtime_error(err);
    }
    running_ = true;
    RCLCPP_INFO(this->get_logger(),"NATS connected successfully");
  }

  void SubscribeNats()
  {
    Subscribe("cuas.c2.command", &C2CommandNode::HandleC2Command);
    Subscribe("cuas.c2.mission", &C2CommandNode::HandleInterceptMission);
    Subscribe("cuas.c2.target_track", &C2CommandNode::HandleTargetTrack);
  }

  using HandlerFunc = void (C2CommandNode::*)(const std::string& data,const std::string& reply);

  struct CallbackContext
  {
    C2CommandNode * self;
    HandlerFunc handler;
  };

  static void OnNatsMessage(natsConnection * nc,natsSubscription * sub, natsMsg * msg,void * closure)
  {
    (void)nc;
    (void)sub;

    auto * context = static_cast<CallbackContext*>(closure);

    if (context == nullptr || context->self == nullptr) {
      natsMsg_Destroy(msg);
      return;
    }

    std::string data(natsMsg_GetData(msg),natsMsg_GetDataLength(msg));

    std::string reply;
    if (natsMsg_GetReply(msg) != nullptr) {
      reply = natsMsg_GetReply(msg);
    }

    (context->self->*(context->handler))(data, reply);

    natsMsg_Destroy(msg);
  }

  void Subscribe(const std::string& subject,HandlerFunc handler)
  {
    auto context = std::make_unique<CallbackContext>();

    context->self = this;
    context->handler = handler;

    natsSubscription * sub = nullptr;

    natsStatus status = natsConnection_Subscribe(&sub,conn_,subject.c_str(),OnNatsMessage,context.get());
    if (status != NATS_OK) {
      RCLCPP_ERROR(this->get_logger(),"Failed subscribe: %s",subject.c_str());
      return;
    }

    callback_contexts_.push_back(std::move(context));
    subscriptions_.push_back(sub);

    RCLCPP_INFO(this->get_logger(),"NATS Subscribed: %s",subject.c_str());
  }

  // =========================================================
  // Dummy Handlers
  // =========================================================

  void HandleC2Command(const std::string& data,const std::string& reply)
  {
    RCLCPP_INFO(this->get_logger(),"[cuas.c2.mission] %s",data.c_str());
    //json을 파싱해서 cuas_msgs::msg::C2Command 메시지로 변환하는 로직이 들어갈 자리
    
    
    auto c2_command_msg = cuas_msgs::msg::C2Command();
    c2_command_json::from_json(json::parse(data), c2_command_msg);
    c2_command_pub_->publish(c2_command_msg);

    // TODO:
    // FCUASC2Command Parse
    if (!reply.empty()) {
      std::string response = R"({"result":"ok","type":"c2_command"})";
      natsConnection_PublishString(conn_,reply.c_str(),response.c_str());
    }
  }

  void HandleInterceptMission(const std::string& data,const std::string& reply)
  {
    RCLCPP_INFO(this->get_logger(),"[cuas.c2.command] %s",data.c_str());
    auto intercept_mission_msg = cuas_msgs::msg::InterceptMission();

    //intercept_mission_json::from_json(json::parse(data), intercept_mission_msg);
    intercept_mission_pub_->publish(intercept_mission_msg);

    // TODO:
    // FCUASMissionAssignment Parse
    if (!reply.empty()) {
      std::string response = R"({"result":"ok","type":"mission"})";
      natsConnection_PublishString(conn_,reply.c_str(),response.c_str());
    }
  }

  void HandleTargetTrack(const std::string& data,const std::string& reply)
  {
    RCLCPP_INFO(this->get_logger(),"[cuas.c2.target_track] %s",data.c_str());
    //json을 파싱해서 cuas_msgs::msg::TargetTrack 메시지로 변환하는 로직이 들어갈 자리
    auto target_track_msg = cuas_msgs::msg::TargetTrack();
    target_track_json::from_json(json::parse(data), target_track_msg);
    target_track_pub_->publish(target_track_msg);
    // FCUASTargetTrack Parse 
    
    if (!reply.empty()) {
      std::string response = R"({"result":"ok","type":"target_track"})";
      natsConnection_PublishString(conn_,reply.c_str(),response.c_str());
    }
  }


private:

  // =========================================================
  // NATS
  // =========================================================

  std::string nats_url_;

  natsConnection * conn_ = nullptr;

  std::vector<natsSubscription *> subscriptions_;

  std::vector<std::unique_ptr<CallbackContext>> callback_contexts_;

  std::mutex nats_mutex_;

  std::atomic_bool running_;

  // =========================================================
  // ROS2
  // =========================================================


  rclcpp::Publisher<cuas_msgs::msg::C2Command>::SharedPtr c2_command_pub_;
  rclcpp::Publisher<cuas_msgs::msg::InterceptMission>::SharedPtr intercept_mission_pub_;
  rclcpp::Publisher<cuas_msgs::msg::TargetTrack>::SharedPtr target_track_pub_;

  //rclcpp::TimerBase::SharedPtr timer_;

  //int publish_period_ms_;


};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_shared<C2CommandNode>());

  rclcpp::shutdown();

  return 0;
}