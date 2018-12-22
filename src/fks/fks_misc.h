/**
 *  @file   fks_misc.h
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef FKS_MISC_H_INCLUDED
#define FKS_MISC_H_INCLUDED

#include <fks_common.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

FKS_LIB_DECL (char const*)  fks_skipSpc(char const* s);
FKS_LIB_DECL (char const*)  fks_skipNotSpc(char const* s);

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
namespace fks {


#ifndef FKS_OLD_CXX
template<class V, class C = typename V::value_type>
#else
template<class V>
#endif
class GetLine {
public:
 #ifdef FKS_OLD_CXX
	typedef char C;
 #endif
	/// @param flags : bit0:'\n' bit1:'\r' bit2='\r\n' bit3:'\0'
	GetLine(V const& v, size_t flags=0x07)
		: v_(v), cur_(v.size() ? &v[0] : 0), flags_(flags) {}

	template<class B>
	bool operator()(B& buf) {
		C const* s = cur_;
		if (!s)
			return false;
		C const* e = &v_[0] + v_.size();
		if (!s || s >= e)
			return false;
		C const* nxt = e;
		size_t	 flags = flags_;
		unsigned c;
		do {
			c = *s++;
			if (c == C('\n') && (flags & 1)) {
				nxt = s;
				break;
			} else if (c == C('\r')) {
				if (*s == C('\n') && (flags & 4)) {
					nxt = s + 1;
					break;
				} else if (flags & 2) {
					nxt = s;
					break;
				}
			} else if (c == 0 && (flags & 8)) {
				nxt = s;
				break;
			}
		} while (s < e);
		size_t l = s - cur_;
		if (l > 0) {
			buf.resize(l+2);	// after add '\n' '\0'
			size_t n = buf.size();
			memcpy(&buf[0], cur_, l);
			buf[l] = 0;
			buf.resize(l);
			cur_ = nxt;
			return true;
		}
		cur_ = e;
		return false;
	}

	C const*	current() const { return cur_; }

private:
	V const& 	v_;
	C const*	cur_;
	size_t		flags_;
};


/// line feed convertion (for vector<C> , basic_string(C))
/// @param flags : bit0:'\n' bit1:'\r' bit2='\r\n' bit3:skip '\0'
#ifndef FKS_OLD_CXX
template<class V, class C = typename V::value_type>
V& ConvLineFeed(V& v, C lf=C('\n'), size_t flags=0x0f)
#else
template<class V> V& ConvLineFeed(V& v, uint16_t lf=0x0d0a, size_t flags=0x0f)
#endif
{
 #ifdef FKS_OLD_CXX
	typedef char C;
 #endif
	size_t sz = v.size();
	if (sz == 0)
		return v;
	C* 		 d = &v[0];
	C const* s = d;
	C const* e = s + sz;
	unsigned c;
	do {
		c = *s++;
		if (c == C('\n') && (flags & 1)) {
			c = lf;
		} else if (c == C('\r')) {
			if (*s == C('\n') && (flags & 4)) {
				++s;
				c = lf;
			} else if (flags & 2) {
				c = lf;
			}
		} else if (c == 0 && (flags & 8)) {
			continue;
		}
		*d++ = c;
	} while (s < e);
	sz = d - &v[0];
	if (sz < v.size())
		*d = 0;
	v.resize(sz);
	return v;
}

#ifndef FKS_OLD_CXX
template<class V, class C = typename V::value_type>
V& ConvLineFeed2(V& v, uint16_t lf=0x0d0a, size_t flags=0x0f)
#else
template<class V> V& ConvLineFeed2(V& v, uint16_t lf=0x0d0a, size_t flags=0x0f)
#endif
{
 #ifdef FKS_OLD_CXX
	typedef char C;
 #endif
	if (lf <= 0xff)
		return ConvLineFeed(v,lf,flags);
	size_t sz = v.size();
	if (sz == 0)
		return v;
	C const* s = &v[0];
	C const* e = s + sz;
	size_t n = 0;
	while (s < e) {
		C c = *s++;
		if (c == unsigned('\r') && *s == unsigned('\n'))
			++s;
		n += (c == unsigned('\n') || c == unsigned('\r'));
	}
	sz += n;
	V dst(sz + 2);
	C lf1 = C(uint8_t(lf >> 8));
	C lf2 = C(uint8_t(lf));
	C* d = &dst[0];
	s = &v[0];
	e = s + sz;
	do {
		C c = *s++;
		if (c == C('\n') && (flags & 1)) {
			*d++ = lf1, *d++ = lf2;
		} else if (c == C('\r')) {
			if (*s == C('\n') && (flags & 4)) {
				++s;
				*d++ = lf1, *d++ = lf2;
			} else if (flags & 2) {
				*d++ = lf1, *d++ = lf2;
			} else {
				*d++ = c;
			}
		} else if (!c && (flags & 8)) {
			continue;
		} else {
			*d++ = c;
		}
	} while (s < e);
	sz = d - &dst[0];
	dst.resize(sz);
	v.swap(dst);
	return v;
}


} // fks
#endif


#endif  // FKS_MISC_H_INCLUDED
