#pragma once

#include "json_utility.hpp"

namespace krbn {
namespace core_configuration {
namespace details {
class complex_modifications_parameters final {
public:
  complex_modifications_parameters(void) : complex_modifications_parameters(nlohmann::json::object()) {
  }

  complex_modifications_parameters(const nlohmann::json& json) : json_(json),
                                                                 basic_simultaneous_threshold_milliseconds_(50),
                                                                 basic_to_if_alone_timeout_milliseconds_(1000),
                                                                 basic_to_if_held_down_threshold_milliseconds_(500),
                                                                 basic_to_delayed_action_delay_milliseconds_(500) {
    update(json);
  }

  nlohmann::json to_json(void) const {
    auto j = json_;
    for (const auto& pair : make_map()) {
      j[pair.first] = pair.second;
    }
    return j;
  }

  int get_basic_simultaneous_threshold_milliseconds(void) const {
    return basic_simultaneous_threshold_milliseconds_;
  }

  int get_basic_to_if_alone_timeout_milliseconds(void) const {
    return basic_to_if_alone_timeout_milliseconds_;
  }

  int get_basic_to_if_held_down_threshold_milliseconds(void) const {
    return basic_to_if_held_down_threshold_milliseconds_;
  }

  int get_basic_to_delayed_action_delay_milliseconds(void) const {
    return basic_to_delayed_action_delay_milliseconds_;
  }

  void update(const nlohmann::json& json) {
    for (const auto& pair : make_map()) {
      if (auto v = json_utility::find_optional<int>(json, pair.first)) {
        const_cast<int&>(pair.second) = *v;
      }
    }

    normalize();
  }

  boost::optional<int> get_value(const std::string& name) const {
    auto map = make_map();
    auto it = map.find(name);
    if (it != std::end(map)) {
      return it->second;
    }
    return boost::none;
  }

  void set_value(const std::string& name, int value) {
    auto map = make_map();
    auto it = map.find(name);
    if (it != std::end(map)) {
      const_cast<int&>(it->second) = value;
    }

    normalize();
  }

private:
  void normalize(void) {
    normalize(basic_simultaneous_threshold_milliseconds_, 0, 1000, "basic.simultaneous_threshold_milliseconds");
    normalize(basic_to_if_alone_timeout_milliseconds_, 0, boost::none, "basic.to_if_alone_timeout_milliseconds");
    normalize(basic_to_if_held_down_threshold_milliseconds_, 0, boost::none, "basic.to_if_held_down_threshold_milliseconds");
    normalize(basic_to_delayed_action_delay_milliseconds_, 0, boost::none, "basic.to_delayed_action_delay_milliseconds");
  }

  void normalize(int& value, boost::optional<int> min, boost::optional<int> max, const std::string& name) {
    if (min) {
      if (value < *min) {
        logger::get_logger().warn("{0} should be >= {1}.", name, *min);
      }
      value = std::max(value, *min);
    }

    if (max) {
      if (value > *max) {
        logger::get_logger().warn("{0} should be <= {1}.", name, *max);
      }
      value = std::min(value, *max);
    }
  }

  std::unordered_map<std::string, const int&> make_map(void) const {
    return {
        {"basic.simultaneous_threshold_milliseconds", basic_simultaneous_threshold_milliseconds_},
        {"basic.to_if_alone_timeout_milliseconds", basic_to_if_alone_timeout_milliseconds_},
        {"basic.to_if_held_down_threshold_milliseconds", basic_to_if_held_down_threshold_milliseconds_},
        {"basic.to_delayed_action_delay_milliseconds", basic_to_delayed_action_delay_milliseconds_},
    };
  }

  nlohmann::json json_;
  int basic_simultaneous_threshold_milliseconds_;
  int basic_to_if_alone_timeout_milliseconds_;
  int basic_to_if_held_down_threshold_milliseconds_;
  int basic_to_delayed_action_delay_milliseconds_;
};

inline void to_json(nlohmann::json& json, const complex_modifications_parameters& parameters) {
  json = parameters.to_json();
}
} // namespace details
} // namespace core_configuration
} // namespace krbn
