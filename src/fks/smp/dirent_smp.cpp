#include <fks_common.h>
#include <fks_dirent.h>
#include <fks_time.h>
#include <stdio.h>
#include <fks_perfcnt.h>
#include <string.h>
#include <ctype.h>

Fks_DateTime dtt;

class App {
public:
	App()
		: flags_(0)
	{
		memset(this, 0, sizeof *this);
	}

	int main(int argc, char* argv[]) {
		printf("[%s]\n", argv[0]);
		fks_perfcnt_t tm_start = fks_perfcnt_get();
		for (int i = 1; i < argc; ++i) {
			char* p = argv[i];
			if (*p == '-') {
				if (scanOpts(p) == false)
					return 0;
				continue;
			}
			printf("### DIR %d>%s\n",i, p);
			//fks_createDirEntries(&dirEntries_, argv[i]);
			//auto cb = [](auto e, auto dir) { printf("%s/%s\n",dir,e->name);};
			//fks_foreachDirEntries(&dirEntries_, cb);
			if (flags_ & FKS_DE_NameStat) {
				Fks_DirEntPathStat*	pPathStats	= NULL;
				fks_isize_t			n			= fks_createDirEntPathStats(&pPathStats, argv[i], &matches_);
				if (n > 0 && pPathStats == NULL) {
					fprintf(stderr, "ERROR\n");
					return 1;
				}
				for (int i = 0; i < n; ++i) {
					Fks_DirEntPathStat* p = &pPathStats[i];
					Fks_DateTime		dt = {0};
					if (p->stat->st_ex_mode & FKS_S_EX_NOTMATCH)
						continue;
					fks_fileTimeToLocalDateTime(p->stat->st_mtime, &dt);
					printf("\t%-31s\t%10lld(%8llx) %04d-%02d-%02d %02d:%02d:%02d %03d\n"
						, p->path, p->stat->st_size, p->stat->st_size
						, dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second, dt.milliSeconds
					);
				}
				fks_releaseDirEntPathStats(pPathStats);
			} else {
				char** 		ppPaths = NULL;
				fks_isize_t	n		= fks_createDirEntPaths(&ppPaths, argv[i], &matches_);
				if (n > 0 && ppPaths == NULL) {
					fprintf(stderr, "ERROR\n");
					return 1;
				}
				for (int i = 0; i < n; ++i) {
					printf("\t%s\n", ppPaths[i]);
				}
				fks_releaseDirEntPaths(ppPaths);
			}
		}
		fks_perfcnt_t tm_dif = fks_perfcnt_get() - tm_start;
		printf("%g sec.\n", FKS_PERFCNT_TO_SEC(tm_dif));
		return 0;
	}

private:
	bool scanOpts(char const* s) {
		++s;
		int c = tolower(*(unsigned char*)s);
		++s;
		switch (c) {
		case 'r':
			matches_.flags |= FKS_DE_Recursive;
			break;
		case 'f':
			matches_.flags |= FKS_DE_File;
			break;
		case 'd':
			matches_.flags |= FKS_DE_Dir;
			break;
		case 'n':
			matches_.flags |= FKS_DE_Tiny;
			break;
		case 's':
			matches_.flags |= FKS_DE_NameStat;
			break;
		case 'a':
			matches_.flags |= FKS_DE_DotOrDotDot;
			break;
		default:
			fprintf(stderr, "Unkown option : %s\n", s);
			return false;
		}
		return true;
	}

private:
	Fks_DirEntries		dirEntries_;
	Fks_DirEnt_Matchs	matches_;
	int					flags_;
};


int main(int argc, char* argv[])
{
	return App().main(argc, argv);
}
