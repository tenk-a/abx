


#ifdef __cplusplus
namespace fks {

class MbcBuff {
public:
	MbcBuff();
	MbcBuff(Fks_MbcEnv const* mbcEnv);

	Fks_MbcEnv const*	mbcEnv() { return mbcEnv_; }
	void setMbcEnv(Fks_MbcEnv const* e) { mbcEnv_ = e; }
	void convCRLFtoLF();

private:
	Fks_MbcEnv const*		mbcEnv_;
	uint8_t*				cur_;
	std::vector<uint8_t>	buff_;
};

}
#endif
