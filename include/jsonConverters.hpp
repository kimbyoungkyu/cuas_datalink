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
  inline uint8_t ParseCommandType(const json& j)
  {
    if (!j.contains("commandType") || j["commandType"].is_null())
    {
      return 0;
    }

    const auto& v = j["commandType"];

    if (v.is_number_integer())
    {
      return static_cast<uint8_t>(v.get<int>());
    }

    if (v.is_string())
    {
      const std::string s = v.get<std::string>();

      if (s == "AssignTarget")       return cuas_msgs::msg::C2Command::ASSIGN_TARGET;
      if (s == "PrepareIntercept")   return cuas_msgs::msg::C2Command::PREPARE_INTERCEPT;
      if (s == "AuthorizeLaunch")    return cuas_msgs::msg::C2Command::AUTHORIZE_LAUNCH;
      if (s == "StartIntercept")     return cuas_msgs::msg::C2Command::START_INTERCEPT;
      if (s == "UpdateTarget")       return cuas_msgs::msg::C2Command::UPDATE_TARGET;
      if (s == "UpdateMission")      return cuas_msgs::msg::C2Command::UPDATE_MISSION;
      if (s == "Hold")               return cuas_msgs::msg::C2Command::HOLD;
      if (s == "Abort")              return cuas_msgs::msg::C2Command::ABORT;
      if (s == "ReturnHome")         return cuas_msgs::msg::C2Command::RETURN_HOME;
      if (s == "Land")               return cuas_msgs::msg::C2Command::LAND;
    }

    return 0;
  }

  inline void from_json(
    const json& j,
    cuas_msgs::msg::C2Command& msg)
  {
    if (j.contains("stamp") && j["stamp"].is_object())
    {
      msg.stamp.sec = j["stamp"].value("sec", 0);
      msg.stamp.nanosec = j["stamp"].value("nanosec", 0u);
    }

    msg.command_id = j.value("commandId", "");
    msg.mission_id = j.value("missionId", "");
    msg.interceptor_id = j.value("interceptorId", "");
    msg.target_id = j.value("targetId", "");

    msg.command_type = ParseCommandType(j);

    msg.reason = j.value("reason", "");
  }
}


namespace intercept_mission_json
{
  inline void ParseStamp(
    const json& j,
    builtin_interfaces::msg::Time& stamp)
  {
    if (!j.is_object())
    {
      stamp.sec = 0;
      stamp.nanosec = 0;
      return;
    }

    stamp.sec = j.value("sec", 0);
    stamp.nanosec = j.value("nanosec", 0u);
  }

  inline uint8_t ParseC2CommandType(const json& j)
  {
    if (!j.contains("commandType") || j["commandType"].is_null())
      return 0;

    const auto& v = j["commandType"];

    if (v.is_number_integer())
      return static_cast<uint8_t>(v.get<int>());

    if (v.is_string())
    {
      const std::string s = v.get<std::string>();

      if (s == "AssignTarget") return cuas_msgs::msg::C2Command::ASSIGN_TARGET;
      if (s == "PrepareIntercept") return cuas_msgs::msg::C2Command::PREPARE_INTERCEPT;
      if (s == "AuthorizeLaunch") return cuas_msgs::msg::C2Command::AUTHORIZE_LAUNCH;
      if (s == "StartIntercept") return cuas_msgs::msg::C2Command::START_INTERCEPT;
      if (s == "UpdateTarget") return cuas_msgs::msg::C2Command::UPDATE_TARGET;
      if (s == "UpdateMission") return cuas_msgs::msg::C2Command::UPDATE_MISSION;
      if (s == "Hold") return cuas_msgs::msg::C2Command::HOLD;
      if (s == "Abort") return cuas_msgs::msg::C2Command::ABORT;
      if (s == "ReturnHome") return cuas_msgs::msg::C2Command::RETURN_HOME;
      if (s == "Land") return cuas_msgs::msg::C2Command::LAND;
    }

    return 0;
  }

