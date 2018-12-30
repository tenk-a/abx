#include <fks_misc.h>
#include <fks_argv.h>
#include <fks_io_mbs.h>
#include <stdio.h>
#include <string.h>

class App {
public:
	App() {}
	int main(int argc, char* argv[]) {
		printf("__cplusplus=%ld\n",__cplusplus);
		for (int i = 0; i < argc; ++i) {
			char* p = argv[i];
			if (*p == '-') {
				if (strncmp(p, "-k=", 3) == 0) {
					int64_t val = 0;
					if (fks_strExpr(p+3, NULL, &val) == 0) {
						printf("-k=%lld\n", val);
					} else {
						printf("%s\n", argv[i]);
					}
				}
			} else {
				printf("%s\n", FKS_MBS_S(argv[i]));
			}
		}
		return 0;
	}
};

#ifdef FKS_WIN32
int wmain(int argc, wchar_t *wargv[]) {
    fks_ioMbsInit(1,0);
    char** argv = fks_convArgWcsToMbs(argc, wargv);
	fks_argv_conv(&argc, &argv);
    return App().main(argc, argv);
}
#else
int main(int argc, char* argv)
{
	fks_argv_conv(&argc, &argv);
	return App().main(argc, argv);
}
#endif
