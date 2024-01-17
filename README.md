# Chaat

A sample client -> server messaging application, to understand the GnuTLS
API.

Usage:
1. Create a Root CA and a CA cert. The root CA file should be named `CAPrivate.pem`.
The certificate file needs to be named `ServCert.crt` and the private keys should be in
`ServPrivate.key`. To change thie behaviour, edit `incl/config.h`
2. Place the files in the current folder
3. Build the CMake project using `make` and then `make build`
4. Run the sender or receiver using `make send` and `make recv` respectively.
