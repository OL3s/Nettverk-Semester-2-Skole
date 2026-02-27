#include <arpa/inet.h>
#include <cctype>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/time.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

namespace network_utils {
    volatile std::sig_atomic_t g_running = 1;

    void handle_sigint(int) {
        g_running = 0;
    }

	void install_signal_handlers() {
		struct sigaction sa;
		std::memset(&sa, 0, sizeof(sa));
		sa.sa_handler = handle_sigint;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0; // do NOT request SA_RESTART
		(void)sigaction(SIGINT, &sa, nullptr);
		(void)sigaction(SIGTERM, &sa, nullptr);
	}

    uint16_t parse_port_or_exit(const char* s) {
        if (!s || !*s) {
            std::cerr << "Missing port\n";
            std::exit(2);
        }
        char* end = nullptr;
        long v = std::strtol(s, &end, 10);
        if (!end || *end != '\0' || v < 1 || v > 65535) {
            std::cerr << "Invalid port: " << s << "\n";
            std::exit(2);
        }
        return static_cast<uint16_t>(v);
    }
}

namespace math_calc {
    double vect_dot_product(const std::vector<double>& a, const std::vector<double>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must be of the same size");
        }
        double result = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            result += a[i] * b[i];
        }
        return result;
    }
}

namespace protocol {
	static std::string hex_dump(const char* data, size_t len) {
		static const char* kHex = "0123456789ABCDEF";
		std::string out;
		out.reserve(len * 3);
		for (size_t i = 0; i < len; ++i) {
			unsigned char c = static_cast<unsigned char>(data[i]);
			out.push_back(kHex[(c >> 4) & 0xF]);
			out.push_back(kHex[c & 0xF]);
			if (i + 1 < len) {
				out.push_back(' ');
			}
		}
		return out;
	}

	static std::string escape_bytes(const char* data, size_t len) {
		std::ostringstream oss;
		for (size_t i = 0; i < len; ++i) {
			unsigned char c = static_cast<unsigned char>(data[i]);
			if (c == '\\') {
				oss << "\\\\";
			} else if (c == '\n') {
				oss << "\\n";
			} else if (c == '\r') {
				oss << "\\r";
			} else if (c == '\t') {
				oss << "\\t";
			} else if (std::isprint(c)) {
				oss << static_cast<char>(c);
			} else {
				oss << "\\x";
				static const char* kHex = "0123456789ABCDEF";
				oss << kHex[(c >> 4) & 0xF] << kHex[c & 0xF];
			}
		}
		return oss.str();
	}

	static std::string trim(const std::string& s) {
		size_t start = 0;
		while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
			++start;
		}
		size_t end = s.size();
		while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
			--end;
		}
		return s.substr(start, end - start);
	}

	static std::vector<double> parse_vector(std::string s) {
		for (char& c : s) {
			if (c == ',') {
				c = ' ';
			}
		}

		std::istringstream iss(s);
		std::vector<double> out;
		double x;
		while (iss >> x) {
			out.push_back(x);
		}
		if (out.empty()) {
			throw std::invalid_argument("Vector is empty");
		}
		return out;
	}

	// Expected payload format (ASCII):
	//   "1 2 3; 4 5 6"  (semicolon separates vectors)
	// Commas are also accepted inside vectors:
	//   "1,2,3;4,5,6"
	static std::pair<std::vector<double>, std::vector<double>> parse_two_vectors(const std::string& payload) {
		const size_t sep = payload.find(';');
		if (sep == std::string::npos) {
			throw std::invalid_argument("Missing ';' separator between vectors");
		}

		std::string left = trim(payload.substr(0, sep));
		std::string right = trim(payload.substr(sep + 1));
		if (left.empty() || right.empty()) {
			throw std::invalid_argument("Both vectors must be present");
		}

		return {parse_vector(left), parse_vector(right)};
	}

	static std::string format_vector(const std::vector<double>& v) {
		std::ostringstream oss;
		oss << "[";
		for (size_t i = 0; i < v.size(); ++i) {
			if (i) {
				oss << ", ";
			}
			oss << v[i];
		}
		oss << "]";
		return oss.str();
	}
}

