#ifndef COLOR_H
#define COLOR_H

#include <string>

// Color (struct): structure that holds four variables (r, g, b, a) that represent the four basic color channels (red, green, blue, alpha)
struct Color
{
    float r, g, b, a;
    Color()
    {
        r = g = b = a = 0;
    }
    Color(float value) : r(value/255), g(value/255), b(value/255), a(1) {}
    Color(float value, float alpha) : r(value/255), g(value/255), b(value/255), a(alpha/255) {}
    Color(float red, float green, float blue) : r{red/255}, g{green/255}, b{blue/255}, a{1}{};
    Color(float red, float green, float blue, float alpha) : r{red/255}, g{green/255}, b{blue/255}, a{alpha/255}{};

    Color(Color color, float alpha) : r(color.r), g(color.g), b(color.b), a(alpha/255) {}

    operator std::string() const
    {
        return std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a);
    }
};
struct Color8
{
    uint8_t r, g, b, a;
    Color8()
    {
        r = g = b = a = 0;
    }
    Color8(uint8_t value) : r(value), g(value), b(value), a(255) {}
    Color8(uint8_t value, uint8_t alpha) : r(value), g(value), b(value), a(alpha) {}
    Color8(uint8_t red, uint8_t green, uint8_t blue) : r{red}, g{green}, b{blue}, a{255}{};
    Color8(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) : r{red}, g{green}, b{blue}, a{alpha}{};

    operator std::string() const
    {
        return std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a);
    }
};

inline operator==(const Color8& color1, const Color8& color2)
{
    return color1.r == color2.r && color1.g == color2.g && color1.b == color2.b && color1.a == color2.a;
}
inline operator!=(const Color8& color1, const Color8& color2)
{
    return !(color1 == color2);
}

