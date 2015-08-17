/***************************************************************************
 * 
 * Copyright (c) 2015 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file server.cpp
 * @author huzejin(com@baidu.com)
 * @date 2015/06/29 13:19:59
 * @brief 
 *  
 **/
#include "utility.h"

int main(int argc, char *argv[])
{
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    int listener = socket(PF_INET, SOCK_STREAM, 0);
    if (listener < 0)
    {
        perror("listener");
        exit(-1);
        printf("listen socket created\n");
    }

    if(bind(listener, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("bind error");
        exit(-1);
    }

    int ret = listen(listener, 5);
    if(ret < 0)
    {
        perror("listen error");
        exit(-1);
    }
    printf("Start to listen: %s\n", SERVER_IP);

    int epfd = epoll_create(EPOLL_SIZE);
    if (epfd < 0)
    {
        perror("epfd error");
        exit(-1);
    }
    printf("epoll created, epollfd = %d\n", epfd);
    static struct epoll_event events[EPOLL_SIZE];
    addfd(epfd, listener, true);

    while (1)
    {
        int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
        if (epoll_events_count < 0)
        {
            perror("epoll failure");
            break;
        }

        printf("epoll_events_count = %d\n", epoll_events_count);

        for (int i = 0; i < epoll_events_count; ++i)
        {
            int sockfd = events[i].data.fd;

            if (sockfd == listener)
            {
                struct sockaddr_in client_address;
                socklen_t client_addrLength = sizeof(struct sockaddr_in);
                int clientfd = accept(listener, (struct sockaddr*)&client_address, &client_addrLength);

                printf("client connection from: %s: %d(IP : port), clientfd = %d\n", inet_ntoa(client_address.sin_addr),
                                                                                     ntohs(client_address.sin_port),
                                                                                     clientfd);
                addfd(epfd, clientfd, true);
                
                clients_list.push_back(clientfd);
                printf("Add new cluentfd = %d to epoll\n", clientfd);
                printf("Now there are %d clients int the chat room", (int)clients_list.size());

                printf("welcome message\n");
                char message[BUF_SIZE];
                bzero(message, BUF_SIZE);
                sprintf(message, SERVER_WELCOME, clientfd);
                int ret = send(clientfd, message, BUF_SIZE, 0);
                if(ret < 0){perror("send error"); exit(-1);}
            }
            else
            {
                int ret = sendBroadcastmessage(sockfd);
                if(ret < 0){perror("error"); exit(-1);}
            }
        }
    }
    close(listener);
    close(epfd);
    return 0;
}




















/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
