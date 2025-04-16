
#include "utils.h"

void set_to_pi(Float* flt) {
    // This is:
    // 3.141592653589793238462643383279502884197169399375
    // 10582097494459230781640628620899862803482534211706
    // 79821480865132823066470938446095505822317253594081
    // 28481117450284102701938521105559644622948954930381
    // 96442881097566593344612847564823378678316527120190
    // 91456485669234603486104543266482133936072602491412
    // 73724

    uint64_t exponent_bytes = 0x4000;
    // clang-format off
    uint64_t mantissa_bytes[16] = {
        0x7e0b31b4906c38ac, 0xb67b8400f97142c7, 0x3fcc9250cca3d9c8, 0x3f4b5d3e4822f896,
        0x6fdbdc70d7f6b513, 0xdad617feb96de80d, 0xfd8de89885d34c6f, 0x848bc90b6aecc4bc,
        0x286e9fc26adadaa3, 0x8636605614dbe4be, 0x09bbdf2a33679a74, 0x3644a29410f31c68,
        0x98e804177d4c7627, 0x39a252049c1114cf, 0x8469898cc51701b8, 0x0000921fb54442d1
    };
    // clang-format on

    float_set_sign(flt, false);
    float_set_exponent(flt, &exponent_bytes);
    float_set_mantissa(flt, &mantissa_bytes);
}

void set_to_e(Float* flt) {
    // This is:
    // 2.718281828459045235360287471352662497757247093699
    // 95957496696762772407663035354759457138217852516642
    // 74274663919320030599218174135966290435729003342952
    // 60595630738132328627943490763233829880753195251019
    // 01157383418793070215408914993488416750924476146066
    // 80822648001684774118537423454424371075390777449920
    // 695517

    uint64_t exponent_bytes = 0x4000;
    // clang-format off
    uint64_t mantissa_bytes[16] = {
        0xf2f578156305664a, 0xce6a6159949e9074, 0xd0e43be2b1426d5b, 0x16efc54d13b5e7df,
        0x926b309e18e1c1cd, 0x35e76aae26bcfeaf, 0xdda10ac6caaa7bda, 0xcac24867ea3ebe0c,
        0xc2f5a7be3dababfa, 0xebb1ed0364055d88, 0x7df2fa5fc6c6c618, 0x67f799273b9c4936,
        0x6d2b53c26c8228c8, 0x79e3b1738b079c5a, 0x95355fb8ac404e7a, 0x00005bf0a8b14576
    };
    // clang-format on

    float_set_sign(flt, false);
    float_set_exponent(flt, &exponent_bytes);
    float_set_mantissa(flt, &mantissa_bytes);
}

void set_to_pi_plus_e(Float* flt) {
    // This is:
    // 5.859874482048838473822930854632165381954416493075
    // 06539594191222003189303663975659319941700386728349
    // 54096144784452853665689112582061796258046256937033
    // 89076748188416431329882011868793474503702150181400
    // 97600264516359663560021762558311795429241003266257
    // 72279133670919377604641966720906505011463379941333
    // 43276568965661009835629338043974983153340901310478
    //      ^-- Probably, the last correct digit, but it's hard to verify...

    uint64_t exponent_bytes = 0x4001;
    // clang-format off
    uint64_t mantissa_bytes[16] = {
        0x388054e4f9b8cf7b, 0x4272f2ad4707e99e, 0x08586719bef32392, 0x2b1d9145adec703b,
        0x81238687786c3b70, 0x085ec1567015735e, 0xed9779afa83ee425, 0x27a708b9aa95c164,
        0x75b223c05443434f, 0xb8f426acbc70a123, 0x43d76cc4fd173046, 0xcf1e1ddda647b2cf,
        0x0309abecf4e74f77, 0x59c301bc138c5895, 0x0ccf74a2b8aba819, 0x000077082efac424
    };
    // clang-format on

    float_set_sign(flt, false);
    float_set_exponent(flt, &exponent_bytes);
    float_set_mantissa(flt, &mantissa_bytes);
}

void set_to_pi_times_e(Float* flt) {
    // This is:
    // 8.539734222673567065463550869546574495034888535765
    // 11496187960113017922861115733080757256386971047394
    // 39137749425116774676463211875906960239906183634537
    // 90704145420215994889633428527467000466877660930727
    // 11290393507480401055727040348627303998656540644166
    // 17922928571370821637441297616847117254467231842034
    // 07516851235476077547389943499045745266122675601721
    //      ^-- Probably, the last correct digit, but it's hard to verify...

    uint64_t exponent_bytes = 0x4002;
    // clang-format off
    uint64_t mantissa_bytes[16] = {
        0xf11c98307c9ff1d9, 0xa813f21130ab0611, 0x533120082da3a9ea, 0x62e48b7654464533,
        0x5b6d702effb27113, 0xe2390cc65fc234e6, 0x7a333ef0e9bb7eb4, 0x00a1c99697c3732e,
        0x00a10c767e44d9d4, 0x77a29af5a13abbd4, 0x074526e50a7bce26, 0xfab46375ca365a7f,
        0x669cf2bec94a3397, 0x70231e112e41cbc6, 0x49e6108579a2d0ca, 0x0000114580b45d47
    };
    // clang-format on

    float_set_sign(flt, false);
    float_set_exponent(flt, &exponent_bytes);
    float_set_mantissa(flt, &mantissa_bytes);
}

