#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

void dealwithrequests(int fd);
void sigpipe_handler(int sig);
int parse_uri(char *url, char *hostname, char *uri, char *port);
void read_requesthdrs(rio_t *rp);
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg);

int main(int argc, char* argv[])
{
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    Signal(SIGPIPE, sigpipe_handler);

    /* Check command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); //line:netp:tiny:accept
        Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, 
                    port, MAXLINE, 0);
        printf("Accepted connection from (%s, %s)\n", hostname, port);
        dealwithrequests(connfd);
        // do something
        Close(connfd);                                            //line:netp:tiny:close
    }
    return 0;
}

void dealwithrequests(int fd) 
{
    rio_t riobuf;
    char buf[MAXLINE], method[MAXLINE], url[MAXLINE], version[MAXLINE];
    char port[10];
    Rio_readinitb(&riobuf, fd);
    if (!Rio_readlineb(&riobuf, buf, MAXLINE)) 
        return;
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, url, version); 
    

    // to be revised: add POST
    if (!strcasecmp(method, "GET")) {
        clienterror(fd, method, "501", "Not Implemented",
                    "Proxy does not implement this method");
        return;
    }

    read_requesthdrs(&riobuf);

    char hostname[MAXLINE], uri[MAXLINE];

    if (!parse_uri(url, hostname, uri, port)) {
        clienterror(fd, method, "404", "Not found", "Cannot parse this url");
        return;
    }

    int connectfd = Open_clientfd(hostname, port);

    rio_t criobuf;
    char cbuf[MAXLINE];
    sprintf(cbuf, "%s %s %s\r\n", method, uri, version);

    if (!strstr(buf, "Host"))
        sprintf(cbuf, "Host: %s\r\n", hostname);

    sprintf(cbuf, "%s", user_agent_hdr);
    sprintf(cbuf, "Connection: close\r\nProxy-Connection: close\r\n");
    Rio_writen(connectfd, cbuf, MAXLINE * 3);

    char crbuf[MAXLINE];

    Rio_readinitb(&criobuf, connectfd);
    Rio_readlineb(&criobuf, crbuf, MAXLINE);

    Rio_writen(fd, crbuf, MAXLINE);

    Close(connectfd);
}

void read_requesthdrs(rio_t *rp) 
{
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
    while(strcmp(buf, "\r\n")) {          //line:netp:readhdrs:checkterm
        Rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
    }
    return;
}

int find(char *str, int c) {
    for (int i = 0; ; i++) {
        if (str[i] == '\0') return -1;
        if (str[i] == c) return i;
    }
}

int parse_uri(char *url, char *hostname, char *uri, char *port) {
    /*  http://www.cmu.edu:8080/hub/index.html */

    int idx_1 = find(url, '/');

    if (-1 == idx_1) {
        return 0;
    }
    int len = strlen(url) - idx_1 - 2;

    char tmp[MAXLINE];
    memset(tmp, 0, sizeof(tmp));

    strcpy(tmp, url);
    strcpy(url, tmp + idx_1 + 2);
    url[len] = '\0';
    
    int idx_2 = find(url, ':');
    int idx_3 = find(url, '/');

    if (-1 == idx_2) {
        strcpy(port, "80");
        strcpy(hostname, url);
        if (idx_3 != -1) 
            hostname[idx_3] = '\0';
    }
    else {
        strcpy(port, url + idx_2 + 1);
        for (int i = 0; ; i++) {
            if (port[i] == '/') {
                port[i] = '\0';
                break;
            }
        }
        strcpy(hostname, url);
        hostname[idx_2] = '\0';
    }
    if (-1 != idx_3) {
        strcpy(uri, url + idx_3);
    } else {
        strcpy(uri, "/");
    }
    return 1;
}

void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE];

    /* Print the HTTP response headers */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Print the HTTP response body */
    sprintf(buf, "<html><title>Tiny Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor=""ffffff"">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>The Tiny Web server</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}

void sigpipe_handler(int sig) {
    printf("SIGPIPE handled\n");
    return ;
}