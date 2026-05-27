#pragma once

namespace cuas_datalink
{

namespace topics
{

// =========================================================
// C2 -> Interceptor
// =========================================================

constexpr const char * C2_COMMAND =
  "/cuas/c2/command";

constexpr const char * INTERCEPT_MISSION =
  "/cuas/c2/mission";

constexpr const char * TARGET_TRACK =
  "/cuas/c2/target_track";

// =========================================================
// Interceptor -> C2
// =========================================================

constexpr const char * INTERCEPTOR_STATUS =
  "/cuas/interceptor/status";

constexpr const char * INTERCEPT_PROGRESS =
  "/cuas/interceptor/progress";

constexpr const char * MISSION_ACK =
  "/cuas/interceptor/mission_ack";

constexpr const char * ENGAGEMENT_RESULT =
  "/cuas/interceptor/result";

constexpr const char * FAULT_REPORT =
  "/cuas/interceptor/fault";

// =========================================================
// Common
// =========================================================

constexpr const char * C2_STATUS =
  "/cuas/c2/status";

constexpr const char * HEARTBEAT =
  "/cuas/datalink/heartbeat";

}  // namespace topics

// =========================================================
// NATS Subjects
// =========================================================

namespace nats_subjects
{

// =========================================================
// C2 -> Interceptor
// =========================================================

constexpr const char * C2_COMMAND =
  "cuas.c2.command";

constexpr const char * INTERCEPT_MISSION =
  "cuas.c2.mission";

constexpr const char * TARGET_TRACK =
  "cuas.c2.target_track";

// =========================================================
// Interceptor -> C2
// =========================================================

constexpr const char * INTERCEPTOR_STATUS =
  "cuas.interceptor.status";

constexpr const char * INTERCEPT_PROGRESS =
  "cuas.interceptor.progress";

constexpr const char * MISSION_ACK =
  "cuas.interceptor.mission_ack";

constexpr const char * ENGAGEMENT_RESULT =
  "cuas.interceptor.result";

constexpr const char * FAULT_REPORT =
  "cuas.interceptor.fault";

// =========================================================
// Common
// =========================================================

constexpr const char * C2_STATUS =
  "cuas.c2.status";

constexpr const char * HEARTBEAT =
  "cuas.datalink.heartbeat";

}  // namespace nats_subjects

}  // namespace cuas_datalink