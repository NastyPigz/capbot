#ifndef SERVER_H
#define SERVER_H

// webserver
#define BOOST_ASIO_HAS_STD_INVOKE_RESULT BOOST_ASIO_DISABLE_STD_INVOKE_RESULT
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <ctime>
#include <memory>

namespace beast = boost::beast;	   // from <boost/beast.hpp>
namespace http = beast::http;	   // from <boost/beast/http.hpp>
namespace net = boost::asio;	   // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;  // from <boost/asio/ip/tcp.hpp>

namespace my_program_state {
	std::size_t request_count();

	std::time_t now();
}  // namespace my_program_state


class http_connection : public std::enable_shared_from_this<http_connection> {
public:
	http_connection(tcp::socket socket);

	// Initiate the asynchronous operations associated with the connection.
	void start();

private:
	// The socket for the currently connected client.
	tcp::socket socket_;

	// The buffer for performing reads.
	beast::flat_buffer buffer_{8192};

	// The request message.
	http::request<http::dynamic_body> request_;

	// The response message.
	http::response<http::dynamic_body> response_;

	// The timer for putting a deadline on connection processing.
	net::steady_timer deadline_{
		socket_.get_executor(), std::chrono::seconds(60)};

	// Asynchronously receive a complete request message.
	void read_request();

	// Determine what needs to be done with the request message.
	void process_request();

	// Construct a response message based on the program state.
	void create_response();

	// Asynchronously transmit the response message.
	void write_response();

	// Check whether we have spent enough time on this connection.
	void check_deadline();
};

// "Loop" forever accepting new connections.
void http_server(tcp::acceptor& acceptor, tcp::socket& socket);

#endif