// color (namespace): provides easy access to basic colors
namespace color
{
    inline Color percent(float red, float green, float blue, float alpha)
    {
        Color result;
        result.r = red;
        result.g = green;
        result.b = blue;
        result.a = alpha;
        return result;
    }
    inline Color hex(uint32_t hex)
    {
        return Color(((hex >> 16) & 0xFF), ((hex >> 8) & 0xFF), ((hex) & 0xFF));
    }
    inline Color hexa(uint32_t hex, float alpha)
    {
        return Color(((hex >> 16) & 0xFF), ((hex >> 8) & 0xFF), ((hex) & 0xFF), alpha);
    }
    const Color
    ABSOLUTEZERO(0, 72, 186),
    ACIDGREEN(176, 191, 26),
    AERO(124, 185, 232),
    AFRICANVIOLET(178, 132, 190),
    AIRSUPERIORITYBLUE(114, 160, 193),
    ALICEBLUE(240, 248, 255),
    ALIZARIN(219, 45, 67),
    ALLOYORANGE(196, 98, 16),
    ALMOND(239, 222, 205),
    AMARANTHDEEPPURPLE(159, 43, 104),
    AMARANTHPINK(241, 156, 187),
    AMARANTHPURPLE(171, 39, 79),
    AMAZON(59, 122, 87),
    AMBER(255, 291, 0),
    AMETHYST(153, 102, 204),
    ANDROID_GREEN(61, 220, 132),
    ANTIQUEBRASS(205, 149, 117),
    ANTIQUEFUCHSIA(102, 93, 30),
    ANTIQUERUBY(132, 27, 45),
    ANTIQUEWHITE(250, 235, 215),
    APRICOT(251, 206, 177),
    AQUA(0, 255, 255),
    AQUAMARINE(127, 255, 212),
    ARCTICLIME(208, 255, 20),
    ARTICHOKEGREEN(75, 111, 68),
    ARYLIDEYELLOW(233, 214, 107),
    ASHGRAY(178, 190, 181),
    ASHGREY = ASHGRAY,
    ATOMICTANGERINE(255, 153, 102),
    AUREOLIN(253, 238, 0),
    AZURE(240, 255, 255),
    BABYBLUE(137, 207, 240),
    BABYBLUEEYES(161, 202, 241),
    BABYPINK(244, 194, 194),
    BABYPOWDER(254, 254, 250),
    BAKERMILLERPINK(255, 145, 175),
    BANANAMANIA(250, 231, 181),
    BARNRED(124, 10, 2),
    BATTLESHIPGRAY(132, 132, 130),
    BATTLESHIPGREY = BATTLESHIPGRAY,
    BEAUBLUE(188, 212, 230),
    BEAVER(159, 129, 112),
    BEIGE(245, 245, 220),
    BDAZZLEDBLUE(46, 88, 148),
    BIGDIPORUBY(156, 37, 66),
    BISQUE(255, 228, 196),
    BISTRE(61, 43, 31),
    BITTERLEMON(202, 224, 13),
    BLACK(0),
    BLACKBEAN(61, 12, 2),
    BLACKCORAL(84, 98, 111),
    BLACKOLIVE(59, 60, 54),
    BLACKPEARL(9, 20, 60),
    BLACKSHADOWS(191, 175, 178),
    BLANCHEDALMOND(255, 235, 205),
    BLASTOFFBRONZE(165, 113, 100),
    BLUEDEFRANCE(49, 140, 231),
    BLIZZARDBLUE(172, 229, 238),
    BLOODORANGE(209, 0, 28),
    BLOODRED(102, 0, 0),
    BLUE(0, 0, 255),
    BLUEBELL(162, 162, 208),
    BLUEGRAY(102, 153, 204),
    BLUEGREY = BLUEGRAY,
    BLUEJEANS(93, 173, 236),
    BLUESAPPHIRE(18, 97, 128),
    BLUEVIOLET(138, 43, 226),
    BLUEYONDER(80, 114, 167),
    BLUEITFUL(60, 105, 231),
    BLUSH(222, 93, 131),
    BOLE(121, 68, 59),
    BONE(227, 218, 201),
    BRICKRED(203, 65, 84),
    BRIGHTLILAC(216, 145, 239),
    BRIGHTYELLOW(255, 170, 29),
    BRONZE(205, 127, 50),
    BROWNSUGAR(175, 110, 77),
    BROWN(165, 42, 42),
    BUDGREEN(123, 182, 97),
    BUFF(255, 198, 128),
    BURGUNDY(128, 0, 32),
    BURLYWOOD(222, 184, 135),
    BURNISHEDBROWN(161, 122, 116),
    BURNTORANGE(204, 85, 0),
    BURNTSIENNA(233, 116, 81),
    BURNTUMBER(138, 51, 36),
    BYZANTINE(189, 51, 164),
    BYZANTIUM(112, 41, 99),
    CADETBLUE(95, 158, 160),
    CADETGRAY(145, 163, 175),
    CADETGREY = CADETGRAY,
    CADMIUMGREEN(0, 107, 60),
    CADMIUMORANGE(237, 135, 45),
    CAFEAULAIT(166, 123, 91),
    CAFENOIR(75, 54, 33),
    CAMBRIDGEBLUE(163, 193, 173),
    CAMEL(193, 154, 107),
    CAMEOPINK(239, 187, 204),
    CANARY(255, 255, 153),
    CANARYYELLOW(255, 239, 0),
    CANDYPINK(228, 113, 122),
    CARDINAL(196, 30, 58),
    CARIBBEANGREEN(0, 204, 153),
    CARMINE(215, 0, 64),
    CARNATIONPINK(255, 166, 201),
    CARNELIAN(179, 27, 27),
    CAROLINABLUE(86, 160, 211),
    CARROTORANGE(237, 145, 33),
    CATAWBA(112, 54, 66),
    CEDARCHEST(201, 90, 73),
    CELADON(172, 225, 175),
    CELESTE(178, 255, 255),
    CERISE(222, 49, 99),
    CERULEAN(0, 123, 167),
    CERULEANBLUE(42, 82, 190),
    CERULEANFROST(109, 155, 195),
    CHAMPAGNE(247, 231, 206),
    CHAMPAGNEPINK(241, 221, 207),
    CHARCOAL(54, 69, 79),
    CHARMPINK(230, 143, 172),
    CHARTREUSE(127, 255, 0),
    CHERRYBLOSSOMPINK(255, 183, 197),
    CHESTNUT(149, 69, 53),
    CHILIRED(226, 61, 40),
    CHINAPINK(222, 111, 161),
    CHINESERED(170, 56, 30),
    CHINESEVIOLET(133, 96, 136),
    CHINESEYELLOW(255, 178, 0),
    CHOCOLATE(210, 105, 30),
    CINEREOUS(152, 129, 123),
    CINNABAR(227, 66, 52),
    CINNAMONSATIN(205, 96, 126),
    CITRINE(228, 208, 10),
    CITRON(159, 169, 31),
    CLARET(127, 23, 52),
    CLEAR(0, 0, 0, 0),
    COFFEE(111, 78, 55),
    COLUMBIABLUE(185, 217, 235),
    CONGOPINK(248, 131, 121),
    COOLGRAY(140, 146, 172),
    COOLGREY = COOLGRAY,
    COPPER(184, 115, 51),
    COPPERPENNY(173, 111, 105),
    COPPERRED(203, 109, 81),
    COPPERROSE(153, 102, 102),
    COQUELICOT(255, 56, 0),
    CORAL(255, 127, 80),
    CORALPINK = CONGOPINK,
    CORDOVAN(137, 63, 69),
    CORN(251, 236, 93),
    CORNFLOWERBLUE(100, 149, 237),
    CORNSILK(255, 248, 220),
    COSMICCOBALT(46,45, 136),
    COSMICLATTE(255, 248, 231),
    COYOTEBROWN(129, 97, 60),
    COTTONCANDY(255, 188, 217),
    CREAM(255, 253, 208),
    CRIMSON(220, 20, 60),
    CULTUREDPEARL(245),
    CYAN(0, 255, 255),
    CYBERGRAPE(88, 66, 124),
    CYBERYELLOW(255, 211, 0),
    CYCLAMEN(245, 111, 161),
    DARKBLUE(0, 0, 139),
    DARKBROWN(101, 67, 33),
    DARKBYZANTIUM(93, 57, 84),
    DARKCYAN(0, 139, 139),
    DARKELECTRICBLUE(83, 104, 120),
    DARKGOLDENROD(184, 134, 11),
    DARKGRAY(169, 169, 169),
    DARKGREEN(0, 100, 0),
    DARKGREY = DARKGRAY,
    DARKJUNGLEGREEN(26, 36, 33),
    DARKKHAKI(189, 183, 107),
    DARKLAVA(72, 60, 50),
    DARKLIVER(84, 61, 55),
    DARKMAGENTA(139, 0, 139),
    DARKOLIVEGREEN(85, 107, 47),
    DARKORANGE(255, 140, 0),
    DARKORCHID(153, 50, 204),
    DARKPURPLE(48, 25, 52),
    DARKRED(139, 0, 0),
    DARKSALMON(233, 150, 122),
    DARKSEAGREEN(143, 188, 143),
    DARKSIENNA(60, 20, 20),
    DARKSKYBLUE(140, 190, 214),
    DARKSLATEBLUE(72, 61, 139),
    DARKSLATEGRAY(47, 79, 79),
    DARKSLATEGREY = DARKSLATEGRAY,
    DARKSPRINGGREEN(23, 114, 69),
    DARKTURQUOISE(0, 206, 209),
    DARKVIOLET(148, 0, 211),
    DAVYSGRAY(85),
    DAVYSGREY = DAVYSGRAY,
    DEEPCERISE(218, 50, 135),
    DEEPCHAMPAGNE(250, 214, 165),
    DEEPCHESTNUT(185, 78, 72),
    DEEPJUNGLEGREEN(0, 75, 73),
    DEEPPINK(255, 20, 147),
    DEEPSAFFRON(255, 153, 51),
    DEEPSKYBLUE(0, 191, 255),
    DEEPSPACESPARKLE(74, 100, 108),
    DEEPTAUPE(126, 94, 96),
    DENIM(21, 96, 189),
    DENIMBLUE(34, 67, 182),
    DESERT = CAMEL,
    DESERTSAND(237, 201, 175),
    DIMGRAY(105, 105, 105),
    DIMGREY = DIMGRAY,
    DODGERBLUE(30, 144, 255),
    DRABDARKBROWN(74, 65, 42),
    DUKEBLUE(0, 0, 156),
    DUTCHWHITE(239, 223, 187),
    EBONY(85, 93, 80),
    ECRU(194, 178, 128),
    EERIEBLACK(27),
    EGGPLANT(97, 64, 81),
    EGGSHELL(240, 234, 214),
    ELECTRICLIME(204, 255, 0),
    ELECTRICPURPLE(191, 0, 255),
    ELECTRICVIOLET(143, 0, 255),
    EMERALD(80, 200, 120),
    EMINENCE(108, 48, 130),
    ENGLISHLAVENDER(180, 131, 149),
    ENGLISHRED(171, 75, 82),
    ENGLISHVERMILLION(204, 71, 75),
    ENGLISHVIOLET(86, 60, 92),
    ERIN(0, 255, 64),
    ETONBLUE(150, 200, 162),
    FALLOW = CAMEL,
    FALURED(128, 24, 24),
    FANDANGO(181, 51, 137),
    FANDANGOPINK(222, 82, 133),
    FAWN(229, 170, 112),
    FERNGREEN(79, 121, 66),
    FIELDDRAB(108, 84, 30),
    FIERYROSE(255, 84, 112),
    FINN(104, 48, 104),
    FIREBRICK(178, 34, 34),
    FIREENGINERED(206, 32, 41),
    FLAME(226, 88, 34),
    FLAX(238, 220, 130),
    FLIRT(162, 0, 109),
    FLORALWHITE(255, 250, 240),
    FORESTGREEN(34, 139, 34),
    FRENCHBEIGE = CAFEAULAIT,
    FRENCHBISTRE(133, 109, 77),
    FRENCHBLUE(0, 114, 187),
    FRENCHFUCHSIA(253, 63, 146),
    FRENCHLILAC(134, 96, 142),
    FRENCHLIME(158, 253, 56),
    FRENCHMAUVE(212, 115, 212),
    FRENCHPINK(253, 108, 158),
    FRENCHRASPBERRY(199, 44, 72),
    FRENCHSKYBLUE(119, 181, 254),
    FRENCHVIOLET(136, 6, 206),
    FROSTBITE(233, 54, 167),
    FUCHSIA(255, 0, 255),
    FULVOUS(228, 132, 0),
    FUZZYWUZZY(135, 66, 31),

