#include <name_generator/NameGenerator.h>

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string_view>

namespace {

bool parseSeed(std::string_view value, std::uint64_t& seed)
{
    const auto result = std::from_chars(value.data(), value.data() + value.size(), seed);
    return result.ec == std::errc{} && result.ptr == value.data() + value.size();
}

bool parseCount(std::string_view value, std::size_t& count)
{
    unsigned int parsed = 0;
    const auto result = std::from_chars(value.data(), value.data() + value.size(), parsed);
    if (result.ec != std::errc{} || result.ptr != value.data() + value.size() || parsed == 0 || parsed > 24)
        return false;
    count = parsed;
    return true;
}

bool parseKind(std::string_view value, namegen::NameKind& kind)
{
    if (value == "generic")
        kind = namegen::NameKind::Generic;
    else if (value == "river")
        kind = namegen::NameKind::River;
    else if (value == "ocean-current")
        kind = namegen::NameKind::OceanCurrent;
    else if (value == "person")
        kind = namegen::NameKind::Person;
    else if (value == "mountain")
        kind = namegen::NameKind::Mountain;
    else if (value == "house")
        kind = namegen::NameKind::House;
    else if (value == "sword")
        kind = namegen::NameKind::Sword;
    else
        return false;
    return true;
}

bool parseProfile(std::string_view value, namegen::NameProfile& profile)
{
    if (value == "generic")
        profile = namegen::NameProfile::Generic;
    else if (value == "quenya-inspired")
        profile = namegen::NameProfile::QuenyaInspired;
    else if (value == "sindarin-inspired")
        profile = namegen::NameProfile::SindarinInspired;
    else if (value == "english")
        profile = namegen::NameProfile::English;
    else if (value == "french")
        profile = namegen::NameProfile::French;
    else if (value == "german")
        profile = namegen::NameProfile::German;
    else if (value == "orcish")
        profile = namegen::NameProfile::Orcish;
    else if (value == "gnomish")
        profile = namegen::NameProfile::Gnomish;
    else if (value == "infernal")
        profile = namegen::NameProfile::Infernal;
    else if (value == "abyssal")
        profile = namegen::NameProfile::Abyssal;
    else if (value == "cthulhu-mythos-inspired")
        profile = namegen::NameProfile::CthulhuMythosInspired;
    else if (value == "fae-inspired")
        profile = namegen::NameProfile::FaeInspired;
    else
        return false;
    return true;
}

} // namespace

int main(int argumentCount, char* arguments[])
{
    if (argumentCount != 5) {
        std::cerr << "Usage: name_generator_cli <seed> <kind> <profile> <count>\n";
        return 2;
    }

    std::uint64_t seed = 0;
    std::size_t count = 0;
    namegen::NameKind kind;
    namegen::NameProfile profile;
    if (!parseSeed(arguments[1], seed) || !parseKind(arguments[2], kind) || !parseProfile(arguments[3], profile)
        || !parseCount(arguments[4], count)) {
        std::cerr << "Invalid seed, kind, profile, or count.\n";
        return 2;
    }

    for (std::size_t index = 0; index < count; ++index)
        std::cout << namegen::generateName(seed + index, kind, profile) << '\n';
    return 0;
}