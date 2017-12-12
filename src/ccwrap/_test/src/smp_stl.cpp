#define __STDC_LIMIT_MACROS

#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <cstdint>
#include <cinttypes>
#include <cassert>
#include <cerrno>
#include <climits>
#include <cmath>
#include <cfloat>
#include <cctype>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <ctime>
#include <cwctype>
#include <cwchar>
#include <clocale>

#ifndef __CCWRAP_NO_HEADER_CFENV
#include <cfenv>
#endif

#include <algorithm>
#if !__CCWRAP_NO_HEADER_ARRAY
#include <array>
#endif
#ifndef __CCWRAP_NO_HEADER_ATOMIC
#include <atomic>
#endif
#include <bitset>
#if !__CCWRAP_NO_HEADER_CHRONO
#include <chrono>
#endif
#ifndef __CCWRAP_NO_HEADER_CODECVT
#include <codecvt>
#endif
#include <complex>
#if !__CCWRAP_NO_HEADER_CONDITION_VARIABLE
#include <condition_variable>
#endif
#include <exception>
#if !__CCWRAP_NO_HEADER_FORWARD_LIST
#include <forward_list>
#endif
#include <fstream>
#include <functional>

#if !__CCWRAP_NO_HEADER_FUTURE
#include <future>
#endif
#ifndef __CCWRAP_NO_HEADER_INITIALIZER_LIST
#include <initializer_list>
#endif
#include <iomanip>
#include <ios>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#if !__CCWRAP_NO_HEADER_MUTEX
#include <mutex>
#endif
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#if !__CCWRAP_NO_HEADER_RANDOM
#include <random>
#endif
#if !__CCWRAP_NO_HEADER_RATIO
#include <ratio>
#endif
#if !__CCWRAP_NO_HEADER_REGEX
#include <regex>
#endif
#if !__CCWRAP_NO_HEADER_SCOPED_ALLOCATOR
#include <scoped_allocator>
#endif
#include <set>
#if !__CCWRAP_NO_HEADER_SHARED_MUTEX
#include <shared_mutex>
#endif
#include <sstream>
#include <stack>
#include <streambuf>
#include <string>
#if !__CCWRAP_NO_HEADER_SYSTEM_ERROR
#include <system_error>
#endif
#if !__CCWRAP_NO_HEADER_THREAD
#include <thread>
#endif
#if !__CCWRAP_NO_HEADER_TUPLE
#include <tuple>
#endif
#if !__CCWRAP_NO_HEADER_TYPE_TRAITS
#include <type_traits>
#endif
#ifndef __CCWRAP_NO_HEADER_TYPEINDEX
#include <typeindex>
#endif
#include <typeinfo>
#if !__CCWRAP_NO_HEADER_UNORDERD_MAP
#include <unordered_map>
#endif
#if !__CCWRAP_NO_HEADER_UNORDERD_SET
#include <unordered_set>
#endif
#include <utility>
#include <valarray>
#include <vector>



struct Containers {
	std::string						string_;
	std::vector<int>				vector_;
	std::list<int>					list_;
	std::deque<int>					deque_;
	std::map<int,int>				map_;
	std::multimap<int,int>			multimap_;
	std::set<int>					set_;
	std::multiset<int>				multiset_;
	std::unordered_map<int,int>		unordered_map_;
	std::unordered_multimap<int,int> unordered_multimap_;
	std::unordered_set<int>			unordered_set_;
	std::unordered_multiset<int>	unordered_multiset_;

	std::forward_list<int>			forward_list_;
	std::array<char,9>				array_;
	std::queue<int>					queue_;
	std::stack<int>					stack_;
};

template<class T>
void Container1_test_lite(T& t) {
	T	t0;
	T	t1 = T();
	T	t2(t1);
	T	t3 = t;
	bool	b = t1.empty();
	//assert(b == true);
	//std::size_t	l = t1.size();
	//assert(l == 0);
	t2 = t3;
	std::swap(t1, t2);
	typedef typename T::value_type	vtype;
	vtype v1 = vtype();
 #if 0
	for (typename T::iterator it = t.begin(); it != t.end(); ++it) {
		typename T::value_type	v2 = *it;
		std::swap(v1, v2);
	}
	for (typename T::const_iterator it = t1.begin(); it != t1.end(); ++it) {
	}
 #endif
}

