#include <stdlib.h>
#include <assert.h>
#include <algorithm>
#include <functional>
#include <vector>
#include <atomic>
#include <thread>
#include <fks/fks_io.h>
#include "subr.hpp"
#include "StrzBuf.hpp"
#include "abxmt.h"


class MtExecBat1 {
public:
    MtExecBat1(size_t no, char const* tmpfname, std::vector<std::string>& cmds)
    	: tmpfname_(tmpfname), no_(int(no)), flag_(false), cmds_(cmds) {}
    MtExecBat1(MtExecBat1 const& r)
    	: tmpfname_(r.tmpfname_), no_(r.no_), flag_(r.flag_), cmds_(r.cmds_) {}
    ~MtExecBat1() {
    	if (flag_ && tmpfname_)
    	    remove(tmpfname_);
    }

    int operator()() {
    	for (;;) {
    	    unsigned procIdx = s_index_.fetch_add(1);
    	    if (procIdx >= cmds_.size())
    	    	break;
    	    std::string& str = cmds_[procIdx];
    	    if (str.empty())
    	    	continue;
    	    int md = check(&str[0], str.size());
    	    if (md == 0) {  // no string
    	    	continue;
    	    } else if (md == 1) {   // 1 line
    	    	//printf("#%d system %s\n", no_, str.c_str());
    	    	system(str.c_str());
    	    } else {	    	    // multi line
    	    	//printf("#%d bat %s\n", no_, tmpfname_);
    	    	FILE* fp = fopen(&tmpfname_[0], "wt");
    	    	if (fp) {
    	    	    //printf("#>>\n%s\n", str.c_str());
    	    	    flag_ = true;
    	    	    fwrite(&str[0], 1, str.size(), fp);
    	    	    fclose(fp);
    	    	    system(&tmpfname_[0]);
    	    	}
    	    }
    	}
    	return 0;
    }

    static int check(char const* str, size_t l) {
    	while (l > 0 && (str[l-1] == '\n')) {
    	    --l;
    	}
    	if (l == 0)
    	    return 0;
    	char const* p = (char*)memchr(str, '\n', l);
    	if (p == NULL)
    	    return 1;
    	return 2;
    }

private:
    char const*     	    	tmpfname_;
    int     	    	    	no_;
    bool    	    	    	flag_;
    std::vector<std::string>&	cmds_;

    static std::atomic_uint 	s_index_;
};
std::atomic_uint    MtExecBat1::s_index_;



void mtCmd(std::vector<std::string>& cmds, unsigned threadNum)
{
    size_t			logicalCpus = (threadNum == 0) ? std::thread::hardware_concurrency() : threadNum;
    std::thread*    threads		= new std::thread[logicalCpus]();
    if (logicalCpus > cmds.size()) {
    	logicalCpus = cmds.size();
    }
    std::vector<std::string>	tmpfnames(logicalCpus);
    char    	    	    	tmpFName[FIL_NMSZ+2] = {0};
    for (size_t i = 0; i < logicalCpus; ++i) {
    	char* nm = fks_tmpFile(&tmpFName[0], FIL_NMSZ, "abx_", ".bat");
    	if (nm == NULL) {
    	    assert(nm);
    	    return;
    	}
    	tmpfnames[i] = nm;
    	//printf("#%d thread( %s )\n", i, tmpfnames[i].c_str());
    	threads[i] = std::thread(MtExecBat1(i, tmpfnames[i].c_str(), cmds));
    }
    for (size_t i = 0; i < logicalCpus; ++i) {
    	threads[i].join();
    }
    delete[] threads;
 #if 0
    for (unsigned i = 0u; i < logicalCpus; ++i) {
    	std::string const& tmpfname = tmpfnames[i];
    	if (!tmpfname.empty())
    	    remove(tmpfname.c_str());
    }
 #endif
}
