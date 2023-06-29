#include <gtest/gtest.h>
#include <iostream>

#include <alt_bn128.hpp>
#include "../src/ntt_bn128.hpp"
#include <immintrin.h>

#define FFT_SIZE (1 << 4)
#define NUM_REPS 5
#define BLOWUP_FACTOR 1
#define NUM_COLUMNS 8
#define NPHASES 4

AltBn128::Engine E;

TEST(BN128_TEST, ntt)
{
    AltBn128::FrElement *a = (AltBn128::FrElement *)malloc(FFT_SIZE * sizeof(AltBn128::FrElement));
    AltBn128::FrElement *initial = (AltBn128::FrElement *)malloc(FFT_SIZE * sizeof(AltBn128::FrElement));
    NTT_AltBn128 bn128ntt(E, FFT_SIZE);

    a[0] = E.fr.one();
    a[1] = E.fr.one();
    for (uint64_t i = 2; i < FFT_SIZE; i++)
    {
        a[i] = E.fr.mul(a[i - 1], a[i - 2]);
    }

    memcpy(initial, a, FFT_SIZE * sizeof(AltBn128::FrElement));

    for (int i = 0; i < NUM_REPS; i++)
    {
        bn128ntt.NTT(a, a, FFT_SIZE);
        bn128ntt.INTT(a, a, FFT_SIZE);
    }

    for (int i = 0; i < FFT_SIZE; i++)
    {
        ASSERT_TRUE(E.fr.eq(a[i], initial[i]));
    }
    free(a);
    free(initial);
}

