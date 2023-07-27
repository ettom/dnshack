#include <string>
#include <optional>
#include <memory>

#include <netdb.h>
#include <arpa/inet.h>

#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/optional.hpp>

struct SockAddr {
    SockAddr() {};
    SockAddr(const struct sockaddr* sa, const std::size_t len) :
            family(sa->sa_family),
            data(std::begin(sa->sa_data), std::end(sa->sa_data)) {};

    decltype(sockaddr().sa_family) family;
    std::vector<char> data;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(family, data);
    }
};

struct AddrInfo {
    AddrInfo() {};
    AddrInfo(const struct addrinfo* ai) :
            flags(ai->ai_flags),
            family(ai->ai_family),
            socktype(ai->ai_socktype),
            protocol(ai->ai_protocol),
            addrlen(ai->ai_addrlen),
            addr(ai->ai_addr ? std::make_optional<SockAddr>(ai->ai_addr, ai->ai_addrlen) : std::nullopt),
            canonname(ai->ai_canonname ? std::make_optional(ai->ai_canonname) : std::nullopt),
            next(ai->ai_next ? std::make_unique<AddrInfo>(ai->ai_next) : nullptr) {};

    decltype(addrinfo().ai_flags) flags;
    decltype(addrinfo().ai_family) family;
    decltype(addrinfo().ai_socktype) socktype;
    decltype(addrinfo().ai_protocol) protocol;
    decltype(addrinfo().ai_addrlen) addrlen;
    std::optional<SockAddr> addr;
    std::optional<std::string> canonname;
    std::unique_ptr<AddrInfo> next;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(flags, family, socktype, protocol, addrlen, addr, canonname, next);
    }

    struct addrinfo* to_addrinfo() const
    {
        struct addrinfo* ai = (struct addrinfo*)calloc(1, sizeof(struct addrinfo));

        ai->ai_flags = flags;
        ai->ai_family = family;
        ai->ai_socktype = socktype;
        ai->ai_protocol = protocol;
        ai->ai_addrlen = addrlen;

        if (addr) {
            ai->ai_addr = (struct sockaddr*)calloc(1, addrlen);
            ai->ai_addr->sa_family = addr.value().family;
            std::memcpy(ai->ai_addr->sa_data, addr.value().data.data(), addr.value().data.size());
        }

        if (canonname) {
            ai->ai_canonname = (char*)calloc(canonname->size() + 1, sizeof(char));
            std::strcpy(ai->ai_canonname, canonname->c_str());
        }

        if (next) {
            ai->ai_next = next->to_addrinfo();
        }

        return ai;
    }
};

struct GetAddrInfoParams {
    GetAddrInfoParams() {};
    GetAddrInfoParams(const char* _node, const char* _service, const struct addrinfo* _hints) :
            node(_node ? std::make_optional(_node) : std::nullopt),
            service(_service ? std::make_optional(_service) : std::nullopt),
            hints(_hints ? std::make_unique<AddrInfo>(_hints) : nullptr) {};

    std::optional<std::string> node;
    std::optional<std::string> service;
    std::unique_ptr<AddrInfo> hints;

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(node, service, hints);
    }
};

// Compared to the original, also frees ai_addr fields
inline void freeaddrinfo2(struct addrinfo* ai)
{
    struct addrinfo* p;
    while (ai != NULL) {
        p = ai;
        ai = ai->ai_next;
        free(p->ai_addr); // free the sockaddr too
        free(p->ai_canonname);
        free(p);
    }
}
