#include <vector>
#include <thread>
#include <atomic>

extern "C" {
	void mtCmd(const char* batPath, int threadNum)
	{
		std::vector<std::string>	cmds;
		char buf[2048];
		FILE* fp = fopen(batPath, "r");
		while(fgets(buf, sizeof(buf), fp) != NULL) {
			cmds.push_back(buf);
		}

		std::atomic_uint index(0);

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
		auto threads = new std::thread[logicalCpus]();
		for (auto i = 0u; i < logicalCpus; i++) {
			threads[i] = std::thread(proc);
		}
		for (auto i = 0u; i < logicalCpus; i++) {
			threads[i].join();
		}
		delete[] threads;
	}
}