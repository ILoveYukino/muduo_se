add_executable(blockqueue_test blockqueue.cc)
target_link_libraries(blockqueue_test muduo_se_base)

add_executable(rlog_test rlog.cc)
target_link_libraries(rlog_test muduo_se_base)

add_executable(singleton_test singleton.cc)
target_link_libraries(singleton_test muduo_se_base)

add_executable(threadpool_test threadpool.cc)
target_link_libraries(threadpool_test muduo_se_base)

add_executable(timestamp_test timestamp.cc)
target_link_libraries(timestamp_test muduo_se_base)

add_executable(EventLoop_test EventLoop.cc)
target_link_libraries(EventLoop_test muduo_se_net)

add_executable(PollPoller_test PollPoller.cc)
target_link_libraries(PollPoller_test muduo_se_net)

add_executable(timequeue_test timequeue.cc)
target_link_libraries(timequeue_test muduo_se_net)

add_executable(eventloopthread_test eventloopthread.cc)
target_link_libraries(eventloopthread_test muduo_se_net)

add_executable(socket_test socket.cc)
target_link_libraries(socket_test muduo_se_net)

add_executable(socket_test2 socket2.cc)
target_link_libraries(socket_test2 muduo_se_net)

add_executable(socket_test3 socket3.cc)
target_link_libraries(socket_test3 muduo_se_net)

add_executable(eventpool eventpool.cc)
target_link_libraries(eventpool muduo_se_net)

add_executable(HttpServer_test HttpServer_test.cc)
target_link_libraries(HttpServer_test muduo_se_net)