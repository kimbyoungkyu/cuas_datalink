#pragma once

#include <rclcpp/rclcpp.hpp>

namespace cuas_datalink
{

inline rclcpp::QoS ReliableControlQoS()
{
  return rclcpp::QoS(rclcpp::KeepLast(50)).reliable().durability_volatile();
}

inline rclcpp::QoS BestEffortTelemetryQoS()
{
  return rclcpp::QoS(rclcpp::KeepLast(100)).best_effort().durability_volatile();
}

}  // namespace cuas_datalink
