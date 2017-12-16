#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <vector>
#include <atomic>
#include <thread>
#include "subr.hpp"
#include "StrzBuf.hpp"
#include "abxmt.h"


class MtExecBat1 {
public:
	MtExecBat1(std::vector<std::string>& cmds) : cmds_(cmds) {}
	MtExecBat1(MtExecBat1 const& r) : cmds_(r.cmds_) {}
	~MtExecBat1() {
		if (tmpFName_[0])
			remove(&tmpFName_[0]);
	}

	void operator()() {
		unsigned procIdx = s_index_.fetch_add(1);
		if (procIdx < cmds_.size()) {
			std::string& str = cmds_[procIdx];
			if (str.empty())
				return;
			int md = check(&str[0], str.size());
			if (md == 0) {	// no string
				return;
			} else if (md == 1) {	// 1 line
				system(str.c_str());
			} else {				// multi line
				if (tmpFName_[0] == 0) {	// make temp file
					if (TmpFile_make(&tmpFName_[0], FIL_NMSZ, "abx") == NULL)
						return;
				}
				FILE* fp = fopen(&tmpFName_[0], "wt");
				if (fp) {
					fwrite(&str[0], 1, str.size(), fp);
					fclose(fp);
					system(&tmpFName_[0]);
				}
			}
		}
	}

	static int check(char const* str, size_t l) {
		if (str[l-1] == '\n') {
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
	std::vector<std::string>&	cmds_;
	StrzBuf<FIL_NMSZ>			tmpFName_;

	static std::atomic_uint 	s_index_;
};
std::atomic_uint 	MtExecBat1::s_index_;



void mtCmd(std::vector<std::string>& cmds, unsigned threadNum)
{
	unsigned logicalCpus = (threadNum == 0) ? std::thread::hardware_concurrency() : threadNum;
	std::thread*  threads = new std::thread[logicalCpus]();
	for (unsigned i = 0u; i < logicalCpus; ++i) {
		threads[i] = std::thread(MtExecBat1(cmds));
	}
	for (unsigned i = 0u; i < logicalCpus; ++i) {
		threads[i].join();
	}
	delete[] threads;
}
