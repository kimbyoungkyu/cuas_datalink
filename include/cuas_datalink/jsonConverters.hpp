#pragma once

#include <nlohmann/json.hpp>
#include "cuas_datalink/qos_profiles.hpp"
#include "cuas_datalink/topic_names.hpp"
#include "cuas_msgs/msg/c2_command.hpp"
#include "cuas_msgs/msg/engagement_result.hpp"
#include "cuas_msgs/msg/fault_report.hpp"
#include "cuas_msgs/msg/intercept_mission.hpp"
#include "cuas_msgs/msg/interceptor_status.hpp"
#include "cuas_msgs/msg/intercept_progress.hpp"
#include "cuas_msgs/msg/mission_ack.hpp"
#include "cuas_msgs/msg/target_track.hpp"
using json = nlohmann::json;

namespace c2_command_json
{
  inline void from_json(const json& j,cuas_msgs::msg::C2Command& msg)
  {
    if (j.contains("stamp"))
    {
      const auto& stamp = j["stamp"];

      msg.stamp.sec =
        stamp.value("sec", 0);

      msg.stamp.nanosec =
        stamp.value("nanosec", 0u);
    }

    // -------------------------------------------------------
    // string
    // -------------------------------------------------------

    msg.command_id =
      j.value("commandId", "");

    msg.mission_id =
      j.value("missionId", "");

    msg.interceptor_id =
      j.value("interceptorId", "");

    msg.target_id =
      j.value("targetId", "");

    msg.reason =
      j.value("reason", "");

    // -------------------------------------------------------
    // uint8
    // -------------------------------------------------------

    msg.command_type =
      static_cast<uint8_t>(
        j.value("commandType", 0));
  }

  // =========================================================
  // ROS2 -> JSON
  // =========================================================

  inline void to_json(json& j,const cuas_msgs::msg::C2Command& msg)
  {
    j =
    {
      {
        "stamp",
        {
          {"sec", msg.stamp.sec},
          {"nanosec", msg.stamp.nanosec}
        }
      },

      {"commandId", msg.command_id},
      {"missionId", msg.mission_id},
      {"interceptorId", msg.interceptor_id},
      {"targetId", msg.target_id},

      {"commandType", msg.command_type},

      {"reason", msg.reason}
    };
  }
}

namespace engagement_result_json
{
  inline void ParseStamp(const json& j,builtin_interfaces::msg::Time& stamp)
  {
    stamp.sec = j.value("sec", 0);
    stamp.nanosec = j.value("nanosec", 0u);
  }
  inline json ToJsonStamp(const builtin_interfaces::msg::Time& stamp)
  {
    return
    {
      {"sec", stamp.sec},
      {"nanosec", stamp.nanosec}
    };
  }
  inline void from_json(const json& j,cuas_msgs::msg::EngagementResult& msg)
  {
    if (j.contains("stamp"))
    {
      ParseStamp(j["stamp"], msg.stamp);
    }

    msg.mission_id =
      j.value("missionId", "");

    msg.interceptor_id =
      j.value("interceptorId", "");

    msg.target_id =
      j.value("targetId", "");

    msg.result =
      static_cast<uint8_t>(
        j.value("result", 0));

    msg.final_distance =
      j.value("finalDistance", 0.0f);

    msg.engagement_time_sec =
      j.value("engagementTimeSec", 0.0f);

    msg.summary =
      j.value("summary", "");
  }
  inline json to_json(const cuas_msgs::msg::EngagementResult& msg)
  {
    return
    {
      {"stamp", ToJsonStamp(msg.stamp)},

      {"missionId", msg.mission_id},
      {"interceptorId", msg.interceptor_id},
      {"targetId", msg.target_id},

      {"result", msg.result},

      {"finalDistance", msg.final_distance},

      {"engagementTimeSec", msg.engagement_time_sec},

      {"summary", msg.summary}
    };
  }
}

namespace fault_report_json
{
  inline void ParseStamp(const json& j,builtin_interfaces::msg::Time& stamp)
  {
    stamp.sec = j.value("sec", 0);
    stamp.nanosec = j.value("nanosec", 0u);
  }

  inline json ToJsonStamp(const builtin_interfaces::msg::Time& stamp)
  {
    return {
      {"sec", stamp.sec},
      {"nanosec", stamp.nanosec}
    };
  }

