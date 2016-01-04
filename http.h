#ifndef HTTP_H
#define HTTP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <fcntl.h>
#include <winsock2.h>
// TODO:
//#define SHOP_DEBUG
//
#ifdef SHOP_DEBUG
#define PORT 8080
#else
#define PORT		 80
#endif //SHOP_DEBUG
#define USER_AGENT      "User-Agent: Mozilla/5.0 (X11; U; Linux i686; fr; rv:1.9.2.10) Gecko/20100922 Ubuntu/11.10 (lucid) Firefox/5.0 GTB7.1\r\n"
#define ENCODE_TYPE     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/ *;q=0.8\r\n"
#define CONNECTION_TYPE "Connection: keep-alive\r\n"
#define HTTP_VERSION    "HTTP/1.1"
#define TIMEOUT_SEC     5
#define TIMEOUT_MSEC    0
#define IP_ADDRESS  "10.195.226.56"
#define IP_CONNECT_STRING "http://%s/MLB/AddGK?cmd=%s&wip_no=%s&station_type=%s"
#define HTTP_VERSION "HTTP/1.1"
/* get an ip from an hostname (DNS search) */
char* host_to_ip(const char *hostname);

/* create a socket in INET TCP mode on PORT (for HTTP request) */
bool http_create_socket(SOCKET *sock, SOCKADDR_IN *sin, const char *ip);

/* perform an HTTP and return the result. Note : the result must be freed */
char* http_request(SOCKET sock, const char *hostname, const char *page);

/*return a pointer just after the HTTP headers in content*/
char* http_header_strip(char *content);

#endif
