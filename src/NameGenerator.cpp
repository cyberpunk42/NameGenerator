#include <name_generator/NameGenerator.h>

#include <algorithm>
#include <iterator>
#include <random>

namespace namegen {

namespace {

struct SyllableSet {
    const char* const* starts;
    std::size_t startCount;
    const char* const* middles;
    std::size_t middleCount;
    const char* const* ends;
    std::size_t endCount;
};

constexpr const char* kStarts[] = {"Ar", "Bel", "Cor", "Dra", "El", "Fen", "Gal", "Har", "Ith", "Kal", "Lor", "Mor",
                                   "Nar", "Or", "Per", "Quel", "Ras", "Sil", "Tor", "Ul", "Val", "Wy", "Ys", "Zan"};
constexpr const char* kMiddles[] = {"a", "e", "i", "o", "u", "ae", "ia", "ou", "ar", "en", "il", "or", "an", "eth"};
constexpr const char* kEnds[] = {"n", "th", "r", "s", "l", "dor", "mir", "wen", "ras", "lin", "dan", "vel", "on", "is"};

// Phonetic inspiration: Tolkien (Quenya and Sindarin), Roach (English), Tranel (French), and
// Wiese (German). These sources inform broad sound patterns; the fragments below are original.
constexpr const char* kQuenyaStarts[] = {"A", "E", "I", "O", "U", "Pa", "Te", "Ca", "Fe", "Ha", "La", "Me", "Na",
                                         "Ra", "Sa", "Va", "Ya", "Qu", "Ty"};
constexpr const char* kQuenyaMiddles[] = {"a", "e", "i", "o", "u", "ai", "au", "oi", "ui", "ia", "ea", "ie"};
constexpr const char* kQuenyaEnds[] = {"n", "r", "l", "s", "t"};

constexpr const char* kSindarinStarts[] = {"A", "E", "G", "I", "O", "U", "B", "D", "F", "Gw", "Br", "Cr", "Dr", "Gl",
                                           "Gr", "Pr", "Thr", "Tr", "Dh", "Rh", "Ch", "Lh", "Ll", "M", "N", "R", "S", "Th", "V"};
constexpr const char* kSindarinMiddles[] = {"a", "e", "i", "o", "u", "ae", "ei", "ui", "au", "ia", "io", "oe"};
constexpr const char* kSindarinEnds[] = {"n", "r", "l", "s", "th", "d", "g", "nd", "ll"};

constexpr const char* kEnglishStarts[] = {"A", "B", "Br", "C", "Ch", "Cl", "D", "Dr", "E", "F", "Fl", "G", "Gr", "H",
                                          "J", "K", "L", "M", "N", "O", "P", "Pr", "R", "S", "Sh", "St", "T", "Th", "Tr", "W", "Y"};
constexpr const char* kEnglishMiddles[] = {"a", "e", "i", "o", "u", "ea", "ee", "ai", "ou", "ow", "er", "ar", "or", "en", "in", "el", "an"};
constexpr const char* kEnglishEnds[] = {"n", "r", "s", "t", "d", "m", "th", "ng", "ck", "nd", "st", "ld", "er", "en", "y"};

constexpr const char* kFrenchStarts[] = {"A", "B", "C", "Ch", "D", "E", "F", "G", "J", "L", "M", "N", "O", "P", "R", "S", "T",
                                         "V", "Z", "Br", "Cl", "Fl", "Gr", "Pr", "Tr"};
constexpr const char* kFrenchMiddles[] = {"a", "e", "i", "o", "u", "au", "eau", "oi", "ai", "ou", "eu", "ien", "an", "on", "ille"};
constexpr const char* kFrenchEnds[] = {"e", "i", "on", "an", "el", "elle", "et", "in", "ain", "eur", "ier", "eau", "aux", "ais"};

constexpr const char* kGermanStarts[] = {"A", "B", "Br", "D", "Dr", "E", "F", "Fr", "G", "Gr", "H", "K", "Kl", "Kr", "L", "M",
                                         "N", "O", "P", "R", "S", "Sch", "St", "T", "W", "Z"};
constexpr const char* kGermanMiddles[] = {"a", "e", "i", "o", "u", "ei", "ie", "au", "ae", "oe", "ue", "en", "er", "el", "an", "un"};
constexpr const char* kGermanEnds[] = {"en", "er", "el", "e", "au", "ei", "ich", "ig", "t", "r", "n", "nd", "st", "cht", "ung"};

constexpr const char* kOrcishStarts[] = {"Br", "Dr", "Gr", "Kr", "R", "Sk", "Sn", "Th", "Ur", "V", "Z", "G", "M", "Rag", "Gor"};
constexpr const char* kOrcishMiddles[] = {"a", "o", "u", "aa", "agh", "ar", "og", "uk", "urz", "rag"};
constexpr const char* kOrcishEnds[] = {"g", "k", "gash", "nak", "th", "mog", "rak", "zug", "ruk", "bash"};

constexpr const char* kGnomishStarts[] = {"B", "D", "F", "G", "K", "L", "M", "N", "P", "T", "W", "Wik", "Tink", "Pip"};
constexpr const char* kGnomishMiddles[] = {"i", "e", "a", "oo", "ibble", "indle", "ocket", "uggle", "in", "en"};
constexpr const char* kGnomishEnds[] = {"by", "kin", "wick", "bottle", "whistle", "wig", "nip", "dle", "bit", "well"};

constexpr const char* kInfernalStarts[] = {"A", "Ba", "Bel", "C", "D", "E", "Mal", "N", "R", "S", "V", "X", "Z", "Az", "Kor"};
constexpr const char* kInfernalMiddles[] = {"a", "e", "i", "o", "u", "aa", "ae", "ia", "or", "ur", "az", "oth"};
constexpr const char* kInfernalEnds[] = {"ax", "eth", "iel", "oth", "azar", "ion", "rax", "vex", "iel", "oth"};

constexpr const char* kAbyssalStarts[] = {"Ab", "Ch", "Dha", "Gha", "Kha", "N'", "Qh", "R'", "Sh", "Th", "Ulg", "Vho", "Xha", "Y'", "Zh"};
constexpr const char* kAbyssalMiddles[] = {"a", "o", "u", "aa", "augh", "ia", "orr", "ul", "uul", "yth"};
constexpr const char* kAbyssalEnds[] = {"ath", "ghul", "oth", "uun", "xoth", "yrr", "zhul", "'ak", "'oth", "ul"};

constexpr const char* kCthulhuStarts[] = {"Cth", "Dho", "G'", "Hast", "Ia", "Kth", "Nyar", "R'ly", "Shub", "Tsath", "Yog", "Y'gol", "Zoth"};
constexpr const char* kCthulhuMiddles[] = {"a", "aa", "ai", "au", "og", "oth", "ul", "ur", "yth", "'a", "'u"};
constexpr const char* kCthulhuEnds[] = {"ath", "g'n", "hoth", "loth", "oggua", "on", "oth", "ulhu", "yog", "yr"};

constexpr const char* kFaeStarts[] = {"Ael", "Bryn", "Cael", "Eira", "Fael", "Gwen", "Ily", "Leth", "Mae", "Nim", "Rhi", "Sio", "Tala", "Vael", "Wyn"};
constexpr const char* kFaeMiddles[] = {"a", "ae", "ai", "e", "ei", "ia", "ie", "il", "o", "ui", "y", "wyn"};
constexpr const char* kFaeEnds[] = {"a", "en", "eth", "ia", "iel", "in", "is", "ith", "ora", "wen"};

template <std::size_t StartCount, std::size_t MiddleCount, std::size_t EndCount>
SyllableSet makeSet(const char* const (&starts)[StartCount], const char* const (&middles)[MiddleCount],
                    const char* const (&ends)[EndCount])
{
    return {starts, StartCount, middles, MiddleCount, ends, EndCount};
}

SyllableSet syllablesFor(NameProfile profile)
{
    switch (profile) {
    case NameProfile::QuenyaInspired:
        return makeSet(kQuenyaStarts, kQuenyaMiddles, kQuenyaEnds);
    case NameProfile::SindarinInspired:
        return makeSet(kSindarinStarts, kSindarinMiddles, kSindarinEnds);
    case NameProfile::English:
        return makeSet(kEnglishStarts, kEnglishMiddles, kEnglishEnds);
    case NameProfile::French:
        return makeSet(kFrenchStarts, kFrenchMiddles, kFrenchEnds);
    case NameProfile::German:
        return makeSet(kGermanStarts, kGermanMiddles, kGermanEnds);
    case NameProfile::Orcish:
        return makeSet(kOrcishStarts, kOrcishMiddles, kOrcishEnds);
    case NameProfile::Gnomish:
        return makeSet(kGnomishStarts, kGnomishMiddles, kGnomishEnds);
    case NameProfile::Infernal:
        return makeSet(kInfernalStarts, kInfernalMiddles, kInfernalEnds);
    case NameProfile::Abyssal:
        return makeSet(kAbyssalStarts, kAbyssalMiddles, kAbyssalEnds);
    case NameProfile::CthulhuMythosInspired:
        return makeSet(kCthulhuStarts, kCthulhuMiddles, kCthulhuEnds);
    case NameProfile::FaeInspired:
        return makeSet(kFaeStarts, kFaeMiddles, kFaeEnds);
    case NameProfile::Generic:
    default:
        return makeSet(kStarts, kMiddles, kEnds);
    }
}

} // namespace

// Select a profile's syllables, seed the RNG from the seed/kind/profile, then assemble
// a start, optional middle, and ending. Retry endings that create an awkward join.
std::string generateName(std::uint64_t seed, NameKind kind, NameProfile profile)
{
    const SyllableSet set = syllablesFor(profile);
    const std::uint64_t profileSalt = std::uint64_t(profile) * 0xD1B54A32D192ED03ULL;
    std::mt19937_64 rng(seed * 0x9E3779B97F4A7C15ULL + 7 + std::uint64_t(kind) + profileSalt);
    std::string name = set.starts[rng() % set.startCount];
    if (rng() % 3 != 0)
        name += set.middles[rng() % set.middleCount];
    std::string end = set.ends[rng() % set.endCount];
    for (int attempt = 0; attempt < 16 && name.size() >= 2
                          && (name.ends_with(end.substr(0, std::min<std::size_t>(2, end.size()))) || name.back() == end.front());
         ++attempt)
        end = set.ends[rng() % set.endCount];
    return name + end;
}

} // namespace namegen
