#include <stdio.h>
#include <stdint.h>

static int fks_jis2eucjp(int c)
{
	if (c < 0x80) {
		return c;
	} else if ( c <= 0xff) {
		return 0x8e00|c;
	} else if (c <= 0xffff) {
		return 0x8080|c;
	} else {
		return 0xf88080 | (uint16_t)c;
	}
}

int main()
{
	for (int l = 0; l < 2; ++l) {
		for (int k = 0x21; k < 0x7f; ++k) {
			if (l == 1) {
				if (k == 0x22 || k == 0x26 || k == 0x27 || k == 0x29 || k == 0x2a || k == 0x2b)
					continue;
				if (k >= 0x30 && k <= 0x6d)
					continue;
			}
			printf("%06x : ", l << 16 | k << 8 );
			for (int j = 0x21; j < 0x7f; ++j) {
				unsigned c = l << 16 | k << 8 | j;
				unsigned d = fks_jis2eucjp(c);
				if (d <= 0xffff)
					printf("%c%c", uint8_t(d >> 8), uint8_t(d));
				else
					printf("%c%c%c", uint8_t(d >> 16), uint8_t(d >> 8), uint8_t(d));
			}
			printf("\n");
		}
	}
	return 0;
}