    GAINSBORO(220, 220, 220),
    GHOSTWHITE(248, 248, 255),
    GOLD(255, 215, 0),
    GOLDENROD(218, 165, 32),
    GRAY(128),
    GREEN(0, 128, 0),
    GREENYELLOW(173, 255, 47),
    GREY = GRAY,
    HONEYDEW(240, 255, 240),
    HOTPINK(255, 105, 180),
    INDIANRED(205, 92, 92),
    INDIGO(75, 0, 130),
    IVORY(255, 255, 240),
    KHAKI(240, 230, 140),
    LAVENDER(230, 230, 250),
    LAVENDERBLUSH(255, 240, 245),
    LAWNGREEN(124, 252, 0),
    LEMONCHIFFON(255, 250, 205),
    LIGHTBLUE(173, 216, 230),
    LIGHTCORAL(240, 128, 128),
    LIGHTCYAN(224, 255, 255),
    LIGHTGOLDENRODYELLOW(250, 250, 210),
    LIGHTGRAY(211, 211, 211),
    LIGHTGREEN(144, 238, 144),
    LIGHTGREY = LIGHTGRAY,
    LIGHTPINK(255, 182, 193),
    LIGHTSALMON(255, 160, 122),
    LIGHTSEAGREEN(32, 178, 170),
    LIGHTSKYBLUE(135, 206, 250),
    LIGHTSLATEGRAY(119, 136, 153),
    LIGHTSLATEGREY = LIGHTSLATEGRAY,
    LIGHTSTEELBLUE(176, 196, 222),
    LIGHTYELLOW(255, 255, 224),
    LIME(0, 255, 0),
    LIMEGREEN(50, 205, 50),
    LINEN(250, 240, 230),
    MAGENTA(255, 0, 255),
    MAROON(128, 0, 0),
    MEDIUMAQUAMARINE(102, 205, 170),
    MEDIUMBLUE(0, 0, 205),
    MEDIUMORCHID(186, 85, 211),
    MEDIUMPURPLE(147, 112, 219),
    MEDIUMSEAGREEN(60, 179, 113),
    MEDIUMSLATEBLUE(123, 104, 238),
    MEDIUMSPRINGGREEN(0, 250, 154),
    MEDIUMTURQUOISE(72, 209, 204),
    MEDIUMVIOLETRED(199, 21, 133),
    MIDNIGHTBLUE(25, 25, 112),
    MINTCREAM(245, 255, 250),
    MISTYROSE(255, 228, 225),
    MOCCASIN(255, 228, 181),
    NAVAJOWHITE(255, 222, 173),
    NAVY(0, 0, 128),
    OLDLACE(253, 245, 230),
    OLIVE(128, 128, 0),
    OLIVEDRAB(107, 142, 35),
    ORANGE(255, 165, 0),
    ORANGERED(255, 69, 0),
    ORCHID(218, 112, 214),
    PALEGOLDENROD(238, 232, 170),
    PALEGREEN(152, 251, 152),
    PALETURQUOISE(175, 238, 238),
    PALEVIOLETRED(219, 112, 147),
    PAPAYAWHIP(255, 239, 213),
    PEACHPUFF(255, 218, 185),
    PERU(205, 133, 63),
    PINK(255, 192, 203),
    PLUM(221, 160, 221),
    POWDERBLUE(176, 224, 230),
    PRIMROSEPETAL(249, 234, 176),
    PURPLE(128, 0, 128),
    RED(255, 0, 0),
    ROSYBROWN(118, 143, 143),
    ROYALBLUE(65, 105, 225),
    SADDLEBROWN(139, 69, 19),
    SALMON(250, 128, 114),
    SANDYBROWN(244, 164, 96),
    SEAGREEN(255, 245, 238),
    SEASHELL(255, 245, 238),
    SEPIABLACK(43, 2, 2),
    SIENNA(160, 82, 45),
    SILVER(192, 192, 192),
    SKYBLUE(135, 206, 235),
    SLATEBLUE(106, 90, 205),
    SLATEGRAY(112, 128, 144),
    SLATEGREY = SLATEGRAY,
    SNOW(255, 250, 250),
    SOFTPEACH(246, 217, 190),
    SPRINGGREEN(0, 255, 127),
    STEELBLUE(70, 130, 180),
    TAN(210, 180, 140),
    TEAL(0, 128, 128),
    THISTLE(0, 128, 128),
    TOMATO(255, 99, 71),
    TRADITIONALCHOCOLATE(123, 63, 0),
    TURQUOISE(64, 224, 208),
    VIOLET(238, 130, 238),
    WHEAT(245, 222, 179),
    WHITE(255),
    WHITESMOKE = CULTUREDPEARL,
    YELLOW(255, 255, 0),
    YELLOWGREEN(154, 205, 50);
}

