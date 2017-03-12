class ARGB {

    private:
        uint8_t alpha, red, green, blue;

    public:
        ARGB() {};
        ~ARGB() {};
        void setAlpha(uint8_t alpha);
        void setRed(uint8_t red);
        void setGreen(uint8_t green);
        void setBlue(uint8_t blue);

        uint8_t getAlpha();
        uint8_t getRed();
        uint8_t getGreen();
        uint8_t getBlue();

        static ARGB* createARGB(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue);
        static ARGB* convertIntToArgb(uint32_t pixel);
        static int32_t convertArgbToInt(ARGB* argb);
        static void validateComponent(uint8_t* component);
};

void ARGB::setAlpha(uint8_t alphaChannel) {
    alpha = alphaChannel;
}

void ARGB::setRed(uint8_t redChannel) {
    red = redChannel;
}

void ARGB::setGreen(uint8_t greenChannel) {
    green = greenChannel;
}

void ARGB::setBlue(uint8_t blueChannel) {
    blue = blueChannel;
}

uint8_t ARGB::getAlpha() {
    return alpha;
}

uint8_t ARGB::getRed() {
    return red;
}

uint8_t ARGB::getGreen() {
    return green;
}

uint8_t ARGB::getBlue() {
    return blue;
}

ARGB* ARGB::createARGB(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue) {
    ARGB* argbComponent = new ARGB();

    argbComponent->setAlpha(alpha);
    argbComponent->setRed(red);
    argbComponent->setGreen(green);
    argbComponent->setBlue(blue);

    return argbComponent;
}

void ARGB::validateComponent(uint8_t* component) {
    if (*component > 255) {
        *component = 255;
    } else if (*component < 0) {
        *component = 0;
    }
}

ARGB* ARGB::convertIntToArgb(uint32_t pixel) {
    ARGB* argbComponent = new ARGB();

    argbComponent->setRed(pixel & 0xFF);
    argbComponent->setGreen((pixel >> 8) & 0xFF);
    argbComponent->setBlue((pixel >> 16) & 0xFF);
    argbComponent->setAlpha((pixel >> 24) & 0xFF);

    return argbComponent;
}

int32_t ARGB::convertArgbToInt(ARGB* argb) {
    return argb->getAlpha() << 24
        | argb->getBlue() << 16
        | argb->getGreen() << 8
        | argb->getRed();
}

