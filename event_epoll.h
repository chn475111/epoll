/**
 * @brief epoll wrapper
 * @author lijk@infosec.com.cn
 * @version 0.0.1
 * @date 2017-6-13 15:42:32
 */
#ifndef __EVENT_EPOLL_H__
#define __EVENT_EPOLL_H__

#include <sys/epoll.h>

typedef struct event_epoll_s
{
    int epfd;
    int num;
    int max;
    struct epoll_event *evs;
    char name[1];
}event_epoll_t;

typedef void (*event_epoll_cb)(int fd, unsigned events, void *data);

#ifdef __cplusplus
extern "C" {
#endif

/// return - null: failed; non-null: succeed
event_epoll_t *event_epoll_create(char *name, int max);

/// return - -1: failed; 0: succeed
int event_epoll_add(event_epoll_t *h, int fd, unsigned events, void *data);

/// return - -1: failed; 0: succeed
int event_epoll_mod(event_epoll_t *h, int fd, unsigned events, void *data);

/// return - -1: failed; 0: succeed
int event_epoll_del(event_epoll_t *h, int fd, unsigned events, void *data);

/// return - -1: failed; 0: succeed
int event_epoll_wait(event_epoll_t *h, int timeout);

/// return - non-return
void event_epoll_proc(event_epoll_t *h, event_epoll_cb handler);

/// return - non-return
void event_epoll_destroy(event_epoll_t *h);

#ifdef __cplusplus
}
#endif

#endif
