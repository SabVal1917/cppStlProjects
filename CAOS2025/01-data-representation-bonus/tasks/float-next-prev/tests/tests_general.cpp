
#include "utils.h"

TEST(test_construct, Suite::construct()) {
    srand(42);

    for (int i = 0; i < 10; i++) {
        int exponent_bits = (int)((unsigned)rand() % 20 + 4);
        int mantissa_bits = (int)((unsigned)rand() % 30 + 4);

        Float flt = {};
        if (float_init(&flt, exponent_bits, mantissa_bits) < 0)
            HANDLE_FAIL(float_init);

        int real_exponent_bits = float_get_exponent_bits(&flt);
        int real_mantissa_bits = float_get_mantissa_bits(&flt);

        if (EXPECT(exponent_bits == real_exponent_bits) << "Your Float returned wrong exponent length ("
                                                        << real_exponent_bits << " instead of " << exponent_bits << ")")
            return;
        if (EXPECT(mantissa_bits == real_mantissa_bits) << "Your Float returned wrong mantissa length ("
                                                        << real_mantissa_bits << " instead of " << mantissa_bits << ")")
            return;

        for (int j = 0; j < 10; j++) {
            uint64_t random_exponent = rand() % (1ll << float_get_exponent_bits(&flt));
            uint64_t random_mantissa = rand() % (1ll << float_get_mantissa_bits(&flt));
            int random_sign = rand() & 1;

            set_float(&flt, random_sign, random_exponent, random_mantissa);

            uint64_t real_exponent = 0, real_mantissa = 0;
            int real_sign = float_get_sign(&flt);

            float_get_exponent(&flt, &real_exponent);
            float_get_mantissa(&flt, &real_mantissa);

            if (EXPECT(real_exponent == random_exponent) << "Your Float does not preserve the exponent ("
                                                         << real_exponent << " instead of " << random_exponent << ")")
                return;
            if (EXPECT(real_mantissa == random_mantissa) << "Your Float does not preserve the exponent ("
                                                         << real_mantissa << " instead of " << random_mantissa << ")")
                return;
            if (EXPECT(real_sign == random_sign)
                << "Your Float does not preserve the sign (" << real_sign << " instead of " << random_sign << ")")
                return;
        }

        float_destroy(&flt);
    }
}

/*
 * Although I tried really hard to make reference solution as perfect
 * and bug-free as possible, I accept the possibility that there could
 * be some bugs in these tests. If you are sure that you've found a
 * mistake, you are welcome to present it to me at the next class and have
 * the entire task and get a full grade on this task.
 *
 * Sincerely yours, Artem.
 */

TEST(test_random_add, Suite::add()) {
    // clang-format off
    test_bin_op(float_add, 11, 37,
            "3838434510953184487662E120",    0, 0x5d5, 0x084a737484,
            "0.2831201012011156035277E-272", 0, 0x075, 0x1102e87f08,
            "3838434510953184487662E120",    0, 0x5d5, 0x084a737484);
    test_bin_op(float_add, 9, 10,
       "-0.184656560E-25", 1, 0x0a9, 0x1b7,
       "174023074E26",     0, 0x170, 0x2b4,
       "174023074E26",     0, 0x170, 0x2b4);
    test_bin_op(float_add, 8, 40,
        "-0.253508024642817047257019E-36", 1, 0x05, 0x590ea77410,
        "-474691916286918656",             1, 0xb9, 0xa59c721a02,
        "-474691916286918656",             1, 0xb9, 0xa59c721a02);
    test_bin_op(float_add, 12, 48,
        "9799357634133340551685061527E145", 0, 0xa3d, 0x95b6ccdb3043,
        "2420894986403378330319071331E282", 0, 0xc02, 0xaeef01301637,
        "2420894986403378330319071331E282", 0, 0xc02, 0xaeef01301637);
    test_bin_op(float_add, 10, 28,
        "-7.80671602487564086",  1, 0x201, 0xf3a13c4,
        "880688463433125712E31", 0, 0x2a1, 0x81a8976,
        "880688463433125712E31", 0, 0x2a1, 0x81a8976);
    test_bin_op(float_sub, 11, 7,
        "-0.5560345E-268", 1, 0x083, 0x6b,
        "7126102E273",     0, 0x7a0, 0x49,
        "-7126102E273",    1, 0x7a0, 0x49);
    // clang-format on
}

