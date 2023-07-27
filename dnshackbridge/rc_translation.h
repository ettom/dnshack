#pragma once

#include <map>
#include <cstdio>

// Error return codes from bionic getaddrinfo()
enum class BionicRc {
    ok = 0,
    eai_addrfamily = 1, /* address family for hostname not supported */
    eai_again = 2,      /* temporary failure in name resolution */
    eai_badflags = 3,   /* invalid value for ai_flags */
    eai_fail = 4,       /* non-recoverable failure in name resolution */
    eai_family = 5,     /* ai_family not supported */
    eai_memory = 6,     /* memory allocation failure */
    eai_nodata = 7,     /* no address associated with hostname */
    eai_noname = 8,     /* hostname nor servname provided, or not known */
    eai_service = 9,    /* servname not supported for ai_socktype */
    eai_socktype = 10,  /* ai_socktype not supported */
    eai_system = 11,    /* system error returned in errno */
    eai_badhints = 12,  /* invalid value for hints */
    eai_protocol = 13,  /* resolved protocol is unknown */
    eai_overflow = 14   /* argument buffer overflow */
};

// Error return codes from glibc getaddrinfo()
enum class GlibcRc {
    ok = 0,
    eai_badflags = -1, /* Invalid value for `ai_flags' field.  */
    eai_noname = -2,   /* NAME or SERVICE is unknown.  */
    eai_again = -3,    /* Temporary failure in name resolution.  */
    eai_fail = -4,     /* Non-recoverable failure in name res.  */
    eai_family = -6,   /* `ai_family' not supported.  */
    eai_socktype = -7, /* `ai_socktype' not supported.  */
    eai_service = -8,  /* SERVICE not supported for `ai_socktype'.  */
    eai_memory = -10,  /* Memory allocation failure.  */
    eai_system = -11,  /* System error returned in `errno'.  */
    eai_overflow = -12 /* Argument buffer overflow.  */
};

inline const std::map<BionicRc, GlibcRc> bionic_to_glibc_map = {
        {BionicRc::ok, GlibcRc::ok},
        {BionicRc::eai_addrfamily, GlibcRc::eai_noname}, // No direct equivalent in glibc
        {BionicRc::eai_again, GlibcRc::eai_again},
        {BionicRc::eai_badflags, GlibcRc::eai_badflags},
        {BionicRc::eai_fail, GlibcRc::eai_fail},
        {BionicRc::eai_family, GlibcRc::eai_family},
        {BionicRc::eai_memory, GlibcRc::eai_memory},
        {BionicRc::eai_nodata, GlibcRc::eai_noname}, // No direct equivalent in glibc
        {BionicRc::eai_noname, GlibcRc::eai_noname},
        {BionicRc::eai_service, GlibcRc::eai_service},
        {BionicRc::eai_socktype, GlibcRc::eai_socktype},
        {BionicRc::eai_system, GlibcRc::eai_system},
        {BionicRc::eai_badhints, GlibcRc::eai_noname}, // No direct equivalent in glibc
        {BionicRc::eai_protocol, GlibcRc::eai_noname}, // No direct equivalent in glibc
        {BionicRc::eai_overflow, GlibcRc::eai_overflow}};

inline GlibcRc bionic_to_glibc_rc(const int bionic_error_code)
{
    if (bionic_error_code < static_cast<int>(BionicRc::ok)
        || bionic_error_code > static_cast<int>(BionicRc::eai_overflow)) {
        fprintf(stderr, "unknown bionic getaddrinfo rc: %d\n", bionic_error_code);
        return GlibcRc::eai_noname;
    }

    const auto it = bionic_to_glibc_map.find(static_cast<BionicRc>(bionic_error_code));
    if (it != bionic_to_glibc_map.end()) {
        return it->second;
    } else {
        return GlibcRc::eai_noname;
    }
}
