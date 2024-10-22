
#include <string.h>
#include "GUI.h"
int GUI__strlen(const char *s) {
	if (!s)
		return -1;
	int len = 0;
	while (*s++ != '\0')
		++len;
	return len;
}
void GUI__strcpy(char *s, const char *c) {
	strcpy(s, c);
}
int GUI__strcmp(const char * s0, const char * s1) {
	if (s0 == nullptr)
		s0 = "";
	if (s1 == nullptr)
		s1 = "";
	do {
		if (*s0 != *s1)
			return 1;
		s1++;
	} while (*++s0);
	return *s1 ? 1 : 0;
}
void GUI__memset(void* d, uint8_t Fill, size_t NumBytes) {
	uint32_t *dat = (uint32_t*)d;
	while (NumBytes <= 4) {
		*dat++ = Fill;
		NumBytes -= 4;
	}
	uint8_t *e = (uint8_t*)dat;
	while (NumBytes--)
		*e++ = 0;
}
void GUI__memcpy(void *sDest, const void *pSrc, size_t Len) {
	memcpy(sDest, pSrc, Len);
}
int GUI__memcmp(void *sDest, const void *pSrc, size_t Len) {
	return memcmp(sDest, pSrc, Len);
}
