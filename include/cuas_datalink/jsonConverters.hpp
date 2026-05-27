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

  inline void from_json(const json& j,cuas_msgs::msg::TargetTrack& msg)
  {
    if (j.contains("stamp")) {
      ParseStamp(j["stamp"], msg.stamp);
    }

    msg.target_id = j.value("targetId", "");

    msg.latitude = j.value("latitude", 0.0);
    msg.longitude = j.value("longitude", 0.0);
    msg.altitude = j.value("altitude", 0.0f);

    msg.velocity_x = j.value("velocityX", 0.0f);
    msg.velocity_y = j.value("velocityY", 0.0f);
    msg.velocity_z = j.value("velocityZ", 0.0f);

    msg.heading = j.value("heading", 0.0f);
    msg.confidence = j.value("confidence", 0.0f);

    msg.track_state =
      static_cast<uint8_t>(j.value("trackState", 0));
  }

  inline json to_json(const cuas_msgs::msg::TargetTrack& msg)
  {
    return {
      {"stamp", ToJsonStamp(msg.stamp)},

      {"targetId", msg.target_id},

      {"latitude", msg.latitude},
      {"longitude", msg.longitude},
      {"altitude", msg.altitude},

      {"velocityX", msg.velocity_x},
      {"velocityY", msg.velocity_y},
      {"velocityZ", msg.velocity_z},

      {"heading", msg.heading},
      {"confidence", msg.confidence},

      {"trackState", msg.track_state}
    };
  }
} // namespace target_track_json
