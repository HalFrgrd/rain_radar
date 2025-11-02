#pragma once

#include <cstdint>
#include <cstdio>
#include <string_view>
#include <cassert>

enum class Err : int8_t
{
    OK = 0,
    ERROR = -1,
    TIMEOUT = -2,
    NO_CONNECTION = -3,
    INVALID_RESPONSE = -4,
    NO_DATA = -5,
    ALREADY_CONNECTED = -6,
    NOT_INITIALISED = -7,
    INVALID_ARGUMENT = -8,
    NO_MEMORY = -9,
    UNSUPPORTED = -10,
    
    // HTTP Client Errors (4xx)
    HTTP_BAD_REQUEST = -11,           // 400
    HTTP_UNAUTHORIZED = -12,          // 401
    HTTP_FORBIDDEN = -13,             // 403
    HTTP_NOT_FOUND = -14,             // 404
    HTTP_METHOD_NOT_ALLOWED = -15,    // 405
    HTTP_REQUEST_TIMEOUT = -16,       // 408
    HTTP_TOO_MANY_REQUESTS = -17,     // 429
    
    // HTTP Server Errors (5xx)
    HTTP_INTERNAL_SERVER_ERROR = -18, // 500
    HTTP_BAD_GATEWAY = -19,           // 502
    HTTP_SERVICE_UNAVAILABLE = -20,   // 503
    HTTP_GATEWAY_TIMEOUT = -21,       // 504
    
    // Generic HTTP errors
    HTTP_CLIENT_ERROR = -22,          // Other 4xx codes
    HTTP_SERVER_ERROR = -23,          // Other 5xx codes
    HTTP_UNKNOWN_ERROR = -24,         // Unrecognized HTTP status

    COULDNT_PARSE_HEADER = -25
};

constexpr std::string_view errToString(Err r)
{
    switch (r)
    {
    case Err::OK:
        return "OK";
    case Err::ERROR:
        return "ERROR";
    case Err::TIMEOUT:
        return "TIMEOUT";
    case Err::NO_CONNECTION:
        return "NO_CONNECTION";
    case Err::INVALID_RESPONSE:
        return "INVALID_RESPONSE";
    case Err::NO_DATA:
        return "NO_DATA";
    case Err::ALREADY_CONNECTED:
        return "ALREADY_CONNECTED";
    case Err::NOT_INITIALISED:
        return "NOT_INITIALISED";
    case Err::INVALID_ARGUMENT:
        return "INVALID_ARGUMENT";
    case Err::NO_MEMORY:
        return "NO_MEMORY";
    case Err::UNSUPPORTED:
        return "UNSUPPORTED";
    case Err::HTTP_BAD_REQUEST:
        return "HTTP_BAD_REQUEST";
    case Err::HTTP_UNAUTHORIZED:
        return "HTTP_UNAUTHORIZED";
    case Err::HTTP_FORBIDDEN:
        return "HTTP_FORBIDDEN";
    case Err::HTTP_NOT_FOUND:
        return "HTTP_NOT_FOUND";
    case Err::HTTP_METHOD_NOT_ALLOWED:
        return "HTTP_METHOD_NOT_ALLOWED";
    case Err::HTTP_REQUEST_TIMEOUT:
        return "HTTP_REQUEST_TIMEOUT";
    case Err::HTTP_TOO_MANY_REQUESTS:
        return "HTTP_TOO_MANY_REQUESTS";
    case Err::HTTP_INTERNAL_SERVER_ERROR:
        return "HTTP_INTERNAL_SERVER_ERROR";
    case Err::HTTP_BAD_GATEWAY:
        return "HTTP_BAD_GATEWAY";
    case Err::HTTP_SERVICE_UNAVAILABLE:
        return "HTTP_SERVICE_UNAVAILABLE";
    case Err::HTTP_GATEWAY_TIMEOUT:
        return "HTTP_GATEWAY_TIMEOUT";
    case Err::HTTP_CLIENT_ERROR:
        return "HTTP_CLIENT_ERROR";
    case Err::HTTP_SERVER_ERROR:
        return "HTTP_SERVER_ERROR";
    case Err::HTTP_UNKNOWN_ERROR:
        return "HTTP_UNKNOWN_ERROR";
    case Err::COULDNT_PARSE_HEADER:
        return "COULDNT_PARSE_HEADER";
    default:
        return "UNKNOWN";
    }
}

constexpr Err httpStatusToErr(int httpStatus)
{
    switch (httpStatus)
    {
    case 200:
    case 201:
    case 202:
    case 204:
        return Err::OK;
    case 400:
        return Err::HTTP_BAD_REQUEST;
    case 401:
        return Err::HTTP_UNAUTHORIZED;
    case 403:
        return Err::HTTP_FORBIDDEN;
    case 404:
        return Err::HTTP_NOT_FOUND;
    case 405:
        return Err::HTTP_METHOD_NOT_ALLOWED;
    case 408:
        return Err::HTTP_REQUEST_TIMEOUT;
    case 429:
        return Err::HTTP_TOO_MANY_REQUESTS;
    case 500:
        return Err::HTTP_INTERNAL_SERVER_ERROR;
    case 502:
        return Err::HTTP_BAD_GATEWAY;
    case 503:
        return Err::HTTP_SERVICE_UNAVAILABLE;
    case 504:
        return Err::HTTP_GATEWAY_TIMEOUT;
    default:
        if (httpStatus >= 400 && httpStatus < 500)
            return Err::HTTP_CLIENT_ERROR;
        else if (httpStatus >= 500 && httpStatus < 600)
            return Err::HTTP_SERVER_ERROR;
        else if (httpStatus >= 200 && httpStatus < 300)
            return Err::OK;
        else
            return Err::HTTP_UNKNOWN_ERROR;
    }
}

template <typename T>
struct ResultOr
{
    static_assert(!std::is_same_v<T, Err>, "ResultOr<T> cannot have T = Result");

    Err const err;
    T value;

    bool ok() const { return err == Err::OK; };
    const T &unwrap() const
    {
        if (!ok())
        {
            printf("Tried to unwrap a ResultOr that was not OK: %s\n", errToString(err).data());
            assert(false);
        }
        return value;
    }

    ResultOr() = delete;

    // this one can be implicit
    ResultOr(Err r) : err(r), value()
    {
        assert(r != Err::OK);
    }
    explicit ResultOr(T v) : err(Err::OK), value(v) {}
};