TEST(test_random_mul, Suite::mul()) {
    // clang-format off
    test_bin_op(float_mul, 10, 45,
        "-15086078007058546456748817E85", 1, 0x36d, 0x001e32708fc0,
        "81305847928955974136303866E41",  0, 0x2dd, 0x069a2dab5122,
        "-Inf",                           1, 0x3ff, 0x000000000000);
    test_bin_op(float_mul, 14, 30,
        "-0.2124439215539467680E-1106", 1, 0x11a2, 0x28e2ddd3,
        "1331104155957766501E37",       0, 0x20b6, 0x057ca41d,
        "-0.2827849868675893808E-1051", 1, 0x1259, 0x31e0dbaa);
    test_bin_op(float_mul, 12, 24,
        "0.6443559105687198E-285", 0, 0x44b, 0x887736,
        "-3116173018344646E288",   1, 0xbef, 0x22d233,
        "-2007924492512264E3",     1, 0x83b, 0xbdd94b);
    test_bin_op(float_mul, 10, 13,
        "8501228402E91",  0, 0x34e, 0x06de,
        "3384890475E143", 0, 0x3f9, 0x13b4,
        "Inf",            0, 0x3ff, 0x0000);
    test_bin_op(float_mul, 14, 38,
        "-0.66940866965398521169560E-1723", 1, 0x09a2, 0x2ace636e9a,
        "45055624864418507976396E372",      0, 0x251e, 0x0081b5a322,
        "-0.30160625900950476333417E-1328", 1, 0x0ec1, 0x2ba6da8ef5);
    test_bin_op(float_mul, 11, 25,
        "-0.7132930921540727E-37", 1, 0x383, 0x108b518,
        "6958311486267226E38",     0, 0x4b1, 0x1a1e57f,
        "-4963315532116787E1",     1, 0x436, 0x0c1545e);
    test_bin_op(float_mul, 6, 54,
        "0.3887121867438497052615886673265E-1", 0, 0x1a, 0x0f9bb6a7b27a9c,
        "130711.0184459594274812843650579",     0, 0x2f, 0x3fa5c12e37f61f,
        "5080.896581164456392798456363379",     0, 0x2b, 0x0f6396195f6e33);
    // clang-format on
}

TEST(test_random_div, Suite::div()) {
    // clang-format off
    test_bin_op(float_div, 12, 8,
            "-42254897E461", 1, 0xe13, 0xac,
            "-54102772E435", 1, 0xdbd, 0xa8,
            "77975715E18",   0, 0x855, 0x02);
    test_bin_op(float_div, 13, 63,
        "82122296249591485002883283168121463E660",  0, 0x1903, 0x2f90f6844ad61f0e,
        "-69492288423632122749488380866842360E99",  1, 0x11bb, 0x43cea04327ebb557,
        "-11817468975689149597971498677083927E527", 1, 0x1746, 0x65897280b3c73e52);
    test_bin_op(float_div, 5, 23,
        "0.304061949253082", 0, 0x0d, 0x1bae02,
        "0.369394779205322", 0, 0x0d, 0x3d2150,
        "0.823135495185852", 0, 0x0e, 0x52b902);
    test_bin_op(float_div, 7, 30,
        "-22891455696",               1, 0x61, 0x1546fe4d,
        "-0.1751527476659062165E-10", 1, 0x1b, 0x0d08723c,
        "Inf",                        0, 0x7f, 0x00000000);
    test_bin_op(float_div, 14, 13,
        "8887613149E1088",   0, 0x2e3e, 0x0796,
        "1119073895E1842",   0, 0x3804, 0x0128,
        "0.7942261167E-753", 0, 0x1639, 0x0635);
    test_bin_op(float_div, 8, 10,
        "-0.147757345E-29", 1, 0x1b, 0x37e,
        "283291716E29",     0, 0xfb, 0x154,
        "-0",               1, 0x00, 0x000);
    test_bin_op(float_div, 10, 63,
        "-0.95911144845062898493960006082030137E-31", 1, 0x197, 0x78ffe3b0e48e479e,
        "-0.34329739993619258045167443616120484E-27", 1, 0x1a3, 0x5997301700bbadeb,
        "0.27938208929892725281476776634514980E-3",   0, 0x1f3, 0x127a077e3651d850);
    // clang-format on
}