inline Color operator +(const Color &color1, const Color &color2) {return color::percent(color1.r+color2.r, color1.g+color2.g, color1.b+color2.b, color1.a+color2.a);}
inline Color operator -(const Color &color1, const Color &color2) {return color::percent(color1.r-color2.r, color1.g-color2.g, color1.b-color2.b, color1.a-color2.a);}
inline Color operator +(const Color &color, float num) {return color::percent(color.r+num, color.g+num, color.b+num, color.a);}
inline Color operator -(const Color &color, float num) {return color::percent(color.r-num, color.g-num, color.b-num, color.a);}
inline Color operator *(const Color &color, float num) {return color::percent(color.r*num, color.g*num, color.b*num, color.a);}
inline Color operator /(const Color &color, float num) {return color::percent(color.r/num, color.g/num, color.b/num, color.a);}
inline Color &operator +=(Color &color1, const Color &color2)
{
    color1.r += color2.r;
    color1.g += color2.g;
    color1.b += color2.b;
    color1.a += color2.a;
    return color1;
}
inline Color &operator -=(Color &color1, const Color &color2)
{
    color1.r -= color2.r;
    color1.g -= color2.g;
    color1.b -= color2.b;
    color1.a -= color2.a;
    return color1;
}
inline Color &operator +=(Color &color1, float num)
{
    color1.r += num;
    color1.g += num;
    color1.b += num;
    return color1;
}
inline Color &operator -=(Color &color1, float num)
{
    color1.r -= num;
    color1.g -= num;
    color1.b -= num;
    return color1;
}
inline Color &operator *=(Color &color1, float num)
{
    color1.r *= num;
    color1.g *= num;
    color1.b *= num;
    return color1;
}
inline Color &operator /=(Color &color1, float num)
{
    color1.r /= num;
    color1.g /= num;
    color1.b /= num;
    return color1;
}
std::ostream& operator<<(std::ostream& os, const Color& obj);

inline Color8 operator *(const Color8 &color, float num) {return Color8(color.r*num, color.g*num, color.b*num, color.a);}

#endif