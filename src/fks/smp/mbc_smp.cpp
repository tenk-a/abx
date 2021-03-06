#include <fks/fks_common.hpp>
#include <fks/fks_dirent.h>
#include <fks/fks_time.h>
#include <fks/fks_misc.h>
#include <fks/fks_mbc.h>
#include <fks/fks_path.h>
#include <fks/fks_perfcnt.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <string>

Fks_DateTime dtt;

class App {
public:
    App()
    {
		encs_.push_back(fks_mbc_utf8);
		encs_.push_back(fks_mbc_cp932);
		encs_.push_back(fks_mbc_eucjp);
		encs_.push_back(fks_mbc_sjisX213);
	 #ifdef FKS_WIN32
		encs_.push_back(fks_mbc_dbc);
	 #endif
		encs_.push_back(fks_mbc_utf32le);
		encs_.push_back(fks_mbc_utf32be);
		encs_.push_back(fks_mbc_utf16le);
		encs_.push_back(fks_mbc_utf16be);
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
	 #if 1
		std::vector<char>	v, v2;
		fks_fileLoad(fname, v);
		printf("load %s : %d\n", fname, int(v.size()));
		if (v.empty()) {
			return false;
		}
		fks::ConvLineFeed(v);
		fks_mbcenc_t me = fks_mbsAutoCharEncoding(&v[0], v.size(), 0, &encs_[0], encs_.size());	//fks_mbsAutoCharEncoding(&v[0], v.size());
		int			 k = 0;
		if (me) {
			if (me == fks_mbc_utf8)			k = 1;
			else if (me == fks_mbc_utf16le)	k = 2;
			else if (me == fks_mbc_utf16be)	k = 3;
			else if (me == fks_mbc_utf32le)	k = 4;
			else if (me == fks_mbc_utf32be)	k = 5;
			else if (me == fks_mbc_asc)		k = 6;
		 #ifdef FKS_WIN32
			else if (me == fks_mbc_dbc)		k = 7;
		 #endif
			else if (me == fks_mbc_cp932)   k = 8;
			else if (me == fks_mbc_sjisX213) k = 9;
			else if (me == fks_mbc_eucjp)	k = 10;
		}
		static char const* const s_tbl[] = { "----", "UTF8", "UTF16LE", "UTF16BE", "UTF32LE", "UTF32BE", "ASCII", "DBC", "CP932", "SJISx213", "EUCJP" };
		printf("\tenc %s\n", s_tbl[k]);
		fks_mkdir("mbc_out");
		char const* name = fks_pathBaseName(fname);
		encSave(name, ".sjisX213", fks_mbc_sjisX213, v);
		encSave(name, ".cp932"   , fks_mbc_cp932  , v);
		encSave(name, ".eucjp"   , fks_mbc_eucjp  , v);
		encSave(name, ".dbc"     , fks_mbc_dbc    , v);
		encSave(name, ".utf8"    , fks_mbc_utf8   , v);
		encSave(name, ".utf16le" , fks_mbc_utf16le, v);
		encSave(name, ".utf16be" , fks_mbc_utf16be, v);
		encSave(name, ".utf32le" , fks_mbc_utf32le, v);
		encSave(name, ".utf32be" , fks_mbc_utf32be, v);
	 #else
		size_t l = fks_fileSize(fname);
		printf("load %s : %d\n", fname, int(l));
		buf_.resize(l+1);
		memset(&buf_[0], 0, l+1);
		size_t sz = 0;
		fks_fileLoad(fname, &buf_[0], l, &sz);
		int rc = fks_mbc_tinyCheckJpEncode(&buf_[0], l, 3);
		static char const* tbl[] = { "----", "ASCII", "SJIS", "EUCJP", "UTF8" };
		printf("\tenc %s\n", tbl[rc >= 0 && rc < 5 ? rc : 0]);
	 #endif
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

	void encSave(char const* fname, char const* ext, fks_mbcenc_t enc, std::vector<char> const& v) {
		std::vector<char> v2;

		fks::ConvCharEncoding(v2, enc, v, fks::AutoCharEncoding(v,encs_));
		char buf[2000];
		sprintf(buf, "%s/%s%s", "mbc_out", fname, ext);
		fks_fileSave(buf, v2);
	}

private:
	std::vector<char>			buf_;
	std::vector<fks_mbcenc_t>	encs_;
};


int main(int argc, char* argv[])
{
    return App().main(argc, argv);
}
