set(base_srcs
    blockqueue.cc
    buffer.cc
    countdownlatch.cc
    rlog.cc
    singleton.cc
    threadpool.cc
    timestamp.cc
)

add_library(muduo_se_base ${base_srcs})
target_link_libraries(muduo_se_base pthread rt)

install(TARGETS muduo_se_base DESTINATION lib)
file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/base)
