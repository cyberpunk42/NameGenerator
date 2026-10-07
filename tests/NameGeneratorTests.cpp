#include <name_generator/NameGenerator.h>

#include <array>
#include <iostream>
#include <string>

namespace {

bool check(bool condition, const char* description)
{
    if (!condition)
        std::cerr << "FAIL: " << description << '\n';
    return condition;
}

} // namespace

int main()
{
    using namespace namegen;

    bool passed = true;
    passed &= check(generateName(7) == generateName(7), "default profile is deterministic");
    passed &= check(generateName(7) != generateName(8), "different seeds produce different names");

    constexpr std::array profiles = {NameProfile::Generic, NameProfile::QuenyaInspired,
                                     NameProfile::SindarinInspired, NameProfile::English,
                                     NameProfile::French, NameProfile::German, NameProfile::Orcish,
                                     NameProfile::Gnomish, NameProfile::Infernal, NameProfile::Abyssal,
                                     NameProfile::CthulhuMythosInspired};
    const std::string genericName = generateName(7, NameKind::River, NameProfile::Generic);
    for (NameProfile profile : profiles) {
        const std::string name = generateName(7, NameKind::River, profile);
        passed &= check(!name.empty(), "profile produces a non-empty name");
        passed &= check(name == generateName(7, NameKind::River, profile), "profile is deterministic");
        passed &= check(profile == NameProfile::Generic || name != genericName, "profile changes the sound inventory");
    }
    passed &= check(generateName(7, NameKind::River) != generateName(7, NameKind::OceanCurrent),
                    "kind influences the generated name");

    return passed ? 0 : 1;
}
