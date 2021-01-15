set(net_srcs
    EventLoop.cc
    Channel.cc
    Poller.cc
    PollPoller.cc
    EpollPoller.cc
    TimerQueue.cc
    EventLoopThread.cc
    Socket.cc
    IpAdress.cc
    Accept.cc
    TcpServer.cc
    TcpConnect.cc
)

add_library(muduo_se_net ${net_srcs})
target_link_libraries(muduo_se_net muduo_se_base)

install(TARGETS muduo_se_net DESTINATION lib)
file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/net)