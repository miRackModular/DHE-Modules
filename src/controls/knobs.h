#pragma once

#include "components/meta.h"
#include "params/mapped-knob-quantity.h"
#include "params/ranged-knob-quantity.h"
#include "widgets/dimensions.h"
#include "widgets/knob-widget.h"

#include "rack.hpp"

#include <string>
#include <type_traits>

namespace dhe {

struct Large {
  static auto constexpr svg_file = "knob-large";
};

struct Medium {
  static auto constexpr svg_file = "knob-medium";
};

struct Small {
  static auto constexpr svg_file = "knob-small";
};

struct Tiny {
  static auto constexpr svg_file = "knob-tiny";
};

struct Knob {
  template <typename P, typename S, typename V> struct Config {
    using ValueType = V;
    static auto constexpr svg_dir = P::svg_dir;
    static auto constexpr svg_file = S::svg_file;
  };

  template <typename P, typename S, typename V>
  using Widget = KnobWidget<Config<P, S, V>>;

  template <typename, typename = void>
  struct DefinesIntRange : std::false_type {};

  template <typename T>
  struct DefinesIntRange<T, void_t<decltype(T::min), decltype(T::max)>>
      : std::is_integral<decltype(T::min)> {};

  template <typename, typename = void> struct HasKnobMap : std::false_type {};

  template <typename T>
  struct HasKnobMap<T, void_t<typename T::KnobMap>> : std::true_type {};

  template <typename S, typename V = float, typename P>
  static inline auto install(P *panel, int id, float xmm, float ymm)
      -> KnobWidget<Config<P, S, V>> * {
    auto *widget = rack::createParamCentered<Widget<P, S, V>>(
        mm2px(xmm, ymm), panel->getModule(), id);
    widget->snap = std::is_integral<V>::value;
    panel->addParam(widget);
    return widget;
  }

  // Configure a knob with the int range specified by T
  template <typename T>
  static inline auto config(rack::engine::Module *module, int id,
                            std::string const &name, int value)
      -> enable_if_t<DefinesIntRange<T>::value, RangedKnobQuantity<int> *> {
    auto const min = static_cast<float>(T::min);
    auto const max = static_cast<float>(T::max);
    auto const default_value = static_cast<float>(value);
    auto *q = module->configParam<RangedKnobQuantity<int>>(
        id, min, max, default_value, name, T::unit);
    q->snapEnabled = true;
    return q;
  }

  // Configure a knob with display values mapped by T
  template <typename T>
  static inline auto config(rack::engine::Module *module, int id,
                            std::string const &name,
                            float value = T::KnobMap::default_value)
      -> enable_if_t<HasKnobMap<T>::value, MappedKnobQuantity<T> *> {
    auto const mapper = typename T::KnobMap{};
    auto const default_rotation = mapper.to_value(value);
    return module->configParam<MappedKnobQuantity<T>>(
        id, 0.F, 1.F, default_rotation, name, T::KnobMap::unit);
  }
};
} // namespace dhe
