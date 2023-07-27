#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/optional.hpp>

#include "serialize_addressinfo.h"

int main(int argc, char* argv[])
{
    const char* input_params = getenv("DNSHACK_PARAMS");
    if (input_params == NULL) {
        fprintf(stderr, "DNSHACK_PARAMS not set\n");
        return EAI_NONAME;
    }

    GetAddrInfoParams params;
    {
        std::stringstream ss(std::string {input_params});
        cereal::JSONInputArchive ar(ss);
        ar(params);
    }

    auto opt_to_str = [](std::optional<std::string> s) -> const char* {
        if (s.has_value()) {
            return s.value().c_str();
        } else {
            return NULL;
        }
    };

    struct addrinfo* result = NULL;
    struct addrinfo* hints = params.hints ? params.hints->to_addrinfo() : NULL;
    const int rc = getaddrinfo(opt_to_str(params.node), opt_to_str(params.service), hints, &result);

    if (rc != 0) {
        return rc;
    }

    std::stringstream output;
    {
        cereal::JSONOutputArchive archive(output);
        archive(result == NULL ? std::nullopt : std::make_optional(AddrInfo(result)));
    }

    std::cout << output.str() << std::endl;

    freeaddrinfo(result);
    freeaddrinfo2(hints);

    return 0;
}
