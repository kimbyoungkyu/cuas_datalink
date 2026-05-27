#pragma once

namespace cuas_datalink
{
namespace topics
{
constexpr const char * C2_COMMAND = "/cuas/c2/command";
constexpr const char * INTERCEPTOR_COMMAND = "/cuas/interceptor/command";
constexpr const char * INTERCEPTOR_STATUS = "/cuas/interceptor/status";
constexpr const char * C2_STATUS = "/cuas/c2/status";
constexpr const char * HEARTBEAT = "/cuas/datalink/heartbeat";
}  // namespace topics

namespace nats_subjects
{
constexpr const char * C2_COMMAND = "cuas.c2.command";
constexpr const char * C2_STATUS = "cuas.c2.status";
constexpr const char * INTERCEPTOR_COMMAND = "cuas.interceptor.command";
constexpr const char * INTERCEPTOR_STATUS = "cuas.interceptor.status";
constexpr const char * HEARTBEAT = "cuas.datalink.heartbeat";
}  // namespace nats_subjects
}  // namespace cuas_datalink
