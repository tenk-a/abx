#include <fks_common.h>
#include <fks_dirent.h>
#include <fks_time.h>
#include <stdint.h>
#include <stdio.h>
#include <fks_perfcnt.h>
#include <fks_mbc.h>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <string>

Fks_DateTime dtt;

class App {
public:
    App()
    {
    }

    int main(int argc, char* argv[]) {
        printf("[%s]\n", argv[0]);
        fks_perfcnt_t tm_start = fks_perfcnt_get();
        for (int i = 1; i < argc; ++i) {
            char* p = argv[i];
            if (*p == '-') {
                if (scanOpts(p) == false)
                    return 1;
                continue;
            }
			if (oneFile(p) == false)
				return 1;
        }


        fks_perfcnt_t tm_dif = fks_perfcnt_get() - tm_start;
        printf("%g sec.\n", FKS_PERFCNT_TO_SEC(tm_dif));
        return 0;
    }

	bool oneFile(char const* fname) {
		size_t l = fks_fileSize(fname);
		printf("load %s : %d\n", fname, int(l));
		buf_.resize(l+1);
		memset(&buf_[0], 0, l+1);
		size_t sz = 0;
		fks_fileLoad(fname, &buf_[0], l, &sz);
		int rc = fks_mbc_tinyCheckJpEncode(&buf_[0], l, 3);
		static char const* tbl[] = { "----", "ASCII", "SJIS", "EUCJP", "UTF8" };
		printf("\tenc %s\n", tbl[rc >= 0 && rc < 5 ? rc : 0]);
		return true;
	}


private:
    bool scanOpts(char const* opt) {
		char const* s = opt + 1;
        int c = *s;
        ++s;
        switch (c) {
		case 'K':
			break;
        default:
            fprintf(stderr, "Unkown option : %s\n", opt);
            return false;
        }
        return true;
    }

private:
	std::vector<char>	buf_;
};


int main(int argc, char* argv[])
{
    return App().main(argc, argv);
}
