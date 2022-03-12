namespace Config {
    enum ScreenTypes {
        _80x25 = 80*25,
        _20x4 = 20*4,
        _16x2 = 16*2,
        _8x1 = 8*1
    };
    const ScreenTypes screen_type = _20x4;
    const unsigned char screen_y = 4;
    const unsigned char screen_x = 20;
    const unsigned int keylength = 16;
    const unsigned char* keyset = (const unsigned char*)"\x01\x02\x03\x0A\x04\x05\x06\x0B\x07\x08\x09\x0C\x0E\x00\x0F\x0D";

}