template<class T>
void Container1_test_lite2(T& t) {
	T	t1 = T();
	T	t2(t1);
	T	t3 = t;
	bool	b = t1.empty();
	assert(b == true);
	std::size_t	l = t1.size();
	assert(l == 0);
	t2 = t3;
	std::swap(t1, t2);
	l = t1.size();
	assert(l > 0);
	typedef typename T::value_type	vtype;
	vtype v1 = vtype();
	for (typename T::iterator it = t.begin(); it != t.end(); ++it) {
		typename T::value_type	v2 = *it;
	}
	for (typename T::const_iterator it = t1.begin(); it != t1.end(); ++it) {
		typename T::value_type const v2 = *it;
	}
}

void	Containers_test() {
	Containers c;

	c.string_ = "1";
	c.array_[0] = 1;
	c.string_.push_back('2');
	c.vector_.push_back(1);
	c.list_.push_back(1);
	c.deque_.push_back(1);
	c.forward_list_.push_front(1);
	c.map_[1] = c.unordered_map_[1] = 1;
	c.multimap_.insert(std::pair<int,int>(1,1));
	c.unordered_multimap_.insert(std::pair<int,int>(1,1));
	c.set_.insert(1);
	c.multiset_.insert(1);
	c.unordered_set_.insert(1);
	c.unordered_multiset_.insert(1);
	c.queue_.push(1);
	c.stack_.push(1);

	Container1_test_lite(c.queue_);
	Container1_test_lite(c.stack_);
	Container1_test_lite(c.array_);
	Container1_test_lite(c.forward_list_);

	Container1_test_lite(c.string_);
	Container1_test_lite(c.vector_);
	Container1_test_lite(c.list_);
	Container1_test_lite(c.deque_);
	Container1_test_lite(c.map_);
	Container1_test_lite(c.multimap_);
	Container1_test_lite(c.set_);
	Container1_test_lite(c.multiset_);
	Container1_test_lite(c.unordered_map_);
	Container1_test_lite(c.unordered_multimap_);
	Container1_test_lite(c.unordered_set_);
	Container1_test_lite(c.unordered_multiset_);

	Container1_test_lite2(c.string_);
	Container1_test_lite2(c.vector_);
	Container1_test_lite2(c.list_);
	Container1_test_lite2(c.deque_);
	Container1_test_lite2(c.map_);
	Container1_test_lite2(c.multimap_);
	Container1_test_lite2(c.set_);
	Container1_test_lite2(c.multiset_);
	Container1_test_lite2(c.unordered_map_);
	Container1_test_lite2(c.unordered_multimap_);
	Container1_test_lite2(c.unordered_set_);
	Container1_test_lite2(c.unordered_multiset_);
}

// ---------------------

template<int N> struct less_n { bool operator()(int n) { return n < N; } };

void algorithm_test() {
	std::vector<int>	v;
	v.push_back(3);
	v.push_back(1);
	v.push_back(8);

	assert( std::all_of(v.begin(), v.end(), less_n<10>()) );
	assert( std::all_of(v.begin(), v.end(), less_n<5>()) == false);

	assert( std::any_of(v.begin(), v.end(), less_n<5>()) );
	assert( std::any_of(v.begin(), v.end(), less_n<0>()) == false );

	std::vector<int>	v2(3);
	std::copy_if(v.begin(), v.end(), v2.begin(), less_n<5>());
	assert(v2[0] == 3 && v2[1] == 1 && v2[2] == 0);

	std::copy_n(v.begin(), 3, v2.begin());
	assert(v2[0] == 3 && v2[1] == 1 && v2[2] == 8);

	assert(std::find_if_not(v.begin(), v.end(), less_n<10>()) == v.end());
	assert(std::find_if_not(v.begin(), v.end(), less_n<0>()) == v.begin());

	v2.clear();
	std::vector<int> v3;
	std::partition_copy(v.begin(), v.end(), std::back_inserter(v2), std::back_inserter(v3), less_n<5>());
	assert(v2.size() == 2 && v2[0] == 3 && v2[1] == 1 && v3.size() == 1 && v3[0] == 8);

	std::vector<int>::iterator	it = std::partition_point(v.begin(), v.end(), less_n<5>());
	assert(it != v.end() && *it == 8);
	it = std::partition_point(v.begin(), v.end(), less_n<10>());
	assert(it == v.end());

	v3 = std::move(v2);
	assert(v3.size() == 2 && v3[0] == 3 && v3[1] == 1);

	std::vector<std::string>	sv;
	sv.push_back("zero");
	sv.push_back("one");
	sv.push_back("two");
	sv.push_back("three");
	sv.push_back("four");
	std::move_backward(sv.begin(), sv.begin()+3, sv.end());
	assert(sv[2] == "zero" && sv[3] == "one" && sv[4] == "two");
	//for (unsigned i = 0; i < sv.size(); ++i) std::printf("%s\n", sv[i].c_str());
}



// ---------------------------

