#pragma once

#include <string_view>

#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include "webino/logging.hpp"
#include "webino/result.hpp"

namespace webino::net
{
    static SSL_CTX* _client_ctx = nullptr;

    inline Result<SSL_CTX*> _new_ctx(const SSL_METHOD* method = TLS_client_method())
    {
        SSL_CTX* ctx = SSL_CTX_new(method);

        if (!ctx)
            return {ResultCode::UNSUCCESSFUL, nullptr};

        return {ResultCode::SUCCESSFUL, ctx};
    }

    inline Result<void*> _free_client_ctx()
    {
        SSL_CTX_free(_client_ctx);
        return {ResultCode::SUCCESSFUL, nullptr};
    }

    inline Result<void*> _free_ctx(SSL_CTX* ctx)
    {
        SSL_CTX_free(ctx);
        return {ResultCode::SUCCESSFUL, nullptr};
    }

    inline Result<void*> _validate_cert(X509* cert, std::string_view domain_name)
    {
        if (X509_cmp_current_time(X509_get_notBefore(cert)) > 0 || X509_cmp_current_time(X509_get_notAfter(cert)) < 0) 
        {
            logging::log_warn("Certificate presented by the server is not yet valid or has expired.");
            return {ResultCode::SUCCESSFULISH, nullptr};
        }

        if (!domain_name.empty() && X509_check_host(cert, domain_name.data(), 0, 0, NULL) != 1) 
        {
            logging::log_warn("Domain name '" + std::string(domain_name) + "' does not match the hostname provided in the server certificate.");
            return {ResultCode::SUCCESSFULISH, nullptr};
        }

        return {ResultCode::SUCCESSFUL, nullptr};
    }
}