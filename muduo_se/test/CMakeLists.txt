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