#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sqlite3.h>
#include <iostream>
#include <format>
#include <vector>
#include <JSON/include/include/nlohmann/json.hpp>
#include <memory>
#include <map>
#include <thread>
#include <shared_mutex>
#include <mutex>
#include <list>
#include <openssl/ssl.h>
#include <openssl/err.h>

using json = nlohmann::json;





using std::string;


class Client;

#define PACKET_SIZE 1024