int main(int argc, char** argv) {
	if (argc != 2 && argc != 3) {
		std::cerr << "Usage: " << (argc > 0 ? argv[0] : "udpVector") << " <port> [--selftest]\n";
		return 2;
	}

	const bool selftest = (argc == 3 && std::string(argv[2]) == "--selftest");
	if (argc == 3 && !selftest) {
		std::cerr << "Unknown option: " << argv[2] << "\n";
		return 2;
	}

	network_utils::install_signal_handlers();

	const uint16_t port = network_utils::parse_port_or_exit(argv[1]);

	const int fd = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		std::cerr << "socket() failed: " << std::strerror(errno) << "\n";
		return 1;
	}

	int reuse = 1;
	if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		std::cerr << "setsockopt(SO_REUSEADDR) failed: " << std::strerror(errno) << "\n";
		::close(fd);
		return 1;
	}

	// Ensure Ctrl+C stops the program promptly even if no packets arrive:
	// recvfrom() will time out periodically so the loop can observe g_running == 0.
	{
		timeval tv{};
		tv.tv_sec = 0;
		tv.tv_usec = 250000; // 250ms
		if (::setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
			std::cerr << "setsockopt(SO_RCVTIMEO) failed: " << std::strerror(errno) << "\n";
			::close(fd);
			return 1;
		}
	}

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY); // listen on all interfaces

	if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
		std::cerr << "bind() failed: " << std::strerror(errno) << "\n";
		::close(fd);
		return 1;
	}

	std::cout << "Listening for UDP packets on 0.0.0.0:" << port << " (Ctrl+C to stop)\n";
	std::cout.setf(std::ios::unitbuf);

	if (selftest) {
		const char* test_payload = "1 2 3; 4 5 6";
		sockaddr_in dst{};
		dst.sin_family = AF_INET;
		dst.sin_port = htons(port);
		if (::inet_pton(AF_INET, "127.0.0.1", &dst.sin_addr) != 1) {
			std::cerr << "selftest: inet_pton failed\n";
			::close(fd);
			return 1;
		}
		const ssize_t sent = ::sendto(fd, test_payload, std::strlen(test_payload), 0,
				reinterpret_cast<sockaddr*>(&dst), sizeof(dst));
		if (sent < 0) {
			std::cerr << "selftest: sendto failed: " << std::strerror(errno) << "\n";
			::close(fd);
			return 1;
		}
		std::cout << "Selftest: sent \"" << test_payload << "\" to 127.0.0.1:" << port << "\n";
	}

	while (network_utils::g_running) {
		char buffer[2048];
		sockaddr_in peer{};
		socklen_t peer_len = sizeof(peer);

		const ssize_t n = ::recvfrom(fd, buffer, sizeof(buffer), 0,
				reinterpret_cast<sockaddr*>(&peer), &peer_len);
		if (n < 0) {
			if (errno == EINTR) {
				continue;
			}
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				continue;
			}
			std::cerr << "recvfrom() failed: " << std::strerror(errno) << "\n";
			break;
		}

		char ip[INET_ADDRSTRLEN];
		const char* ip_s = ::inet_ntop(AF_INET, &peer.sin_addr, ip, sizeof(ip));
		if (!ip_s) {
			std::strncpy(ip, "?", sizeof(ip));
			ip[sizeof(ip) - 1] = '\0';
		}

		const uint16_t peer_port = ntohs(peer.sin_port);
		std::string payload(buffer, buffer + n);

		std::cout << "Received " << n << " bytes from " << ip << ":" << peer_port
				  << "\n  raw=\"" << protocol::escape_bytes(buffer, static_cast<size_t>(n)) << "\""
				  << "\n  hex=" << protocol::hex_dump(buffer, static_cast<size_t>(n))
				  << "\n";

		try {
			auto [a, b] = protocol::parse_two_vectors(payload);
			double dot = math_calc::vect_dot_product(a, b);
			std::cout << "Parsed vectors:"
					  << "\n  A=" << protocol::format_vector(a)
					  << "\n  B=" << protocol::format_vector(b)
					  << "\n  dot=" << dot
					  << "\n";
		} catch (const std::exception& e) {
			std::cout << "Error: " << e.what() << "\n";
		}

		if (selftest) {
			break;
		}
	}

	::close(fd);
	std::cout << "Stopped.\n";
	return 0;
}