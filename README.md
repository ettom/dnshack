# dnshack

A hacky solution to https://github.com/t184256/nix-on-droid/issues/2.

There are two components, the dnshackresolver executable and the dnshackbridge shared library. The dnshackbridge library
is compiled against the glibc of nix-on-droid and provides an implementation for the `getaddrinfo` function. Instead of
performing the DNS resolution itself, it serializes the input parameters and passes them to the dnshackresolver program.

dnshackresolver is compiled against Android libc and internally calls the Android libc `getaddrinfo` function for the
DNS resolution. It deserializes the input parameters, calls `getaddrinfo`, and then serializes the result to return it
back via stdout to dnshackbridge.

This method should allow any program compiled against Nix's glibc to use the bionic libc DNS resolution when
dnshackbridge is preloaded using `LD_PRELOAD`. It's also probably horrible for performance and likely not very reliable.

## Build it
As dnshackresolver needs to be built with the Android NDK, (which isn't available on aarch64) there is no native
package. So just build it on x86 with `nix-build ./dnshackresolver`. dnshackbridge can be built with `nix-build -E 'with
import <nixpkgs> {}; callPackage ./dnshackbridge {}'`. Set `crossSystem` if building on x86.

## Run it
```sh
export DNSHACK_RESOLVER_CMD=/path/to/dnshackresolver
export LD_PRELOAD=/path/to/libdnshackbridge.so
curl hostname_provided_via_android_dns
```

These exports can also be added to your `~/.profile` or turned into a wrapper script.

## Binary package
The toplevel default.nix pulls a prebuilt binary for dnshackresolver from releases and creates a ready-to-consume
package. So, to use it on nix-on-droid by default (with home-manager), do something like this:

```nix
home.file.".bashrc".text =
  let dnshack = pkgs.callPackage (builtins.fetchTarball "https://github.com/ettom/dnshack/tarball/master") { };
  in ''
    export DNSHACK_RESOLVER_CMD="${dnshack}/bin/dnshackresolver"
    export LD_PRELOAD="${dnshack}/lib/libdnshackbridge.so"
  '';
```
