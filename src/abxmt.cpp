#include "abxmt.h"
#ifdef ENABLE_MT_X

#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <atomic>
#include <thread>



#ifdef USE_CXX11LESS
class MtExecBat1 {
public:
	MtExecBat1(std::vector<std::string>& cmds) : cmds_(cmds) {}
	MtExecBat1(MtExecBat1 const& r) : cmds_(r.cmds_) {}

	void operator()() {
		unsigned procIdx = s_index_.fetch_add(1);
		if (procIdx < cmds_.size()) {
			system(cmds_[procIdx].c_str());
		}
	}

private:
	std::vector<std::string>&	cmds_;

	static std::atomic_uint 	s_index_;
};
std::atomic_uint 	MtExecBat1::s_index_;
#endif



void mtCmd(const char* batPath, unsigned threadNum)
{
	std::vector<std::string>	cmds;
	char buf[2048];
	FILE* fp = fopen(batPath, "rt");
	while(fgets(buf, sizeof(buf), fp) != NULL) {
		cmds.push_back(buf);
	}

 #ifdef USE_CXX11LESS
	unsigned logicalCpus = (threadNum == 0) ? std::thread::hardware_concurrency() : threadNum;
	std::thread*  threads = new std::thread[logicalCpus]();
	for (unsigned i = 0u; i < logicalCpus; ++i) {
		threads[i] = std::thread(MtExecBat1(cmds));
	}
	for (unsigned i = 0u; i < logicalCpus; ++i) {
		threads[i].join();
	}
	delete[] threads;
 #else
	std::atomic_uint index(0U);
	auto proc = [&index, &cmds]() {
		while (1) {
			auto procIdx = index++;
			if (cmds.size() <= procIdx)
				break;

			auto cmd = cmds[procIdx];
			system(cmd.c_str());
		}
	};
	unsigned logicalCpus = threadNum==0 ? std::thread::hardware_concurrency() : threadNum;
	std::thread*  threads = new std::thread[logicalCpus]();
	for (unsigned i = 0u; i < logicalCpus; i++) {
		threads[i] = std::thread(proc);
	}
	for (unsigned i = 0u; i < logicalCpus; i++) {
		threads[i].join();
	}
	delete[] threads;
 #endif
}

#endif
