#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
namespace ssl = net::ssl;       // from <boost/asio/ssl.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>f


template <class Body, class Allocator>
auto make_http_request(http::request<Body, http::basic_fields<Allocator>>&& request) {
    int version = 11;

    std::string host = request.at(http::field::host).to_string();
    std::string port = "80";
    auto pos = host.find(":");
    // port detect
    if (pos != std::string::npos) {
        port = host.substr(pos + 1, host.length());
        host = host.substr(0, pos);
    }

    request.erase(http::field::proxy_connection);

    // http://mail.ru/news/ -> /news/
    std::string target(request.target());
    int i = 0;
    for (int j = 0; i < target.length(); i++) {
        if (target[i] == '/') {
            j++;
            if (j == 3) {
                break;
            }
        }
    }

    target = target.substr(i, target.length());

    request.target(target);

    // The io_context is required for all I/O
    net::io_context ioc;

    // These objects perform our I/O
    tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);

    // Look up the domain name
    // msg.at
    auto const results = resolver.resolve(host.c_str(), port.c_str());

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(stream).connect(results);

    // Send the HTTP request to the remote host
    http::write(stream, request);

    // This buffer is used for reading and must be persisted
    beast::flat_buffer buffer;

    // Declare a container to hold the response
    http::response<http::dynamic_body> res;

    // Receive the HTTP response
    http::read(stream, buffer, res);

    return res;
}