#include "webserver/server.h"

std::size_t my_program_state::request_count() {
    static std::size_t count = 0;
    return ++count;
}

std::time_t my_program_state::now() {
    return std::time(0);
}

http_connection::http_connection(tcp::socket socket) : socket_(std::move(socket)) {}

void http_connection::start() {
    read_request();
    check_deadline();
}

void http_connection::read_request() {
    auto self = shared_from_this();

    http::async_read(socket_, buffer_, request_,
        [self](beast::error_code ec, std::size_t bytes_transferred) {
            boost::ignore_unused(bytes_transferred);
            if (!ec) self->process_request();
        });
}

void http_connection::process_request() {
    response_.version(request_.version());
    response_.keep_alive(false);

    switch (request_.method()) {
        case http::verb::get:
            response_.result(http::status::ok);
            response_.set(http::field::server, "Beast");
            create_response();
            break;

        default:
            // We return responses indicating an error if
            // we do not recognize the request method.
            response_.result(http::status::bad_request);
            response_.set(http::field::content_type, "text/plain");
            beast::ostream(response_.body())
                << "Invalid request-method '"
                << std::string(request_.method_string()) << "'";
            break;
    }

    write_response();
}

void http_connection::create_response() {
    if (request_.target() == "/count") {
        response_.set(http::field::content_type, "text/html");
        beast::ostream(response_.body())
            << "<html>\n"
            << "<head><title>Request count</title></head>\n"
            << "<body>\n"
            << "<h1>Request count</h1>\n"
            << "<p>There have been " << my_program_state::request_count()
            << " requests so far.</p>\n"
            << "</body>\n"
            << "</html>\n";
    } else if (request_.target() == "/time") {
        response_.set(http::field::content_type, "text/html");
        beast::ostream(response_.body())
            << "<html>\n"
            << "<head><title>Current time</title></head>\n"
            << "<body>\n"
            << "<h1>Current time</h1>\n"
            << "<p>The current time is " << my_program_state::now()
            << " seconds since the epoch.</p>\n"
            << "</body>\n"
            << "</html>\n";
    } else if (request_.target() == "/") {
        response_.set(http::field::content_type, "text/html");
        beast::ostream(response_.body())
            << "<html>\n"
            << "<head><title>Home Page</title></head>\n"
            << "<body>\n"
            << "<h1>Hello World</h1>\n"
            << "<p>Test</p>\n"
            << "</body>\n"
            << "</html>\n";
    } else {
        response_.result(http::status::not_found);
        response_.set(http::field::content_type, "text/plain");
        beast::ostream(response_.body()) << "404 Not Found. \r\n";
    }
}

void http_connection::write_response() {
    auto self = shared_from_this();

    response_.content_length(response_.body().size());

    http::async_write(
        socket_, response_, [self](beast::error_code ec, std::size_t) {
            self->socket_.shutdown(tcp::socket::shutdown_send, ec);
            self->deadline_.cancel();
        });
}

void http_connection::check_deadline() {
    auto self = shared_from_this();

    deadline_.async_wait([self](beast::error_code ec) {
        if (!ec) {
            // Close socket to cancel any outstanding operation.
            self->socket_.close(ec);
        }
    });
}

// 	// Check whether we have spent enough time on this connection.
// 	void check_deadline() {
// 		auto self = shared_from_this();

// 		deadline_.async_wait([self](beast::error_code ec) {
// 			if (!ec) {
// 				// Close socket to cancel any outstanding operation.
// 				self->socket_.close(ec);
// 			}
// 		});
// 	}
// };

// "Loop" forever accepting new connections.
void http_server(tcp::acceptor& acceptor, tcp::socket& socket) {
	acceptor.async_accept(socket, [&](beast::error_code ec) {
		if (!ec) std::make_shared<http_connection>(std::move(socket))->start();
		http_server(acceptor, socket);
	});
}