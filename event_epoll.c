#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "event_epoll.h"

event_epoll_t *event_epoll_create(char *name, int max)
{
    event_epoll_t *h = (event_epoll_t*)malloc(sizeof(event_epoll_t) + strlen(name));
    if(h == NULL)
    {
        fprintf(stderr, "[%s:%d] %s malloc failed - %d:%s\n", __FILE__, __LINE__, name, errno, strerror(errno));
        goto ErrP;
    }
    memset(h, 0, sizeof(event_epoll_t) + strlen(name));

    h->evs = (struct epoll_event*)malloc(max * sizeof(struct epoll_event));
    if(h->evs == NULL)
    {
        fprintf(stderr, "[%s:%d] %s malloc failed - %d:%s\n", __FILE__, __LINE__, name, errno, strerror(errno));
        goto ErrP;
    }
    memset(h->evs, 0, max * sizeof(struct epoll_event));

    h->epfd = epoll_create(max);
    if(h->epfd == -1)
    {
        fprintf(stderr, "[%s:%d] %s epoll_create failed - %d:%s\n", __FILE__, __LINE__, name, errno, strerror(errno));
        goto ErrP;
    }

    h->num = 0;
    h->max = max;
    strcpy(h->name, name);
    return h;
ErrP:
    if(h)
    {
        if(h->evs)
            free(h->evs);
        if(h->epfd > 0)
            close(h->epfd);
        free(h);
    }
    return NULL;
}

int event_epoll_add(event_epoll_t *h, int fd, unsigned events, void *data)
{
    struct epoll_event ev;

    ev.data.ptr = data;
    ev.data.fd = fd;
    ev.data.u32 = 0;
    ev.data.u64 = 0;

    ev.events = events;

    if(epoll_ctl(h->epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        fprintf(stderr, "[%s:%d] %s epoll_ctl EPOLL_CTL_ADD failed - %d:%s\n", __FILE__, __LINE__, h->name, errno, strerror(errno));
        return -1;
    }
    return 0;
}

int event_epoll_mod(event_epoll_t *h, int fd, unsigned events, void *data)
{
    struct epoll_event ev;

    ev.data.ptr = data;
    ev.data.fd = fd;
    ev.data.u32 = 0;
    ev.data.u64 = 0;

    ev.events = events;

    if(epoll_ctl(h->epfd, EPOLL_CTL_MOD, fd, &ev) == -1)
    {
        fprintf(stderr, "[%s:%d] %s epoll_ctl EPOLL_CTL_MOD failed - %d:%s\n", __FILE__, __LINE__, h->name, errno, strerror(errno));
        return -1;
    }
    return 0;
}

int event_epoll_del(event_epoll_t *h, int fd, unsigned events, void *data)
{
    struct epoll_event ev;

    ev.data.ptr = data;
    ev.data.fd = fd;
    ev.data.u32 = 0;
    ev.data.u64 = 0;

    ev.events = events;

    if(epoll_ctl(h->epfd, EPOLL_CTL_DEL, fd, &ev) == -1)
    {
        fprintf(stderr, "[%s:%d] %s epoll_ctl EPOLL_CTL_DEL failed - %d:%s\n", __FILE__, __LINE__, h->name, errno, strerror(errno));
        return -1;
    }
    return 0;
}

int event_epoll_wait(event_epoll_t *h, int timeout)
{
    h->num = epoll_wait(h->epfd, h->evs, h->max, timeout);
    if(h->num == -1)
    {
        fprintf(stderr, "[%s:%d] %s epoll_wait failed - %d:%s\n", __FILE__, __LINE__, h->name, errno, strerror(errno));
        return -1;
    }
    return h->num;
}

void event_epoll_proc(event_epoll_t *h, event_epoll_cb handler)
{
    int i;
    for(i = 0; i < h->num; i++)
    {
        if(handler == NULL)
        {
            fprintf(stderr, "[%s:%d] %s epoll handler is %p\n", __FILE__, __LINE__, h->name, handler);
            break;
        }
        handler(h->evs[i].data.fd, h->evs[i].events, h->evs[i].data.ptr);
    }
}

void event_epoll_destroy(event_epoll_t *h)
{
    if(h)
    {
        if(h->evs)
            free(h->evs);
        if(h->epfd > 0)
            close(h->epfd);
        free(h);
    }
}
