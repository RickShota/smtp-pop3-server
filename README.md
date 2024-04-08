# smtp-pop3-server

基于 smtp-pop3 协议的远程控制系统

如果对你有帮助请不要吝啬 star 一下吧~

## 开发环境

- Ubuntu 20.04.6
- MinGW gcc 9.4.0
- outlook 6.0

## makefile 命令详解

- `make` 编译代码，生成的可执行文件在 build 里
- `make smtp` 编译并运行 smtp 服务器
- `make pop` 编译并运行 pop3 服务器
- `make debug` 编译并执行 gdb 调试
