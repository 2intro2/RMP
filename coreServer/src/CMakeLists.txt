#定义SRC_LIST变量 包含该目录下所有的源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./model MODEL_LIST)

#由指定源文件生成的可执行程序
add_executable(coreServer ${SRC_LIST} ${DB_LIST} ${MODEL_LIST})

#指定可执行文件链接时需要的库文件
target_link_libraries(coreServer mysqlclient pthread ssl crypto)