  inline void from_json(const json& j,cuas_msgs::msg::FaultReport& msg)
  {
    if (j.contains("stamp")) {
      ParseStamp(j["stamp"], msg.stamp);
    }

    msg.interceptor_id = j.value("interceptorId", "");
    msg.mission_id = j.value("missionId", "");

    msg.severity =
      static_cast<uint8_t>(j.value("severity", 0));

    msg.fault_code =
      static_cast<uint16_t>(j.value("faultCode", 0));

    msg.fault_name = j.value("faultName", "");
    msg.description = j.value("description", "");

    msg.requires_abort = j.value("requiresAbort", false);
  }

  inline json to_json(const cuas_msgs::msg::FaultReport& msg)
  {
    return {
      {"stamp", ToJsonStamp(msg.stamp)},

      {"interceptorId", msg.interceptor_id},
      {"missionId", msg.mission_id},

      {"severity", msg.severity},
      {"faultCode", msg.fault_code},
      {"faultName", msg.fault_name},
      {"description", msg.description},

      {"requiresAbort", msg.requires_abort}
    };
  }

} // namespace fault_report_json

namespace target_track_json
{
  // =========================================================
  // Time
  // =========================================================

  inline void ParseStamp(
    const json& j,
    builtin_interfaces::msg::Time& stamp)
  {
    stamp.sec = j.value("sec", 0);
    stamp.nanosec = j.value("nanosec", 0u);
  }

  inline json ToJsonStamp(const builtin_interfaces::msg::Time& stamp)
  {
    return {
      {"sec", stamp.sec},
      {"nanosec", stamp.nanosec}
    };
  }

  // =========================================================
  // Track State
  // =========================================================

  inline uint8_t ParseTrackState(const json& j)
  {
    if (!j.contains("trackState") || j["trackState"].is_null())
    {
      return 0;
    }

    const auto& v = j["trackState"];

    // 숫자 처리
    if (v.is_number_integer())
    {
      return static_cast<uint8_t>(v.get<int>());
    }

    // 문자열 처리
    if (v.is_string())
    {
      const std::string s = v.get<std::string>();

      if (s == "Tracking")   return 1;
      if (s == "Detected")   return 2;
      if (s == "Lost")       return 3;
      if (s == "Tentative")  return 4;
      if (s == "Confirmed")  return 5;

      return 0;
    }

    return 0;
  }

  inline std::string TrackStateToString(uint8_t state)
  {
    switch (state)
    {
      case 1: return "Tracking";
      case 2: return "Detected";
      case 3: return "Lost";
      case 4: return "Tentative";
      case 5: return "Confirmed";
      default: return "Unknown";
    }
  }

  // =========================================================
  // From JSON
  // =========================================================

  inline void from_json(
    const json& j,
    cuas_msgs::msg::TargetTrack& msg)
  {
    // -----------------------------
    // Stamp
    // -----------------------------
    if (j.contains("stamp"))
    {
      ParseStamp(j["stamp"], msg.stamp);
    }

    // -----------------------------
    // Target
    // -----------------------------
    msg.target_id =
      j.value("targetId", "");

    // -----------------------------
    // Position
    // -----------------------------
    msg.latitude =
      j.value("latitude", 0.0);

    msg.longitude =
      j.value("longitude", 0.0);

    msg.altitude =
      j.value("altitude", 0.0f);

    // -----------------------------
    // Velocity
    // -----------------------------
    msg.velocity_x =
      j.value("velocityX", 0.0f);

    msg.velocity_y =
      j.value("velocityY", 0.0f);

    msg.velocity_z =
      j.value("velocityZ", 0.0f);

    // -----------------------------
    // Track Info
    // -----------------------------
    msg.heading =
      j.value("heading", 0.0f);

    msg.confidence =
      j.value("confidence", 0.0f);

    msg.track_state =
      ParseTrackState(j);
  }

  // =========================================================
  // To JSON
  // =========================================================

  inline json to_json(
    const cuas_msgs::msg::TargetTrack& msg)
  {
    return {
      // -----------------------------
      // Stamp
      // -----------------------------
      {
        "stamp",
        ToJsonStamp(msg.stamp)
      },

      // -----------------------------
      // Target
      // -----------------------------
      {
        "targetId",
        msg.target_id
      },

      // -----------------------------
      // Position
      // -----------------------------
      {
        "latitude",
        msg.latitude
      },
      {
        "longitude",
        msg.longitude
      },
      {
        "altitude",
        msg.altitude
      },

      // -----------------------------
      // Velocity
      // -----------------------------
      {
        "velocityX",
        msg.velocity_x
      },
      {
        "velocityY",
        msg.velocity_y
      },
      {
        "velocityZ",
        msg.velocity_z
      },

      // -----------------------------
      // Track Info
      // -----------------------------
      {
        "heading",
        msg.heading
      },
      {
        "confidence",
        msg.confidence
      },

      // 문자열로 출력
      {
        "trackState",
        TrackStateToString(msg.track_state)
      }
    };
  }

} // namespace target_track_jso