void set_to_pi_div_e(Float* flt) {
    // This is:
    // 1.155727349790921717910093183312696299120851023164
    // 41582049970653532728863184091693944018843423567355
    // 88044866536870207009142190480078563608344377886136
    // 04544109645138219699555760622688895095637708058521
    // 98635121805853707231355565921333354095807331307283
    // 22589947135972862258741687174951500204902423801143
    // 61210276222194259128255528725959244698699011316907
    //      ^-- Probably, the last correct digit, but it's hard to verify...

    uint64_t exponent_bytes = 0x3ff;
    // clang-format off
    uint64_t mantissa_bytes[16] = {
        0x7fb08af77817a025, 0xcf68e9fe65193aa7, 0x30451854ccb0b0c6, 0x25b33bbce68c7458,
        0x610ba0bc53fffa87, 0xe99fc67e92fc2b8b, 0x3465622b1c31243b, 0x2affe66274dcad16,
        0x7b7c88a47f80e626, 0xa55961c57af3a55c, 0x4a1a979153e750ee, 0x80ab6f6c07ec809f,
        0x7a227ab23c00ce8e, 0x22b56c115e82c7f5, 0xdefdc3b8933cc9f1, 0x000027ddbf6271db
    };
    // clang-format on

    float_set_sign(flt, false);
    float_set_exponent(flt, &exponent_bytes);
    float_set_mantissa(flt, &mantissa_bytes);
}

TEST(test_float1024_add, Suite::add()) {
    Float a = {}, b = {}, actual = {}, expected = {};

    if (float_init(&a, 15, 1008) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&b, 15, 1008) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&actual, 15, 1008) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&expected, 15, 1008) < 0)
        HANDLE_FAIL(float_init);

    set_to_pi(&a);
    set_to_e(&b);
    set_to_pi_plus_e(&expected);

    float_add(&actual, &a, &b);

    EXPECT(cmp_float(&actual, &expected)) << "𝜋 + e is wrong";

    float_destroy(&expected);
    float_destroy(&actual);
    float_destroy(&b);
    float_destroy(&a);
}

TEST(test_float1024_mul, Suite::mul()) {
    Float a = {}, b = {}, actual = {}, expected = {};

    if (float_init(&a, 15, 1008) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&b, 15, 1008) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&actual, 15, 1008) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&expected, 15, 1008) < 0)
        HANDLE_FAIL(float_init);

    set_to_pi(&a);
    set_to_e(&b);
    set_to_pi_times_e(&expected);

    float_mul(&actual, &a, &b);

    EXPECT(cmp_float(&actual, &expected)) << "𝜋 * e is wrong";

    float_destroy(&a);
    float_destroy(&b);
    float_destroy(&actual);
    float_destroy(&expected);
}

TEST(test_float1024_div, Suite::div()) {
    Float a = {}, b = {}, actual = {}, expected = {};

    if (float_init(&a, 15, 1008) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&b, 15, 1008) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&actual, 15, 1008) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&expected, 15, 1008) < 0)
        HANDLE_FAIL(float_init);

    set_to_pi(&a);
    set_to_e(&b);
    set_to_pi_div_e(&expected);

    float_div(&actual, &a, &b);

    EXPECT(cmp_float(&actual, &expected)) << "𝜋 / e is wrong";

    float_destroy(&expected);
    float_destroy(&actual);
    float_destroy(&b);
    float_destroy(&a);
}

TEST(test_float1024_parse, Suite::string()) {
    Float actual = {}, expected = {};

    if (float_init(&actual, 15, 1008) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&expected, 15, 1008) < 0)
        HANDLE_FAIL(float_init);

    const char* pi = "3.141592653589793238462643383279502884197169399375"
                     "10582097494459230781640628620899862803482534211706"
                     "79821480865132823066470938446095505822317253594081"
                     "28481117450284102701938521105559644622948954930381"
                     "96442881097566593344612847564823378678316527120190"
                     "91456485669234603486104543266482133936072602491412"
                     "73724587";

    set_to_pi(&expected);
    float_parse(&actual, pi);

    EXPECT(cmp_float(&actual, &expected)) << "𝜋 is parsed incorrectly";

    float_destroy(&actual);
    float_destroy(&expected);
}

TEST(test_float1024_to_string, Suite::string()) {
    Float actual = {}, expected = {};

    if (float_init(&actual, 15, 1008) < 0)
        HANDLE_FAIL(float_init);
    if (float_init(&expected, 15, 1008) < 0)
        HANDLE_FAIL(float_init);

    set_to_pi(&expected);
    int length = float_string(&expected, nullptr, 0);
    std::string str(length, '\0');
    float_string(&expected, str.data(), str.size());

    float_parse(&actual, str.data());

    EXPECT(cmp_float(&actual, &expected)) << "float_to_string fails for 𝜋";

    float_destroy(&actual);
    float_destroy(&expected);
}