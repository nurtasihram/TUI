

REG_FUNC(void, BindSize, uint16_t *xSize, uint16_t *ySize);
REG_FUNC(void, BindMouse, int16_t *xMouse, int16_t *yMouse, int8_t *keyMouse);
REG_FUNC(void, CreateDisplay, const wchar_t *lpTitle, uint16_t xSize, uint16_t ySize);
REG_FUNC(void, Dot, uint16_t x, uint16_t y, uint32_t rgb);
REG_FUNC(void, Fill, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t rgb);
REG_FUNC(void, SetBitmap, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const uint32_t *lpBits, uint16_t BytesPerLine);
REG_FUNC(void, GetBitmap, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t *lpBits, uint16_t BytesPerLine);

REG_FUNC(void, OpenConsole, void);
REG_FUNC(void, CloseConsole, void);
