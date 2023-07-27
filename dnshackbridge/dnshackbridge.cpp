#include <string>
#include <optional>
#include <sstream>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <netdb.h>
#include <arpa/inet.h>

#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/optional.hpp>

#include "serialize_addressinfo.h"
#include "rc_translation.h"

int getaddrinfo(const char* node, const char* service, const struct addrinfo* hints, struct addrinfo** res)
{
    unsetenv("LD_PRELOAD"); // Otherwise this function will be called recursively

    *res = NULL;

    std::stringstream resolver_env;
    {
        cereal::JSONOutputArchive archive(resolver_env);
        archive(GetAddrInfoParams(node, service, hints));
    }

    setenv("DNSHACK_PARAMS", resolver_env.str().c_str(), 1);

    const char* resolver_cmd = getenv("DNSHACK_RESOLVER_CMD");
    if (resolver_cmd == NULL) {
        fprintf(stderr, "DNSHACK_RESOLVER_CMD not set\n");
        return EAI_NONAME;
    }

    FILE* fp = popen(resolver_cmd, "r");

    if (fp == NULL) {
        fprintf(stderr, "Couldn't open %s\n", resolver_cmd);
        return EAI_NONAME;
    }

    std::stringstream resolver_output;
    constexpr std::size_t MAX_LINE_SZ = 4096;
    char line[MAX_LINE_SZ];
    while (fgets(line, MAX_LINE_SZ, fp))
        resolver_output << line;

    const int ret = pclose(fp);
    if (ret == -1) {
        fprintf(stderr, "pclose error\n");
        return EAI_AGAIN;
    }

    if (WIFEXITED(ret)) {
        const int rc = WEXITSTATUS(ret);
        const auto translated_rc = static_cast<int>(bionic_to_glibc_rc(rc));
        if (rc != 0) {
            return translated_rc;
        }
    } else {
        return EAI_AGAIN;
    }

    std::optional<AddrInfo> result;
    {
        cereal::JSONInputArchive ar(resolver_output);
        ar(result);
    }

    if (result.has_value()) {
        struct addrinfo* r = result.value().to_addrinfo();
        *res = r;
    }

    return 0;
}
