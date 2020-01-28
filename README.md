# Schedule  ![](https://img.shields.io/badge/license-MIT-blue) ![](https://img.shields.io/badge/PRs-welcome-green) ![](https://img.shields.io/badge/issues-welcome-green)

+ A simple, easy-to-use command-line tool for task management.

## Folder structure
```c
.
├── LICENSE
├── README.md
├── schedule.c     // source code
├── schedule.h    // types and includes
└── utils.c       // some functions for testing here
```

## Install
1. clone this repo.
```bash
git clone https://github.com/Derek-Gcx/schedule.git
```
2. compile.
```bash
cd schedule && mkdir build
gcc schedule.c -o ./build/schedule
```
3. move
```bash
sudo mv ./build/schedule /bin/
```
4. now you can use schedule as a command-line tool !
```bash
schedule run "echo "hello world!""
```

## What can ```schedule``` do?
#### run
+ arrange a task and execute it either forestage or backstage.
+ **options**
  + ```-n/--name <name>```: Specify the task name, default to ```run```
  + ```-d/--dest <file>```: Redirect the output to file specified by ```<file>```
  + ```-u/--user <name>```: Specify the user, default to the current user
  + ```-L/--latency <int>```: Postpone the execution for ```<int>``` seconds
  + ```-b/--back```: Execute the programs backstage. When the option ```-b/--back``` is given, option ```-d/--dest``` must be specified as well.

+ **example**
```bash
schedule run ./exe1 ./exe2 ./exe3 -b -d /tmp/res.txt -n my_workflow
```

#### all
+ display all the running tasks and their names and pids.
+ **options**
  + no options.
+ **example**
```bash
schedule all
```

#### terminate
+ terminate a task which you started before.
+ **You have to specify the name of the task you'd like to terminate. If you're not sure about the name, you can run ```schedule all``` to get it.**
+ **example**
```bash
schedule all
schedule terminate -n some_task
```

## 遗留问题、bug
+ 在运行```schedule run exe1```时，如果使用```ctrl+c```，尽管该进程已被中断，该进程的信息仍然会遗留在```/tmp/schedule_pool```文件中。
+ 异常处理部分较混乱，在execute中子进程部分未进行异常判断，缺失非法操作的捕捉和处理程序。
+ 使用逻辑和主流命令行工具相比有所出入。我最近也在阅读一些命令行工具的源码，有时间尽量重构。
+ --help选项展现的帮助信息不完整。
+ 在后台运行功能中，多进程的编写方式和输入输出重定向比较幼稚。目前采用的是fork+exec+dop2实现子进程调用外部命令并重定向的功能，或许有其他实现方式，比如通过popen建立管道并获取管道进程pid。需要进一步拓展知识。
+ 可以考虑加入更多的功能和选项，比如超时自动终止进程。

## 废话
+ 之前给神经网络调参并进行迭代对弈的时候发现每次都要在命令行运行一系列程序很烦，再加上最近武汉肺炎闲得无聊，就写了一个任务管理的命令行小工具。
+ 由于自己的水平实在有限，请一定要**谨慎**使用。