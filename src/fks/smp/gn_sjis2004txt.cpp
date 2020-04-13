#include <stdio.h>
#include <stdint.h>

static int fks_jis2sjis(unsigned c)
{
	if (c <= 0xffff) {
	    c -= 0x2121;
	    if (c & 0x100)
	    	c += 0x9e;
	    else
	    	c += 0x40;
	    if ((uint8_t)c >= 0x7f)
	    	++c;
	    c = (((c >> (8+1)) + 0x81)<<8) | ((uint8_t)c);
	    if (c >= 0xA000)
	    	c += 0x4000;
	    return c;
	} else {	// jis2004
		unsigned a, b;
		b = (uint16_t)c - 0x2121;
		a = b >> 8;
	    if (b & 0x100)
	    	b += 0x9e;
	    else
	    	b += 0x40;
	    b = (uint8_t)b;
		if (b >= 0x7f)
			++b;
		if (a < 78-1) {	// 1,3,4,5,8,12,15-ku (0,2,3,4,7,11,14)
			a = (a + 1 + 0x1df) / 2 - ((a+1)/8) * 3;
		} else { // 78..94
			a = (a + 1 + 0x19b) / 2;
		}
		return (a << 8) | b;
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
				unsigned d = fks_jis2sjis(c);
				printf("%c%c", uint8_t(d >> 8), uint8_t(d));
			}
			printf("\n");
		}
	}
	return 0;
}