  inline uint8_t ParseTrackState(const json& j)
  {
    if (!j.contains("trackState") || j["trackState"].is_null())
      return cuas_msgs::msg::TargetTrack::UNKNOWN;

    const auto& v = j["trackState"];

    if (v.is_number_integer())
      return static_cast<uint8_t>(v.get<int>());

    if (v.is_string())
    {
      const std::string s = v.get<std::string>();

      if (s == "Unknown") return cuas_msgs::msg::TargetTrack::UNKNOWN;
      if (s == "Detected") return cuas_msgs::msg::TargetTrack::DETECTED;
      if (s == "Tracking") return cuas_msgs::msg::TargetTrack::TRACKING;
      if (s == "Lost") return cuas_msgs::msg::TargetTrack::LOST;
      if (s == "Confirmed") return cuas_msgs::msg::TargetTrack::CONFIRMED;
    }

    return cuas_msgs::msg::TargetTrack::UNKNOWN;
  }

  inline void FromJson(
    const json& j,
    cuas_msgs::msg::C2Command& msg)
  {
    if (!j.is_object())
      return;

    if (j.contains("stamp"))
      ParseStamp(j["stamp"], msg.stamp);

    msg.command_id = j.value("commandId", "");
    msg.mission_id = j.value("missionId", "");
    msg.interceptor_id = j.value("interceptorId", "");
    msg.target_id = j.value("targetId", "");

    msg.command_type = ParseC2CommandType(j);

    msg.reason = j.value("reason", "");
  }

  inline void FromJson(
    const json& j,
    cuas_msgs::msg::TargetTrack& msg)
  {
    if (!j.is_object())
      return;

    if (j.contains("stamp"))
      ParseStamp(j["stamp"], msg.stamp);

    msg.target_id = j.value("targetId", "");

    msg.latitude = j.value("latitude", 0.0);
    msg.longitude = j.value("longitude", 0.0);
    msg.altitude = j.value("altitude", 0.0f);

    msg.velocity_x = j.value("velocityX", 0.0f);
    msg.velocity_y = j.value("velocityY", 0.0f);
    msg.velocity_z = j.value("velocityZ", 0.0f);

    msg.heading = j.value("heading", 0.0f);
    msg.confidence = j.value("confidence", 0.0f);

    msg.track_state = ParseTrackState(j);
  }

