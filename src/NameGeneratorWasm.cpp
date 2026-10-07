#include <name_generator/NameGenerator.h>

#include <emscripten/bind.h>

#include <charconv>
#include <cstdint>
#include <string>

namespace {

std::string generateNameForWeb(const std::string& seedText, int kindValue, int profileValue)
{
    std::uint64_t seed = 0;
    const auto parsed = std::from_chars(seedText.data(), seedText.data() + seedText.size(), seed);
    if (parsed.ec != std::errc{} || parsed.ptr != seedText.data() + seedText.size()
        || kindValue < static_cast<int>(namegen::NameKind::Generic)
        || kindValue > static_cast<int>(namegen::NameKind::Sword)
        || profileValue < static_cast<int>(namegen::NameProfile::Generic)
        || profileValue > static_cast<int>(namegen::NameProfile::FaeInspired))
        return {};

    return namegen::generateName(seed, static_cast<namegen::NameKind>(kindValue),
                                 static_cast<namegen::NameProfile>(profileValue));
}

} // namespace

EMSCRIPTEN_BINDINGS(name_generator)
{
    emscripten::function("generateName", &generateNameForWeb);
}