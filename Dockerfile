FROM  alpine

# make build environment
RUN ["apk", "update"]
RUN ["apk", "add", "clang", "binutils", "cmake", "make", "gcc", "g++"]


# 设置容器 当前 工作目录，可多次使用
WORKDIR /tmp

# 复制 本地文件 至 容器; UID, GID为0
# src: must inside the context
#      文件 | 远程文件URL:
#      目录: 仅复制目录中的内容，不复制目录本身
# dst: 创建若不存在
#ADD [".", "."]


# 默认命令，或默认 entrypoint 参数，只能有一个CMD指令
#CMD ["/home/tool/build.sh"]

VOLUME ["/home"]
