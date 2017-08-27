/**
 *  @file   StrzBuf.hpp
 *  @brief  strz buffer
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @note
 *      Boost Software License Version 1.0
 */

#ifndef STRZBUF_HPP
#define STRZBUF_HPP

template<unsigned SZ>
class StrzBuf {
public:
    enum { npos = size_t(-1) };

    StrzBuf() { memset(buf_, 0, sizeof(buf_)); }
    StrzBuf(char const* name) { strncpyZ(buf_, name, sizeof(buf_)); }
    StrzBuf(StrzBuf const& name) { strncpyZ(buf_, r.buf_, sizeof(buf_)); }

    StrzBuf& operator=(char const* name) { strncpyZ(buf_, name, sizeof(buf_)); return *this; }
    StrzBuf& operator=(StrzBuf const& r) { strncpyZ(buf_, r.buf_, sizeof(buf_)); return *this; }
    StrzBuf& operator+=(char const* name) { strncat(buf_, name, sizeof(buf_)); buf_[SZ-1] = '\0'; return *this; }
    StrzBuf& operator+=(StrzBuf const& r) { strncat(buf_, r.buf_, sizeof(buf_)); buf_[SZ-1] = '\0'; return *this; }

    size_t  size() const { return strlen(buf_); }
    size_t  capacity() const { return sizeof(buf_); }
    void    clear() { memset(buf_, 0, sizeof(buf_)); }
    bool    empty() const { return buf_[0] == '\0'; }
    char const* c_str() const { return buf_; }
    char const* data() const { return buf_; }

    char&   operator[](size_t n) { return buf_[n]; }
    char const& operator[](size_t n) const { return buf_[n]; }

    bool operator<(StrzBuf const& r) const {
     #if 0 //def _WIN32
        return _stricmp(buf_, r.buf_) < 0;
     #else
        return strcmp(buf_, r.buf_) < 0;
     #endif
    }

    bool operator<(char const* r) const {
     #if 0 //def _WIN32
        return _stricmp(buf_, r) < 0;
     #else
        return strcmp(buf_, r) < 0;
     #endif
    }

    size_t  find_first_of(char c) const {
        char const* s = strchr(buf_, c);
        if (s)
            return s - buf_;
        return npos;
    }
    size_t  find_last_of(char c) const {
        char const* s = strrchr(buf_, c);
        if (s)
            return s - buf_;
        return npos;
    }

    StrzBuf& assign(char const* b, char const* e) {
        size_t l = e - b;
        if (l > SZ-1)
            l = SZ-1;
        memcpy(buf_, b, l);
        buf_[l] = '\0';
        return *this;
    }

private:
    char buf_[SZ];
};


#endif
