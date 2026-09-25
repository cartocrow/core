#include "geometric_feature.h"

namespace cartocrow {
double to_double(const GeometryAttribute& v) {
    return std::visit(overloaded{
        [](int x) -> double {
            return static_cast<double>(x);
        },
        [](int64_t x) -> double {
            return static_cast<double>(x);
        },
        [](double x) -> double {
            return x;
        },
        [](auto const&) -> double {
            throw std::runtime_error("Variant does not hold a convertible type");
        }
        }, v);
}

bool convertible_to_double(const GeometryAttribute& v) {
    return std::holds_alternative<double>(v) || std::holds_alternative<int>(v) || std::holds_alternative<int64_t>(v);
}

int to_int(const GeometryAttribute& v) {
    return std::visit(overloaded{
        [](int x) -> int {
            return static_cast<int>(x);
        },
        [](int64_t x) -> int {
            return static_cast<int>(x);
        },
        [](auto const&) -> int {
            throw std::runtime_error("Variant does not hold a convertible type");
        }
        }, v);
}

bool convertible_to_int(const GeometryAttribute& v) {
    return std::holds_alternative<int>(v) || std::holds_alternative<int64_t>(v);
}
}