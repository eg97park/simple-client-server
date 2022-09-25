#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef __linux__
#include <arpa/inet.h>
#include <sys/socket.h>
#endif // __linux
#ifdef WIN32
#include <winsock2.h>
#include "../mingw_net.h"
#endif // WIN32
#include <thread>

#include <vector>
#include <algorithm>

#ifdef WIN32
void perror(const char* msg) { fprintf(stderr, "%s %ld\n", msg, GetLastError()); }
#endif // WIN32

void usage(const char* arg) {
	printf("syntax : %s <port> [-e[-b]]\n", arg);
	printf("  -e : echo\n");
	printf("  -b : broadcast\n");
	printf("sample : %s 1234 -e -b\n", arg);
}

struct Param {
	bool echo{false};
	bool broadcast{false};
	uint16_t port{0};

	bool parse(int argc, char* argv[]) {
		for (int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-e") == 0) {
				echo = true;
				continue;
			}
			if (strcmp(argv[i], "-b") == 0) {
				broadcast = true;
				continue;
			}
			port = atoi(argv[i]);
		}
		return port != 0;
	}
} param;

void recvThread(int sd, std::vector<int>& sdPool) {
	printf("connected\n");
	static const int BUFSIZE = 65536;
	char buf[BUFSIZE];
	while (true) {
		ssize_t res = ::recv(sd, buf, BUFSIZE - 1, 0);
		if (res == 0 || res == -1) {
			fprintf(stderr, "recv return %ld", res);
			perror(" ");
			break;
		}
		buf[res] = '\0';
		printf("%s", buf);
		fflush(stdout);
		if (param.echo) {
			res = ::send(sd, buf, res, 0);
			if (res == 0 || res == -1) {
				fprintf(stderr, "send return %ld", res);
				perror(" ");
				break;
			}
		}
		if (param.broadcast) {
			for (std::vector<int>::iterator it = sdPool.begin() ; it != sdPool.end(); ++it){
				res = ::send(*it, buf, res, 0);
				fprintf(stdout, "%ld -> %d\n", std::this_thread::get_id(), (*it));
				if (res == 0 || res == -1) {
					fprintf(stderr, "send return %ld", res);
					perror(" ");
					break;
				}
			}
		}
	}
	printf("disconnected\n");
	::close(sd);
	sdPool.erase(std::remove(sdPool.begin(), sdPool.end(), sd), sdPool.end());
}

int main(int argc, char* argv[]) {
	if (!param.parse(argc, argv)) {
		usage(argv[0]);
		return -1;
	}

#ifdef WIN32
	WSAData wsaData;
	WSAStartup(0x0202, &wsaData);
#endif // WIN32

	int sd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		perror("socket");
		return -1;
	}

	int res;
#ifdef __linux__
	int optval = 1;
	res = ::setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (res == -1) {
		perror("setsockopt");
		return -1;
	}
#endif // __linux

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(param.port);

	ssize_t res2 = ::bind(sd, (struct sockaddr *)&addr, sizeof(addr));
	if (res2 == -1) {
		perror("bind");
		return -1;
	}

	res = listen(sd, 5);
	if (res == -1) {
		perror("listen");
		return -1;
	}

	std::vector<int> sdPool;
	while (true) {
		struct sockaddr_in cli_addr;
		socklen_t len = sizeof(cli_addr);
		int cli_sd = ::accept(sd, (struct sockaddr *)&cli_addr, &len);
		if (cli_sd == -1) {
			perror("accept");
			break;
		}
		fprintf(stdout, "add %d\n", cli_sd);
		sdPool.push_back(cli_sd);
		std::thread* t = new std::thread(recvThread, cli_sd, std::ref(sdPool));
		t->detach();
	}
	::close(sd);
}
