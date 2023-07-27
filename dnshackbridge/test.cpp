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
#include <gtest/gtest.h>

#include "serialize_addressinfo.h"

TEST(AddrInfoTest, SerializationAndDeserialization)
{
    struct addrinfo original, originalNext;
    struct sockaddr_in originalAddr;
    struct sockaddr_in6 originalNextAddr;

    // Initialize originalAddr and originalNextAddr to an IPv4 address
    inet_pton(AF_INET, "192.168.0.1", &(originalAddr.sin_addr));
    originalAddr.sin_family = AF_INET;
    originalAddr.sin_port = htons(8080);

    inet_pton(AF_INET6, "fd12:3456:789a:bcde:f012:3456:789a:bcde", &(originalNextAddr.sin6_addr));
    originalNextAddr.sin6_family = AF_INET6;
    originalNextAddr.sin6_port = htons(8081);

    // Prepare original addrinfo
    original.ai_flags = AI_PASSIVE;
    original.ai_family = AF_INET;
    original.ai_socktype = SOCK_STREAM;
    original.ai_protocol = 0;
    original.ai_addrlen = sizeof(struct sockaddr_in);
    original.ai_addr = reinterpret_cast<struct sockaddr*>(&originalAddr);
    original.ai_canonname = strdup("www.original.com");
    original.ai_next = &originalNext;

    // Prepare next addrinfo
    originalNext.ai_flags = AI_CANONNAME;
    originalNext.ai_family = AF_INET6;
    originalNext.ai_socktype = SOCK_DGRAM;
    originalNext.ai_protocol = 0;
    originalNext.ai_addrlen = sizeof(struct sockaddr_in6);
    originalNext.ai_addr = reinterpret_cast<struct sockaddr*>(&originalNextAddr);
    originalNext.ai_canonname = strdup("www.next.com");
    originalNext.ai_next = nullptr;

    // Serialize
    AddrInfo originalWrapped(&original);
    std::stringstream ss;
    {
        cereal::JSONOutputArchive archive(ss);
        archive(originalWrapped);
    }

    // Deserialize
    AddrInfo deserialized;
    {
        cereal::JSONInputArchive archive(ss);
        archive(deserialized);
    }

    // Convert deserialized AddrInfo back to addrinfo
    struct addrinfo* deserializedRaw = deserialized.to_addrinfo();

    // Compare fields
    EXPECT_EQ(original.ai_flags, deserializedRaw->ai_flags);
    EXPECT_EQ(original.ai_family, deserializedRaw->ai_family);
    EXPECT_EQ(original.ai_socktype, deserializedRaw->ai_socktype);
    EXPECT_EQ(original.ai_protocol, deserializedRaw->ai_protocol);
    EXPECT_EQ(original.ai_addrlen, deserializedRaw->ai_addrlen);
    EXPECT_STREQ(original.ai_canonname, deserializedRaw->ai_canonname);
    EXPECT_EQ(0, memcmp(original.ai_addr, deserializedRaw->ai_addr, sizeof(original.ai_addr->sa_data)));

    EXPECT_EQ(originalNext.ai_flags, deserializedRaw->ai_next->ai_flags);
    EXPECT_EQ(originalNext.ai_family, deserializedRaw->ai_next->ai_family);
    EXPECT_EQ(originalNext.ai_socktype, deserializedRaw->ai_next->ai_socktype);
    EXPECT_EQ(originalNext.ai_protocol, deserializedRaw->ai_next->ai_protocol);
    EXPECT_EQ(originalNext.ai_addrlen, deserializedRaw->ai_next->ai_addrlen);
    EXPECT_STREQ(originalNext.ai_canonname, deserializedRaw->ai_next->ai_canonname);
    EXPECT_EQ(0,
              memcmp(originalNext.ai_addr, deserializedRaw->ai_next->ai_addr, sizeof(originalNext.ai_addr->sa_data)));

    freeaddrinfo2(deserializedRaw);
    free(original.ai_canonname);
    free(originalNext.ai_canonname);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
