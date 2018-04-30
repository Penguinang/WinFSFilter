准备
在网上找到了 楚狂人 写的一篇 教程
了解到作者当时所使用的DDK已经过去十几年了，现在已经发展为WDK，在微软官网下载WDK并安装
File System Filter Driver 官方文档
WDK 官方文档
学习
似乎新的VS已经没有FS Filter的模板项目(Legacy)，只有FS MiniFilter，所以决定先看教程，再写FS MiniFilter
理解模板项目中的代码，并尝试编译
修改inf文件的Class和ClassGuid后编译成功
编译成功后，打开调试，需要两个机器，通过网络连接调试。可以考虑使用虚拟机。考虑使用编译后直接在本机安装的方法调试，需要使用到文件I/O操作来调试，但据了解，在Driver的代码中，有部分C的功能是不支持的，如I/O操作等，只能使用NativeAPI完成I/O操作，不过NativeAPI资料好像不多，不怎么好学。
编译成功后，试着在自己电脑上安装驱动，只有在X64模式下编译才能成功安装。需要数字签名
bcdedit /set nointegritychecks on 无效
bcdedit /set testsigning on 可以进入测试模式，可以不需要数字签名 直接右键INF文件即可，CMD中使用命令sc start/stop/delete minifilter(驱动名称)可以分别打开/关闭/卸载驱动。安装之后启动并试着用DebugView查看DbgPrint的消息，但是没找到。重启电脑后，无法进入登录界面，表现为出现蓝色窗口图标后就重启，在Linux中删除了C:\\Windows\System32\drivers\minifilter.sys及C:\\Windows\INF\minifilter.INF后仍不能正常启动。分析可能原因是
之前为了使用WinDbg调试本机，启用了Debug，即命令bcdedit -debug on; --------------------------- 后来确定是这个原因
重启电脑前没有卸载掉驱动，导致可能在电脑启动时对于IRP没有做出正确的处理;
选择重置电脑后，可以进入了
安装虚拟机，装 Windows10 专业版，在虚拟机上装WDK，运行C:\Program Files (x86)\Windows Kits\10\Remote\x64\WDK Test Target Setup x64-x64_en-us.msi。可参考 文档1 , 文档2
设置虚拟机端口转发.VirtualBox中，设置为192.168.184.1(VB网卡IP) : 50657 ----> 10.0.2.15(虚拟机子系统IP) : 50657 , 192.168.184.1(VB网卡IP) : 50005 ----> 10.0.2.15(虚拟机子系统IP) : 50005
配置VS中目标机器，目标机IP为VB网卡IP 192.168.184.1
在VS Project Propertity/Driver Install/Devloypment目录下选择目标机为刚才的机子
VS调试选择KernelMode，成功部署
成功部署后，点击调试，发现不能输出调试信息。
直接将驱动和INF文件拷贝到虚拟机安装，使用DBGView查看调试信息（需要用管理员打开），勾选Enable Kernel Verbose Output，在虚拟机命令行（管理员）使用SC启动驱动，看到还是没有调试信息。查找之后得知，DBGPrint有调试信息等级，可能是调试器将等级太低的信息过滤了，所以在代码中直接使用DBGPrintEX(DPFLTR_DEFAULT_ID,DPFLTR_ERROR_LEVEL,"Message\n")来控制信息登记为错误级输出，看到DBGView成功输出了信息。
在Github上找到了微软官方账号给出的驱动样例，拷贝下来研究。
学习passThrough的代码。如何拦截IO请求？使用PreOPCallBack函数的返回值。试着拦截了写的操作，成功了。发现在这里测试的时候，最好选用cmd而不是图形界面，图形界面会涉及到很多其他的系统触发的IO，导致调试信息太多，找不到真正目标的调试信息。
对驱动中context的作用理解：
context记录了文件的相关信息
context是开发者自己定义，自己创建，自己赋值，自己使用的。
在不同的回调之间传递参数，除了函数限定的参数之外，还有就是可以用context来传参数。
如何限定特定的目录？
暂时知道的可行做法是使用FltGetFileNameInformation获取文件的路径(以设备出发而不是目录), 自己进行筛选，判断是否对该文件需要进行过滤
编写
文件读取，拦截IRP_MJ_READ
文件写入，拦截IRP_MJ_WRITE
文件回收，拦截IRP_MJ_SET_INFORMATION中FileDispositionInformation,FileDispositionInformationEx,FileRenameInformation三种操作
关于ACCESS_MASK过滤IRP_MJ_CREATE时候遇到的问题(CREATE相当于c的fopen):
在IRP_MJ_CREATE的请求中，参数的一个成员(Data->Iopb->Parameters.Create.SecurityContext->DesiredAccess)是文件的打开权限，想到可以从此监控文件
又想到了 WIN 中另一个 API CreateFile，在这个函数中也有叫做dwDesiredAccess的参数，这两个参数是不是一样的？试着这样理解，并且实践
实践中，发现使用GENERIC_READ不能正确检查出CREATE请求中包含READ的操作，GENERIC_WRITE也是。(GENERIC_READ和GENERIC_WRITE是dwDesiredAccess的取值之一，是windows中使用CreateFile打开文件使用的权限标志)
自己使用CreateFile指定权限来打开文件，使用驱动监测，得到了实际获得的权限值，与GENERIC_READ对比，确实不同。
google 发现有人说这两个参数不是一回事，换句话说，GENERIC_READ并不是真正的ACCESS_MASK,系统最终会将GENERIC_READ映射为FILE_GENERIC_READ，所以最终驱动看到的是FILE_GENERIC_READ。
换用FILE_GENERIC_READ检查读取文件的操作，成功。
发现使用写模式打开的文件还是可以截获到FILE_GENERIC_READ。打开FILE_GENERIC_READ的定义看到
 	#define FILE_GENERIC_READ         (STANDARD_RIGHTS_READ     |\
 	                                   FILE_READ_DATA           |\
 	                                   FILE_READ_ATTRIBUTES     |\
 	                                   FILE_READ_EA             |\
 	                                   SYNCHRONIZE)	
,原来这个权限中还包含了对ATTRIBUTES等其他内容读取，而打开文件写入时是会读取这些额外信息的。所以拦截文件读取时，只拦截FILE_READ_DATA即可。
文件直接删除，其本质是打开文件并且使用DELETE_ON_CLOSE参数，所以只要在IRP_MJ_CREATE里面检测包含DELETE_ON_CLOSE的操作并拦截就可以了。