#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "event_epoll.h"

event_epoll_t *event_epoll_create(int max)
{
    event_epoll_t *h = (event_epoll_t*)malloc(sizeof(event_epoll_t));
    if(h == NULL)
        return NULL;
    memset(h, 0, sizeof(event_epoll_t));

    h->evs = (struct epoll_event*)malloc(max * sizeof(struct epoll_event));
    if(h->evs == NULL)
        goto ErrP;
    memset(h->evs, 0, max * sizeof(struct epoll_event));

    h->epfd = epoll_create(max);
    if(h->epfd == -1)
        goto ErrP;

    h->num = 0;
    h->max = max;
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

    return epoll_ctl(h->epfd, EPOLL_CTL_ADD, fd, &ev);
}

int event_epoll_mod(event_epoll_t *h, int fd, unsigned events, void *data)
{
    struct epoll_event ev;

    ev.data.ptr = data;
    ev.data.fd = fd;
    ev.data.u32 = 0;
    ev.data.u64 = 0;

    ev.events = events;

    return epoll_ctl(h->epfd, EPOLL_CTL_MOD, fd, &ev);
}

int event_epoll_del(event_epoll_t *h, int fd, unsigned events, void *data)
{
    struct epoll_event ev;

    ev.data.ptr = data;
    ev.data.fd = fd;
    ev.data.u32 = 0;
    ev.data.u64 = 0;

    ev.events = events;

    return epoll_ctl(h->epfd, EPOLL_CTL_DEL, fd, &ev);
}

int event_epoll_wait(event_epoll_t *h, int timeout)
{
    h->num = epoll_wait(h->epfd, h->evs, h->max, timeout);
    return h->num;
}

void event_epoll_proc(event_epoll_t *h, event_epoll_cb handler)
{
    int i;
    for(i = 0; i < h->num; i++)
    {
        if(handler) handler(h->evs[i].data.fd, h->evs[i].events, h->evs[i].data.ptr);
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