namespace interceptor_report
{
  inline std::string ResultToString(uint8_t result)
  {
      switch (result)
      {
          case 0: return "UNKNOWN";
          case 1: return "SUCCESS_SIM";
          case 2: return "MISSED_SIM";
          case 3: return "ABORTED";
          case 4: return "TARGET_LOST";
          case 5: return "SYSTEM_FAULT";
          case 6: return "SAFETY_ABORT";
          default: return "INVALID";
      }
  }

  inline json EngagementResultToJson(const cuas_msgs::msg::EngagementResult::SharedPtr& msg)
  {
      return {
          {
              "stamp",
              {
                  {"sec", msg->stamp.sec},
                  {"nanosec", msg->stamp.nanosec}
              }
          },

          {"mission_id", msg->mission_id},
          {"interceptor_id", msg->interceptor_id},
          {"target_id", msg->target_id},

          {"result", msg->result},
          {"result_text", ResultToString(msg->result)},

          {"final_distance", msg->final_distance},
          {"engagement_time_sec", msg->engagement_time_sec},

          {"summary", msg->summary}
      };
  }


  inline std::string SeverityToString(uint8_t severity)
  {
      switch (severity)
      {
          case 0: return "INFO";
          case 1: return "WARNING";
          case 2: return "ERROR";
          case 3: return "CRITICAL";
          default: return "UNKNOWN";
      }
  }

  inline json FaultReportToJson(const cuas_msgs::msg::FaultReport::SharedPtr& msg)
  {
      return {
          {
              "stamp",
              {
                  {"sec", msg->stamp.sec},
                  {"nanosec", msg->stamp.nanosec}
              }
          },

          {"interceptor_id", msg->interceptor_id},
          {"mission_id", msg->mission_id},

          {"severity", msg->severity},
          {"severity_text", SeverityToString(msg->severity)},

          {"fault_code", msg->fault_code},
          {"fault_name", msg->fault_name},
          {"description", msg->description},

          {"requires_abort", msg->requires_abort}
      };
  }


  inline json InterceptorStatusToJson(const cuas_msgs::msg::InterceptorStatus::SharedPtr& msg)
  {
      return {
          {
              "stamp",
              {
                  {"sec", msg->stamp.sec},
                  {"nanosec", msg->stamp.nanosec}
              }
          },

          {"interceptor_id", msg->interceptor_id},
          {"mission_id", msg->mission_id},

          {"vehicle_state", msg->vehicle_state},

          {"latitude", msg->latitude},
          {"longitude", msg->longitude},
          {"altitude", msg->altitude},

          {"velocity_x", msg->velocity_x},
          {"velocity_y", msg->velocity_y},
          {"velocity_z", msg->velocity_z},

          {"battery_remaining", msg->battery_remaining},

          {"armed", msg->armed},
          {"offboard_enabled", msg->offboard_enabled},
          {"healthy", msg->healthy}
      };
  }  

  inline json InterceptorMissionStatusToJson(const cuas_msgs::msg::InterceptProgress::SharedPtr& msg)
  {
      return {
          {
              "stamp",
              {
                  {"sec", msg->stamp.sec},
                  {"nanosec", msg->stamp.nanosec}
              }
          },

          {"mission_id", msg->mission_id},
          {"interceptor_id", msg->interceptor_id},
          {"target_id", msg->target_id},

          {"phase", msg->phase},

          {"distance_to_target", msg->distance_to_target},
          {"relative_speed", msg->relative_speed},
          {"mission_elapsed_sec", msg->mission_elapsed_sec},

          {"status_text", msg->status_text}
      };
  }

  inline json IntercepterMissionAckToJson(const cuas_msgs::msg::MissionAck::SharedPtr& msg)
  {
      return {
          {
              "stamp",
              {
                  {"sec", msg->stamp.sec},
                  {"nanosec", msg->stamp.nanosec}
              }
          },

          {"command_id", msg->command_id},
          {"mission_id", msg->mission_id},
          {"interceptor_id", msg->interceptor_id},

          {"accepted", msg->accepted},
          {"result_code", msg->result_code},

          {"message", msg->message}
      };
  }
}