TEST(BN128_TEST, ntt_block)
{
    AltBn128::FrElement *a = (AltBn128::FrElement *)malloc(FFT_SIZE * NUM_COLUMNS * sizeof(AltBn128::FrElement));
    AltBn128::FrElement *initial = (AltBn128::FrElement *)malloc(FFT_SIZE * NUM_COLUMNS * sizeof(AltBn128::FrElement));
    NTT_AltBn128 bn128ntt(E, FFT_SIZE);

    for (uint i = 0; i < 2; i++)
    {
        for (uint j = 0; j < NUM_COLUMNS; j++)
        {
            E.fr.add(a[i * NUM_COLUMNS + j], E.fr.one(), E.fr.set(j));
        }
    }

    for (uint64_t i = 2; i < FFT_SIZE; i++)
    {
        for (uint j = 0; j < NUM_COLUMNS; j++)
        {
            a[i * NUM_COLUMNS + j] = E.fr.add(a[NUM_COLUMNS * (i - 1) + j], a[NUM_COLUMNS * (i - 2) + j]);
        }
    }

    memcpy(initial, a, FFT_SIZE * NUM_COLUMNS * sizeof(AltBn128::FrElement));

    // Option 1: dst is a NULL pointer
    for (int i = 0; i < NUM_REPS; i++)
    {
        bn128ntt.NTT(NULL, a, FFT_SIZE, NUM_COLUMNS);
        bn128ntt.INTT(NULL, a, FFT_SIZE, NUM_COLUMNS);
    }

    for (int i = 0; i < FFT_SIZE * NUM_COLUMNS; i++)
    {
        ASSERT_TRUE(E.fr.eq(a[i], initial[i]));
    }

    // Option 2: dst = src
    for (int i = 0; i < NUM_REPS; i++)
    {
        bn128ntt.NTT(a, a, FFT_SIZE, NUM_COLUMNS);
        bn128ntt.INTT(a, a, FFT_SIZE, NUM_COLUMNS);
    }

    for (int i = 0; i < FFT_SIZE * NUM_COLUMNS; i++)
    {
        ASSERT_TRUE(E.fr.eq(a[i], initial[i]));
    }

    // Option 3: dst != src
    AltBn128::FrElement *dst = (AltBn128::FrElement *)malloc(FFT_SIZE * NUM_COLUMNS * sizeof(AltBn128::FrElement));
    for (int i = 0; i < NUM_REPS; i++)
    {
        bn128ntt.NTT(dst, a, FFT_SIZE, NUM_COLUMNS);
        for (uint64_t k = 0; k < FFT_SIZE * NUM_COLUMNS; ++k)
            a[k] = E.fr.zero();
        bn128ntt.INTT(a, dst, FFT_SIZE, NUM_COLUMNS);
    }

    for (int i = 0; i < FFT_SIZE * NUM_COLUMNS; i++)
    {
        ASSERT_TRUE(E.fr.eq(a[i], initial[i]));
    }

    // Option 4: different configurations of phases and blocks
    for (int i = 0; i < NUM_REPS; i++)
    {
        bn128ntt.NTT(a, a, FFT_SIZE, NUM_COLUMNS, NULL, 3, 5);
        bn128ntt.INTT(a, a, FFT_SIZE, NUM_COLUMNS, NULL, 4, 3);
    }

    for (int i = 0; i < FFT_SIZE * NUM_COLUMNS; i++)
    {
        ASSERT_TRUE(E.fr.eq(a[i], initial[i]));
    }

    // Option 5: out of range parameters
    for (int i = 0; i < NUM_REPS; i++)
    {
        bn128ntt.NTT(a, a, FFT_SIZE, NUM_COLUMNS, NULL, 3, 3000);
        bn128ntt.INTT(a, a, FFT_SIZE, NUM_COLUMNS, NULL, 4, -1);
    }

    for (int i = 0; i < FFT_SIZE * NUM_COLUMNS; i++)
    {
        ASSERT_TRUE(E.fr.eq(a[i], initial[i]));
    }
    free(a);
    free(initial);

    // Edge case:Try to call ntt with FFT_SIZE = 1 ncols=3
    uint64_t fft_size = 1;
    uint64_t ncols = 3;
    AltBn128::FrElement a1[3] = {E.fr.set(1), E.fr.set(2), E.fr.set(3)};
    AltBn128::FrElement b1[3];

    bn128ntt.NTT(b1, a1, fft_size, ncols);
    bn128ntt.INTT(a1, b1, fft_size, ncols);
    ASSERT_TRUE(E.fr.eq(a1[0], E.fr.set(1)));
    ASSERT_TRUE(E.fr.eq(a1[1], E.fr.set(2)));
    ASSERT_TRUE(E.fr.eq(a1[2], E.fr.set(3)));

    // Edge case:Try to call ntt with FFT_SIZE = 2 ncols=3
    fft_size = 2;
    ncols = 3;
    AltBn128::FrElement a2[6] = {E.fr.set(1), E.fr.set(2), E.fr.set(3), E.fr.set(4), E.fr.set(5), E.fr.set(6)};
    AltBn128::FrElement b2[6];

    bn128ntt.NTT(b2, a2, fft_size, ncols);
    bn128ntt.INTT(a2, b2, fft_size, ncols);

    ASSERT_TRUE(E.fr.eq(a2[0], E.fr.set(1)));
    ASSERT_TRUE(E.fr.eq(a2[1], E.fr.set(2)));
    ASSERT_TRUE(E.fr.eq(a2[2], E.fr.set(3)));
    ASSERT_TRUE(E.fr.eq(a2[3], E.fr.set(4)));
    ASSERT_TRUE(E.fr.eq(a2[4], E.fr.set(5)));
    ASSERT_TRUE(E.fr.eq(a2[5], E.fr.set(6)));

    // Edge case: It does not crash with size==0 or ncols==0
    fft_size = 0;
    ncols = 3;
    bn128ntt.NTT(b2, a2, fft_size, ncols);
    bn128ntt.INTT(a2, b2, fft_size, ncols);
    fft_size = 1;
    ncols = 0;
    bn128ntt.NTT(b2, a2, fft_size, ncols);
    bn128ntt.INTT(a2, b2, fft_size, ncols);
}
TEST(BN128_TEST, LDE)
{
    AltBn128::FrElement *a = (AltBn128::FrElement *)malloc((FFT_SIZE << BLOWUP_FACTOR) * sizeof(AltBn128::FrElement));
    NTT_AltBn128 bn128ntt(E, FFT_SIZE);
    NTT_AltBn128 gntt_extension(E, FFT_SIZE << BLOWUP_FACTOR);

    AltBn128::FrElement *zeros_array = (AltBn128::FrElement *)malloc(((FFT_SIZE << BLOWUP_FACTOR) - FFT_SIZE) * sizeof(AltBn128::FrElement));
#pragma omp parallel for
    for (uint i = 0; i < ((FFT_SIZE << BLOWUP_FACTOR) - FFT_SIZE); i++)
    {
        zeros_array[i] = E.fr.zero();
    }

    a[0] = E.fr.one();
    a[1] = E.fr.one();
    for (uint64_t i = 2; i < FFT_SIZE; i++)
    {
        a[i] = E.fr.add(a[i - 1], a[i - 2]);
    }

    AltBn128::FrElement shift = E.fr.set(25); // TODO: ask for this number, where to put it how to calculate it
    bn128ntt.INTT(a, a, FFT_SIZE);

    // TODO: This can be pre-generated
    AltBn128::FrElement *r = (AltBn128::FrElement *)malloc(FFT_SIZE * sizeof(AltBn128::FrElement));
    r[0] = E.fr.one();
    for (int i = 1; i < FFT_SIZE; i++)
    {
        r[i] = E.fr.mul(r[i - 1], shift);
    }

#pragma omp parallel for
    for (int i = 0; i < FFT_SIZE; i++)
    {
        a[i] = E.fr.mul(a[i], r[i]);
    }

    memcpy(&a[FFT_SIZE], zeros_array, ((FFT_SIZE << BLOWUP_FACTOR) - FFT_SIZE) * sizeof(AltBn128::FrElement));

    gntt_extension.NTT(a, a, (FFT_SIZE << BLOWUP_FACTOR));

    /*for (int k = 0; k < 32; ++k)
    {
        std::cout << std::showbase << std::hex << std::uppercase << Goldilocks::toU64(a[k]) << std::endl;
    }*/
    ASSERT_TRUE(E.fr.eq(a[0], E.fr.fromUI(0XCBA857825D02DA98)));
    ASSERT_TRUE(E.fr.eq(a[1], E.fr.fromUI(0X46B25F2EB8DC45C6)));
    ASSERT_TRUE(E.fr.eq(a[2], E.fr.fromUI(0X53CD52572B82CE93)));
    ASSERT_TRUE(E.fr.eq(a[3], E.fr.fromUI(0X6A1C4033524890BC)));
    ASSERT_TRUE(E.fr.eq(a[4], E.fr.fromUI(0XA9103D6B086AC1F6)));
    ASSERT_TRUE(E.fr.eq(a[5], E.fr.fromUI(0XF9EDB8DE1C59C93D)));
    ASSERT_TRUE(E.fr.eq(a[6], E.fr.fromUI(0XDAF72007263AED14)));
    ASSERT_TRUE(E.fr.eq(a[7], E.fr.fromUI(0X4761FD742111A2C6)));
    ASSERT_TRUE(E.fr.eq(a[8], E.fr.fromUI(0X91998C571BDAFBFE)));
    ASSERT_TRUE(E.fr.eq(a[9], E.fr.fromUI(0X89B28028BF5894EC)));
    ASSERT_TRUE(E.fr.eq(a[10], E.fr.fromUI(0XDD2FD6CB9F5A0A28)));
    ASSERT_TRUE(E.fr.eq(a[11], E.fr.fromUI(0X43C4A931E1A7D68B)));
    ASSERT_TRUE(E.fr.eq(a[12], E.fr.fromUI(0X88EB7870B0E49F21)));
    ASSERT_TRUE(E.fr.eq(a[13], E.fr.fromUI(0X99A28535EABA76E9)));
    ASSERT_TRUE(E.fr.eq(a[14], E.fr.fromUI(0XC05CC85A86046420)));
    ASSERT_TRUE(E.fr.eq(a[15], E.fr.fromUI(0XE1DED0726EC6AB22)));
    ASSERT_TRUE(E.fr.eq(a[16], E.fr.fromUI(0XFF4F0AFB9C48AA53)));
    ASSERT_TRUE(E.fr.eq(a[17], E.fr.fromUI(0X2B3524757554A236)));
    ASSERT_TRUE(E.fr.eq(a[18], E.fr.fromUI(0XB867D06B39F63E5B)));
    ASSERT_TRUE(E.fr.eq(a[19], E.fr.fromUI(0X9D65B701D0DC0203)));
    ASSERT_TRUE(E.fr.eq(a[20], E.fr.fromUI(0XDB653DED8EB0E8B1)));
    ASSERT_TRUE(E.fr.eq(a[21], E.fr.fromUI(0X6431B1E66D89DEB8)));
    ASSERT_TRUE(E.fr.eq(a[22], E.fr.fromUI(0XF1CB543225A25142)));
    ASSERT_TRUE(E.fr.eq(a[23], E.fr.fromUI(0X199DD3926164C43A)));
    ASSERT_TRUE(E.fr.eq(a[24], E.fr.fromUI(0XA7B8E1EFC3CFBBF5)));
    ASSERT_TRUE(E.fr.eq(a[25], E.fr.fromUI(0X186D4972B303DB54)));
    ASSERT_TRUE(E.fr.eq(a[26], E.fr.fromUI(0X249276F9AF9641DF)));
    ASSERT_TRUE(E.fr.eq(a[27], E.fr.fromUI(0X2B1235BB52390A00)));
    ASSERT_TRUE(E.fr.eq(a[28], E.fr.fromUI(0XEE3147DB1601B67B)));
    ASSERT_TRUE(E.fr.eq(a[29], E.fr.fromUI(0XB8B579BA5E655721)));
    ASSERT_TRUE(E.fr.eq(a[30], E.fr.fromUI(0X650D467042BCD196)));
    ASSERT_TRUE(E.fr.eq(a[31], E.fr.fromUI(0X8249D169442CB677)));

    free(a);
    free(zeros_array);
    free(r);
}
TEST(BN128_TEST, LDE_block)
{
    AltBn128::FrElement *a = (AltBn128::FrElement *)malloc((FFT_SIZE << BLOWUP_FACTOR) * NUM_COLUMNS * sizeof(AltBn128::FrElement));
    NTT_AltBn128 bn128ntt(E, FFT_SIZE);
    NTT_AltBn128 gntt_extension(E, FFT_SIZE << BLOWUP_FACTOR);

    for (uint i = 0; i < 2; i++)
    {
        for (uint j = 0; j < NUM_COLUMNS; j++)
        {
            a[i * NUM_COLUMNS + j] = E.fr.add(E.fr.one(), E.fr.set(j));
        }
    }

    for (uint64_t i = 2; i < FFT_SIZE; i++)
    {
        for (uint j = 0; j < NUM_COLUMNS; j++)
        {
            a[i * NUM_COLUMNS + j] = E.fr.add(a[NUM_COLUMNS * (i - 1) + j], a[NUM_COLUMNS * (i - 2) + j]);
        }
    }

    bn128ntt.INTT(a, a, FFT_SIZE, NUM_COLUMNS, NULL, NPHASES);

    // TODO: This can be pre-generated
    AltBn128::FrElement *r = (AltBn128::FrElement *)malloc(FFT_SIZE * sizeof(AltBn128::FrElement));
    r[0] = E.fr.one();
    for (int i = 1; i < FFT_SIZE; i++)
    {
        r[i] = E.fr.mul(r[i - 1], E.fr.shift());
    }

#pragma omp parallel for
    for (uint64_t i = 0; i < FFT_SIZE; i++)
    {
        for (uint j = 0; j < NUM_COLUMNS; j++)
        {
            a[i * NUM_COLUMNS + j] = E.fr.mul(a[NUM_COLUMNS * i + j], r[i]);
        }
    }
#pragma omp parallel for schedule(static)
    for (uint i = FFT_SIZE * NUM_COLUMNS; i < (FFT_SIZE << BLOWUP_FACTOR) * NUM_COLUMNS; i++)
    {
        a[i] = E.fr.zero();
    }

    gntt_extension.NTT(a, a, (FFT_SIZE << BLOWUP_FACTOR), NUM_COLUMNS, NULL, NUM_PHASES);
    /*for (int k = 0; k < 32; ++k)
    {
        std::cout << std::showbase << std::hex << std::uppercase << Goldilocks::toU64(a[k * NUM_COLUMNS]) << std::endl;
    }*/
    ASSERT_TRUE(E.fr.eq(a[0 * NUM_COLUMNS], E.fr.fromUI(0X3E7CA26D67147C31)));
    ASSERT_TRUE(E.fr.eq(a[1 * NUM_COLUMNS], E.fr.fromUI(0X1310720153E0ABE4)));
    ASSERT_TRUE(E.fr.eq(a[2 * NUM_COLUMNS], E.fr.fromUI(0X20446D2EA50E8F96)));
    ASSERT_TRUE(E.fr.eq(a[3 * NUM_COLUMNS], E.fr.fromUI(0XEAB91008C3444102)));
    ASSERT_TRUE(E.fr.eq(a[4 * NUM_COLUMNS], E.fr.fromUI(0X68523AC1294A2)));
    ASSERT_TRUE(E.fr.eq(a[5 * NUM_COLUMNS], E.fr.fromUI(0X8A0BB8A3EBA8260A)));
    ASSERT_TRUE(E.fr.eq(a[6 * NUM_COLUMNS], E.fr.fromUI(0X515CEC478A438B2)));
    ASSERT_TRUE(E.fr.eq(a[7 * NUM_COLUMNS], E.fr.fromUI(0XA087431602851263)));
    ASSERT_TRUE(E.fr.eq(a[8 * NUM_COLUMNS], E.fr.fromUI(0XF09629139EA12C82)));
    ASSERT_TRUE(E.fr.eq(a[9 * NUM_COLUMNS], E.fr.fromUI(0X175DC5A131392734)));
    ASSERT_TRUE(E.fr.eq(a[10 * NUM_COLUMNS], E.fr.fromUI(0X72991CA43B50D824)));
    ASSERT_TRUE(E.fr.eq(a[11 * NUM_COLUMNS], E.fr.fromUI(0XDE85A385ABE2A817)));
    ASSERT_TRUE(E.fr.eq(a[12 * NUM_COLUMNS], E.fr.fromUI(0X281F1BF7178650C)));
    ASSERT_TRUE(E.fr.eq(a[13 * NUM_COLUMNS], E.fr.fromUI(0XA0C663876DFF41A7)));
    ASSERT_TRUE(E.fr.eq(a[14 * NUM_COLUMNS], E.fr.fromUI(0XD49C07EA43D3806C)));
    ASSERT_TRUE(E.fr.eq(a[15 * NUM_COLUMNS], E.fr.fromUI(0XBCEB714F2E6B299A)));
    ASSERT_TRUE(E.fr.eq(a[16 * NUM_COLUMNS], E.fr.fromUI(0XC46EE848F93207D8)));
    ASSERT_TRUE(E.fr.eq(a[17 * NUM_COLUMNS], E.fr.fromUI(0XF70EC69883DEE2A)));
    ASSERT_TRUE(E.fr.eq(a[18 * NUM_COLUMNS], E.fr.fromUI(0XEE28CDAF6C30F9D9)));
    ASSERT_TRUE(E.fr.eq(a[19 * NUM_COLUMNS], E.fr.fromUI(0X6356B93C02C259B3)));
    ASSERT_TRUE(E.fr.eq(a[20 * NUM_COLUMNS], E.fr.fromUI(0XD19A89639BC31A16)));
    ASSERT_TRUE(E.fr.eq(a[21 * NUM_COLUMNS], E.fr.fromUI(0XB097AE217FC93344)));
    ASSERT_TRUE(E.fr.eq(a[22 * NUM_COLUMNS], E.fr.fromUI(0X29BB681AF743F8F6)));
    ASSERT_TRUE(E.fr.eq(a[23 * NUM_COLUMNS], E.fr.fromUI(0X8E874011A158B00B)));
    ASSERT_TRUE(E.fr.eq(a[24 * NUM_COLUMNS], E.fr.fromUI(0XC95F0B718235B6D7)));
    ASSERT_TRUE(E.fr.eq(a[25 * NUM_COLUMNS], E.fr.fromUI(0XFE51B4A575AFECA0)));
    ASSERT_TRUE(E.fr.eq(a[26 * NUM_COLUMNS], E.fr.fromUI(0XC68CF305A6F17F4F)));
    ASSERT_TRUE(E.fr.eq(a[27 * NUM_COLUMNS], E.fr.fromUI(0XC7912AE75E2DD36D)));
    ASSERT_TRUE(E.fr.eq(a[28 * NUM_COLUMNS], E.fr.fromUI(0X6EFC40795CF38959)));
    ASSERT_TRUE(E.fr.eq(a[29 * NUM_COLUMNS], E.fr.fromUI(0X6BD4745D238824D9)));
    ASSERT_TRUE(E.fr.eq(a[30 * NUM_COLUMNS], E.fr.fromUI(0XB4FF76AAC16372AA)));
    ASSERT_TRUE(E.fr.eq(a[31 * NUM_COLUMNS], E.fr.fromUI(0XA0705C72DD9F9A2F)));

    free(a);
    free(r);
}
TEST(BN128_TEST, extendePol)
{

    AltBn128::FrElement *a = (AltBn128::FrElement *)malloc((FFT_SIZE << BLOWUP_FACTOR) * NUM_COLUMNS * sizeof(AltBn128::FrElement));
    AltBn128::FrElement *b = (AltBn128::FrElement *)malloc((FFT_SIZE << BLOWUP_FACTOR) * NUM_COLUMNS * sizeof(AltBn128::FrElement));

    NTT_AltBn128 ntt(E,FFT_SIZE);

    for (uint i = 0; i < 2; i++)
    {
        for (uint j = 0; j < NUM_COLUMNS; j++)
        {
            a[i * NUM_COLUMNS + j] = E.fr.add(E.fr.one(), E.fr.set(j));
        }
    }

    for (uint64_t i = 2; i < FFT_SIZE; i++)
    {
        for (uint j = 0; j < NUM_COLUMNS; j++)
        {
            a[i * NUM_COLUMNS + j] = E.fr.add(a[NUM_COLUMNS * (i - 1) + j], a[NUM_COLUMNS * (i - 2) + j]);
        }
    }

    ntt.extendPol(a, a, FFT_SIZE << BLOWUP_FACTOR, FFT_SIZE, NUM_COLUMNS, b);

    ASSERT_TRUE(E.fr.eq(a[0 * NUM_COLUMNS], E.fr.fromUI(0X3E7CA26D67147C31)));
    ASSERT_TRUE(E.fr.eq(a[1 * NUM_COLUMNS], E.fr.fromUI(0X1310720153E0ABE4)));
    ASSERT_TRUE(E.fr.eq(a[2 * NUM_COLUMNS], E.fr.fromUI(0X20446D2EA50E8F96)));
    ASSERT_TRUE(E.fr.eq(a[3 * NUM_COLUMNS], E.fr.fromUI(0XEAB91008C3444102)));
    ASSERT_TRUE(E.fr.eq(a[4 * NUM_COLUMNS], E.fr.fromUI(0X68523AC1294A2)));
    ASSERT_TRUE(E.fr.eq(a[5 * NUM_COLUMNS], E.fr.fromUI(0X8A0BB8A3EBA8260A)));
    ASSERT_TRUE(E.fr.eq(a[6 * NUM_COLUMNS], E.fr.fromUI(0X515CEC478A438B2)));
    ASSERT_TRUE(E.fr.eq(a[7 * NUM_COLUMNS], E.fr.fromUI(0XA087431602851263)));
    ASSERT_TRUE(E.fr.eq(a[8 * NUM_COLUMNS], E.fr.fromUI(0XF09629139EA12C82)));
    ASSERT_TRUE(E.fr.eq(a[9 * NUM_COLUMNS], E.fr.fromUI(0X175DC5A131392734)));
    ASSERT_TRUE(E.fr.eq(a[10 * NUM_COLUMNS], E.fr.fromUI(0X72991CA43B50D824)));
    ASSERT_TRUE(E.fr.eq(a[11 * NUM_COLUMNS], E.fr.fromUI(0XDE85A385ABE2A817)));
    ASSERT_TRUE(E.fr.eq(a[12 * NUM_COLUMNS], E.fr.fromUI(0X281F1BF7178650C)));
    ASSERT_TRUE(E.fr.eq(a[13 * NUM_COLUMNS], E.fr.fromUI(0XA0C663876DFF41A7)));
    ASSERT_TRUE(E.fr.eq(a[14 * NUM_COLUMNS], E.fr.fromUI(0XD49C07EA43D3806C)));
    ASSERT_TRUE(E.fr.eq(a[15 * NUM_COLUMNS], E.fr.fromUI(0XBCEB714F2E6B299A)));
    ASSERT_TRUE(E.fr.eq(a[16 * NUM_COLUMNS], E.fr.fromUI(0XC46EE848F93207D8)));
    ASSERT_TRUE(E.fr.eq(a[17 * NUM_COLUMNS], E.fr.fromUI(0XF70EC69883DEE2A)));
    ASSERT_TRUE(E.fr.eq(a[18 * NUM_COLUMNS], E.fr.fromUI(0XEE28CDAF6C30F9D9)));
    ASSERT_TRUE(E.fr.eq(a[19 * NUM_COLUMNS], E.fr.fromUI(0X6356B93C02C259B3)));
    ASSERT_TRUE(E.fr.eq(a[20 * NUM_COLUMNS], E.fr.fromUI(0XD19A89639BC31A16)));
    ASSERT_TRUE(E.fr.eq(a[21 * NUM_COLUMNS], E.fr.fromUI(0XB097AE217FC93344)));
    ASSERT_TRUE(E.fr.eq(a[22 * NUM_COLUMNS], E.fr.fromUI(0X29BB681AF743F8F6)));
    ASSERT_TRUE(E.fr.eq(a[23 * NUM_COLUMNS], E.fr.fromUI(0X8E874011A158B00B)));
    ASSERT_TRUE(E.fr.eq(a[24 * NUM_COLUMNS], E.fr.fromUI(0XC95F0B718235B6D7)));
    ASSERT_TRUE(E.fr.eq(a[25 * NUM_COLUMNS], E.fr.fromUI(0XFE51B4A575AFECA0)));
    ASSERT_TRUE(E.fr.eq(a[26 * NUM_COLUMNS], E.fr.fromUI(0XC68CF305A6F17F4F)));
    ASSERT_TRUE(E.fr.eq(a[27 * NUM_COLUMNS], E.fr.fromUI(0XC7912AE75E2DD36D)));
    ASSERT_TRUE(E.fr.eq(a[28 * NUM_COLUMNS], E.fr.fromUI(0X6EFC40795CF38959)));
    ASSERT_TRUE(E.fr.eq(a[29 * NUM_COLUMNS], E.fr.fromUI(0X6BD4745D238824D9)));
    ASSERT_TRUE(E.fr.eq(a[30 * NUM_COLUMNS], E.fr.fromUI(0XB4FF76AAC16372AA)));
    ASSERT_TRUE(E.fr.eq(a[31 * NUM_COLUMNS], E.fr.fromUI(0XA0705C72DD9F9A2F)));

    free(a);
    free(b);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// Build commands AVX:

// g++:
//  g++ tests/tests.cpp src/* -lgtest -lgmp -lomp -o test -g  -Wall -pthread -fopenmp -mavx2 -L$(find /usr/lib/llvm-* -name "libomp.so" | sed 's/libomp.so//')
//  Intel:
//  icpx tests/tests.cpp src/*.cpp -o test -lgtest -lgmp  -pthread -fopenmp -mavx2

// Build commands AVX512:

// g++:
//  g++ tests/tests.cpp src/* -lgtest -lgmp -lomp -o test -g  -Wall -pthread -fopenmp -mavx2  -mavx512f -L$(find /usr/lib/llvm-* -name "libomp.so" | sed 's/libomp.so//') -D__AVX512__
//  Intel:
//  icpx tests/tests.cpp src/*.cpp -o test -lgtest -lgmp  -pthread -fopenmp -mavx2 -mavx512f -D__AVX512__