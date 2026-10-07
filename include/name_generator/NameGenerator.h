#pragma once

#include <cstdint>
#include <string>

namespace namegen {

// Caller-defined use of a generated name. The enum is part of the reusable API and can be
// extended as the library grows.
enum class NameKind : std::uint8_t {
    Generic,
    River,
    OceanCurrent,
    Person,
    Mountain,
    House,
    Sword,
};

enum class NameProfile : std::uint8_t {
    Generic,
    QuenyaInspired,
    SindarinInspired,
    English,
    French,
    German,
    Orcish,
    Gnomish,
    Infernal,
    Abyssal,
    CthulhuMythosInspired,
    FaeInspired,
};

// Names are deterministic for a seed, kind, and profile.
std::string generateName(std::uint64_t seed, NameKind kind = NameKind::Generic,
                         NameProfile profile = NameProfile::Generic);

} // namespace namegen
