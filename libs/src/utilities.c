/**
 * File that contains the implementation of som useful method used in the entire project
 * @author Gianmarco Petrocchi
 */

#include "../utilities.h"

long isNumber(const char* s){
    char* e = NULL;
    long val = strtol(s, &e, 0);
    if(e != NULL && *e == (char)0) return val;
    return -1;
}

int isDir(const char* filename){
    struct stat path;

    if(lstat(filename, &path) == -1){
        return -1;
    }

    if(path.st_mode & S_IFDIR) return 0;
    else return -1;
}

int isFile(const char* filename){
    struct stat path;

    if(lstat(filename, &path) == -1){
        return -1;
    }

    if(path.st_mode & S_IFREG) return 0;
    else return -1;
}

ssize_t readn(int fd, void* ptr, size_t n){
    size_t nleft;
    ssize_t nread;

    nleft = n;
    while(nleft > 0){
        if((nread = read(fd, ptr, nleft)) < 0){
            if(nleft == n) return -1;
            else break;
        }else if(nread == 0) break;
        nleft -= nread;
        ptr += nread;
    }
    return (n - nleft);
}

ssize_t writen(int fd, void* ptr, size_t n){
    size_t nleft;
    ssize_t nwritten;

    nleft = n;
    while(nleft > 0){
        if((nwritten = write(fd, ptr, nleft)) < 0){
            if(nleft == n) return -1;
            else break;
        }else if(nwritten == 0) break;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return (n - nleft);
}

int msleep(long msec){
    struct timespec ts;
    int res;

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do{
        res = nanosleep(&ts, &ts);
    }while(res && errno == EINTR);

    return res;
}