class ChronoTimeDisp {
public:
	//typedef std::chrono::high_system_clock	clock;
	typedef std::chrono::high_resolution_clock	clock;

	ChronoTimeDisp() : start_(clock::now()) {}
	~ChronoTimeDisp() {
		using namespace std::chrono;
		//std::time_t		b = clock::to_time_t(start_);
		//std::time_t		e = clock::to_time_t(clock::now());
		clock::time_point	e = clock::now();
		microseconds 		d = duration_cast<microseconds>(e - start_);
		std::cout << "time: " << d.count() << " usec" << std::endl;
	}

private:
	clock::time_point		start_;
};


void chrono_test() {
	using namespace std::chrono;
	system_clock::time_point	sys_tp = system_clock::now();
	system_clock::rep			sys_rep = duration_values<system_clock::rep>::zero();
	system_clock::rep			sys_rep2= duration_values<system_clock::rep>::min();
	system_clock::rep			sys_rep3= duration_values<system_clock::rep>::max();
	system_clock::period		sys_period = system_clock::period();
	system_clock::duration		sys_duration = system_clock::duration();
	std::time_t					sys_time = system_clock::to_time_t(system_clock::now());
	system_clock::time_point	ste_tp = system_clock::from_time_t(sys_time);
	assert(system_clock::is_steady == 0);

 #ifndef NO_STEADY_CLOCK
	steady_clock::time_point	ste_time = steady_clock::now();
	assert(steady_clock::is_steady);
 #endif

	hours			hour1(24);
	minutes 		minute1(60);
	seconds 		second1(60);
	milliseconds 	millisecond1(1000);
	microseconds 	microsecond1(1000);
	nanoseconds 	nanosecond1(1000);

	static_assert(treat_as_floating_point<hours::rep>::value == 0, "");
	typedef duration<double, std::ratio<1, 60> >	dd;
	static_assert(treat_as_floating_point<dd::rep>::value, "treat_as_floating_point<dd> is false");
}


static int sub_1(int n) {
	return n * 2;
}
static int sub_2(int a, int b) {
	return a - b;
}

class SubClass1 {
	int m_v;
public:
	SubClass1() : m_v(2) {}
	int mfn1(int a) { return a * m_v; }
	int mfn2(int a, int b) { return a + b; }
};

void functional_test() {
	std::function<int(int)>	fn = sub_1;
	int n = fn(2);
	assert(n == 4);
	fn = nullptr; //NULL;
	try {
		fn(1);
		assert(0);
	} catch (std::bad_function_call& e) {
		;
	}

	//using namespace std::placeholders;
	std::function<int(int)> fn2 = std::bind(sub_2, 4, std::placeholders::_1);
	n = fn2(3);
	assert(n == 1);

	SubClass1	sc1;
	assert(std::mem_fn(&SubClass1::mfn1)(sc1, 1) == 2);
	assert(std::mem_fn(&SubClass1::mfn2)(sc1, 1, 2) == 3);

 #if 0
	assert(std::bit_and<unsigned char>()(0xcc,0x88) == 0xcc & 0x88);
	assert(std::bit_or<unsigned char>()(0x50,0x0c) == 0x50 | 0x0c);
	assert(std::bit_xor<unsigned char>()(0x0f,0xff) == 0x0f ^ 0xff);
	assert(std::bit_not<unsigned char>()(0x33) == ~0x33);
 #endif

	n = 11;
	std::reference_wrapper<int>	rwn = std::ref(n);
	n = 10;
	assert(rwn == 10);

	std::reference_wrapper<int const> crwn = std::cref(n);
	n = 12;
	assert(crwn == 12);

 #ifndef __CCWRAP_NO_CXX11_AUTO
	auto rwn2 = std::ref(n);
	n = 11;
	assert(rwn2 == 11);
	auto crwn2 = std::cref(n);
	n = 13;
	assert(crwn2 == 13);
 #endif

	std::hash<int>	hash_i;

 #ifndef __CCWRAP_NO_DECLTYPE
	static_assert(std::is_placeholder<decltype(std::placeholders::_1)>::value,"");
	static_assert(std::is_bind_expression<decltype(std::bind(sub_2, 4, std::placeholders::_1))>::value,"");
 #endif
	static_assert(std::is_placeholder<int>::value == 0,"");
	static_assert(std::is_bind_expression<int(int)>::value == 0,"");


}

// ---------------------------




int main(int argc, char* argv[])
{
	ChronoTimeDisp chronoTimeDisp;
	std::printf("%s\n", argv[0]);
	std::printf("\t__cplusplus=%ld\n", __cplusplus);
	Containers_test();
	algorithm_test();
	chrono_test();
	//std::printf("\tdone\n");
	return 0;
}
