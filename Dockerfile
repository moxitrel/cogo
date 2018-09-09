# Parser directives: 解析器指令必须位于最顶端，约定小写，包括一个空白行
# escape=\    # 指定转义字符, 缺省 \
# escape=`    # 适合 windows

# INSTRUCTION args: INSTRUCTION 不区分大小写，建议大写. Run in order
#
# 继承, 指定Base Image, 必需是第一条指令, 多次FROM生成多个映像
FROM  <image>           # 默认 latest
FROM  <image>:<tag>
FROM  <image>@<digest>
FROM  <image> AS <name> # alias, use <name> to refer <image>
# an empty image, no-op
FROM  scratch
# 5MB linux base on musl libc, busybox
FROM  alpine

# 被当作base image时，被后代FROM执行
ONBUILD [INSTRUCTION args]

# 添加元数据
# 每个 LABEL 指令产生一个新层, 建议合并
LABEL   <key>=<value> \
        "com.example.vendor"="ACME Incorporated" \
        version="1.0" \

# 环境变量, $var, ${var}
# ${var:-word}: var未定义，则返回 word
# ${var:+word}: var以定义，则返回 word
#
# 支持环境变量指令：
#   ADD
#   COPY
#   ENV
#   EXPOSE
#   LABEL
#   USER
#   WORKDIR
#   VOLUME
#   STOPSIGNAL
#
#   ONBUILD 支持指令
ENV  <key>=<val>, ...
ENV   var   val

# 环境变量, 通过 docker build --build-arg <name> = <value>,
# 若 在FROM之前，只能被FROM使用
ARG <name>[=<default-value>]
ARG var=val

# 执行命令的shell
# Linux:  默认 ["/bin/sh","-c"]
# Windows:默认 ["cmd","/S","/C"]
SHELL ["shell", "opt", ...]

# 设置容器 当前 工作目录，可多次使用
WORKDIR /path/to/workdir

# 执行容器命令，并提交结果，分层
RUN <command>                       # shell中运行，/bin/sh -c <command>
RUN ["cmd", "opt", "arg1", "arg2"]  # exec 形式

# 复制 本地文件 至 容器; UID, GID为0
# src: must inside the context
#      文件 | 远程文件URL:
#      目录: 仅复制目录中的内容，不复制目录本身
# dst: 创建若不存在
ADD <src>... <dst>
ADD ["<src>",... "<dst>"]
#COPY <src>... <dst>                 # src不能为 URL
#COPY ["<src>",... "<dst>"]

# run时，总是执行这个命令，只能有一个
ENTRYPOINT ["cmd", "opt", "arg"]
ENTRYPOINT   cmd    opt    arg      # 不会传入 CMD 或 run命令行参数

# 默认命令，或默认 entrypoint 参数，只能有一个CMD指令
CMD ["cmd", "opt", "arg1", "arg2"]  # 无entrypoint
CMD [       "opt", "arg1", "arg2"]  # 有ENTRYPOINT，作为默认参数
CMD   cmd    opt    arg1    arg2

# 声明 Listen 端口, 默认TCP
# e.g. 21, 80/tcp, 8080/udp
EXPOSE <port>[/<protocol>] ...

# 声明外部挂载: 数据共享、存储
VOLUME ["/data", ...]
VOLUME /var/log   /var/db

# 运行image时的用户名或UID
USER uid

STOPSIGNAL signal