  inline void FromJson(
    const json& j,
    cuas_msgs::msg::InterceptMission& msg)
  {
    if (!j.is_object())
      return;

    if (j.contains("stamp"))
      ParseStamp(j["stamp"], msg.stamp);

    msg.mission_id = j.value("missionId", "");
    msg.interceptor_id = j.value("interceptorId", "");
    msg.target_id = j.value("targetId", "");

    if (j.contains("target") && j["target"].is_object())
      FromJson(j["target"], msg.target);

    msg.max_speed = j.value("maxSpeed", 0.0f);
    msg.safe_altitude = j.value("safeAltitude", 0.0f);
    msg.loiter_altitude = j.value("loiterAltitude", 0.0f);

    msg.launch_latitude = j.value("launchLatitude", 0.0);
    msg.launch_longitude = j.value("launchLongitude", 0.0);
    msg.launch_altitude = j.value("launchAltitude", 0.0f);

    msg.home_latitude = j.value("homeLatitude", 0.0);
    msg.home_longitude = j.value("homeLongitude", 0.0);
    msg.home_altitude = j.value("homeAltitude", 0.0f);

    msg.allow_terminal_phase = j.value("allowTerminalPhase", false);
    msg.allow_auto_return = j.value("allowAutoReturn", false);
    msg.allow_abort_on_lost_target = j.value("allowAbortOnLostTarget", false);
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
      if (s == "Detected")   return 1;
      if (s == "Tracking")   return 2;
      if (s == "Lost")       return 3;
      if (s == "Confirmed")  return 4;
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
      case 0: return "Unknown";
      case 1: return "SuccessSim";
      case 2: return "MissedSim";
      case 3: return "Aborted";
      case 4: return "TargetLost";
      case 5: return "SystemFault";
      case 6: return "SafetyAbort";
      default: return "Invalid";
    }
  }

  inline std::string SeverityToString(uint8_t severity)
  {
    switch (severity)
    {
      case 0: return "Info";
      case 1: return "Warning";
      case 2: return "Error";
      case 3: return "Critical";
      default: return "Unknown";
    }
  }

  inline std::string VehicleStateToString(uint8_t state)
  {
    switch (state)
    {
      case 0: return "Idle";
      case 1: return "Ready";
      case 2: return "Armed";
      case 3: return "Active";
      case 4: return "Holding";
      case 5: return "Returning";
      case 6: return "Landed";
      case 7: return "Fault";
      default: return "Unknown";
    }
  }

  inline std::string PhaseToString(uint8_t phase)
  {
    switch (phase)
    {
      case 0: return "None";
      case 1: return "Assigned";
      case 2: return "Preparing";
      case 3: return "Launched";
      case 4: return "Midcourse";
      case 5: return "CooperativeTracking";
      case 6: return "TerminalApproachSim";
      case 7: return "Completed";
      case 8: return "Aborted";
      case 9: return "Failed";
      default: return "Unknown";
    }
  }

  inline std::string CommandResultCodeToString(uint8_t code)
  {
    switch (code)
    {
      case 0: return "OK";
      case 1: return "Rejected";
      case 2: return "Busy";
      case 3: return "InvalidTarget";
      case 4: return "NotReady";
      case 5: return "SafetyBlocked";
      case 6: return "InternalError";
      default: return "Unknown";
    }
  }

  inline json StampToJson(const builtin_interfaces::msg::Time& stamp)
  {
    return {
      {"sec", stamp.sec},
      {"nanosec", stamp.nanosec}
    };
  }

  inline json EngagementResultToJson(
    const cuas_msgs::msg::EngagementResult::SharedPtr& msg)
  {
    return {
      {"stamp", StampToJson(msg->stamp)},

      {"missionId", msg->mission_id},
      {"interceptorId", msg->interceptor_id},
      {"targetId", msg->target_id},

      {"result", msg->result},
      {"resultText", ResultToString(msg->result)},

      {"finalDistance", msg->final_distance},
      {"engagementTimeSec", msg->engagement_time_sec},

      {"summary", msg->summary}
    };
  }

  inline json FaultReportToJson(
    const cuas_msgs::msg::FaultReport::SharedPtr& msg)
  {
    return {
      {"stamp", StampToJson(msg->stamp)},

      {"interceptorId", msg->interceptor_id},
      {"missionId", msg->mission_id},

      {"severity", msg->severity},
      {"severityText", SeverityToString(msg->severity)},

      {"faultCode", msg->fault_code},
      {"faultName", msg->fault_name},
      {"description", msg->description},

      {"requiresAbort", msg->requires_abort}
    };
  }

  inline json InterceptorStatusToJson(
    const cuas_msgs::msg::InterceptorStatus::SharedPtr& msg)
  {
    return {
      {"stamp", StampToJson(msg->stamp)},

      {"interceptorId", msg->interceptor_id},
      {"missionId", msg->mission_id},

      {"vehicleState", msg->vehicle_state},
      {"vehicleStateText", VehicleStateToString(msg->vehicle_state)},

      {"latitude", msg->latitude},
      {"longitude", msg->longitude},
      {"altitude", msg->altitude},

      {"velocityX", msg->velocity_x},
      {"velocityY", msg->velocity_y},
      {"velocityZ", msg->velocity_z},

      {"batteryRemaining", msg->battery_remaining},

      {"armed", msg->armed},
      {"offboardEnabled", msg->offboard_enabled},
      {"healthy", msg->healthy}
    };
  }

  inline json InterceptorMissionStatusToJson(
    const cuas_msgs::msg::InterceptProgress::SharedPtr& msg)
  {
    return {
      {"stamp", StampToJson(msg->stamp)},

      {"missionId", msg->mission_id},
      {"interceptorId", msg->interceptor_id},
      {"targetId", msg->target_id},

      {"phase", msg->phase},
      {"phaseText", PhaseToString(msg->phase)},

      {"distanceToTarget", msg->distance_to_target},
      {"relativeSpeed", msg->relative_speed},
      {"missionElapsedSec", msg->mission_elapsed_sec},

      {"statusText", msg->status_text}
    };
  }

  inline json IntercepterMissionAckToJson(
    const cuas_msgs::msg::MissionAck::SharedPtr& msg)
  {
    return {
      {"stamp", StampToJson(msg->stamp)},

      {"commandId", msg->command_id},
      {"missionId", msg->mission_id},
      {"interceptorId", msg->interceptor_id},

      {"accepted", msg->accepted},
      {"resultCode", msg->result_code},
      {"resultCodeText", CommandResultCodeToString(msg->result_code)},

      {"message", msg->message}
    };
  }
}















