#include <gtest/gtest.h>
#include <iostream>

#include <alt_bn128.hpp>
#include "../src/polinomial.hpp"
#include <immintrin.h>

extern AltBn128::Engine E;

TEST(POLINOMIAL, calculateZ)
{

    Polinomial z(8);
    Polinomial num(8);
    Polinomial den(8);

    E.fr.fromString(*num[0], "4911628027266406350604947469265275209387110149918804780815889794429309491684");
    E.fr.fromString(*num[1], "20310792779602427393732665622828856047084366356500917168377120086118863275811");
    E.fr.fromString(*num[2], "1196750058995069201718089946167816007720235841631385630607921276425001085994");
    E.fr.fromString(*num[3], "1111225186392788304513452573137810672277515231821693883178762450182762403508");
    E.fr.fromString(*num[4], "2102452419006940660600983775350739742821322510470566294082864626851644261153");
    E.fr.fromString(*num[5], "972536729868622982328636333657569293935286385267103999640264218628124058043");
    E.fr.fromString(*num[6], "4600270189072507050582909884560888782305397078581242789133564660855840126729");
    E.fr.fromString(*num[7], "13483075053371846317432690503072357106062791052870074796725235091461436658263");

    E.fr.fromString(*den[0], "4911628027266406350604947469265275209387110149918804780815889794429309491684");
    E.fr.fromString(*den[1], "989958611368595508593843828882611976185149419898889677548111455563253207368");
    E.fr.fromString(*den[2], "5556456018503294844002330662621414936431575313377468232783891952818342998562");
    E.fr.fromString(*den[3], "21498951828027743595134499148168477355187285252473971039425093601612117999984");
    E.fr.fromString(*den[4], "8258529359730418991511276379948612512071981453201279778300801487103102209380");
    E.fr.fromString(*den[5], "4628856201859520072027666026369366739518281711169220909555071151541510158395");
    E.fr.fromString(*den[6], "17150545648004518440418588230363895606746817810019721318483854289360168133165");
    E.fr.fromString(*den[7], "13483075053371846317432690503072357106062791052870074796725235091461436658263");

    Polinomial::calculateZ(E, z, num, den);

    ASSERT_TRUE(E.fr.toString(*z[0]) == "1");
    ASSERT_TRUE(E.fr.toString(*z[1]) == "1");
    ASSERT_TRUE(E.fr.toString(*z[2]) == "10947560697510911632079566558878805489653361432836544974700601051600144928342");
    ASSERT_TRUE(E.fr.toString(*z[3]) == "3887990708794126804048948014853508364472768896809755625644620936391199506505");
    ASSERT_TRUE(E.fr.toString(*z[4]) == "17450127735505846934156659733761929644827144824153491828697120992279845087370");
    ASSERT_TRUE(E.fr.toString(*z[5]) == "7757805176756105117292078575710087639667088373893900965354734284941374388029");
    ASSERT_TRUE(E.fr.toString(*z[6]) == "18406769587444797546348508934909858263237438417093067496391277387400670728945");
    ASSERT_TRUE(E.fr.toString(*z[7]) == "1");
}

TEST(POLINOMIAL, calculateH1H2) {
    Polinomial f(8);
    Polinomial t(8);

    Polinomial h1(8);
    Polinomial h2(8);

    E.fr.fromString(*f[0], "7");
    E.fr.fromString(*f[1], "6");
    E.fr.fromString(*f[2], "5");
    E.fr.fromString(*f[3], "4");
    E.fr.fromString(*f[4], "3");
    E.fr.fromString(*f[5], "2");
    E.fr.fromString(*f[6], "1");
    E.fr.fromString(*f[7], "0");

    E.fr.fromString(*t[0], "0");
    E.fr.fromString(*t[1], "1");
    E.fr.fromString(*t[2], "2");
    E.fr.fromString(*t[3], "3");
    E.fr.fromString(*t[4], "4");
    E.fr.fromString(*t[5], "5");
    E.fr.fromString(*t[6], "6");
    E.fr.fromString(*t[7], "7");

    Polinomial::calculateH1H2(E, h1, h2, f, t);

    for(u_int32_t i = 0; i < 8; ++i) {
        std::cout << E.fr.toString(*h1[i]) << std::endl;
    }

    ASSERT_TRUE(E.fr.toString(*h1[0]) == "0");
    ASSERT_TRUE(E.fr.toString(*h1[1]) == "1");
    ASSERT_TRUE(E.fr.toString(*h1[2]) == "2");
    ASSERT_TRUE(E.fr.toString(*h1[3]) == "3");
    ASSERT_TRUE(E.fr.toString(*h1[4]) == "4");
    ASSERT_TRUE(E.fr.toString(*h1[5]) == "5");
    ASSERT_TRUE(E.fr.toString(*h1[6]) == "6");
    ASSERT_TRUE(E.fr.toString(*h1[7]) == "7");

    ASSERT_TRUE(E.fr.toString(*h2[0]) == "0");
    ASSERT_TRUE(E.fr.toString(*h2[1]) == "1");
    ASSERT_TRUE(E.fr.toString(*h2[2]) == "2");
    ASSERT_TRUE(E.fr.toString(*h2[3]) == "3");
    ASSERT_TRUE(E.fr.toString(*h2[4]) == "4");
    ASSERT_TRUE(E.fr.toString(*h2[5]) == "5");
    ASSERT_TRUE(E.fr.toString(*h2[6]) == "6");
    ASSERT_TRUE(E.fr.toString(*h2[7]) == "7");
}
