### 准备
1. 在网上找到了 `楚狂人` 写的一篇 [教程](http://file3.data.weipan.cn.wscdns.com/41553047/2471e7782fee3cedbaae7014a2e9c593beef9c18?ip=1524395329,222.20.30.243&ssig=NCVoxrsmQW&Expires=1524395929&KID=sae,l30zoo1wmz&fn=Windows%20%E6%96%87%E4%BB%B6%E7%B3%BB%E7%BB%9F%E8%BF%87%E6%BB%A4%E9%A9%B1%E5%8A%A8%E5%BC%80%E5%8F%91%E6%95%99%E7%A8%8B%EF%BC%88%E7%AC%AC%E4%BA%8C%E7%89%88%EF%BC%89.pdf&skiprd=2&se_ip_debug=222.20.30.243&corp=2&from=1221134)
2. 了解到作者当时所使用的`DDK`已经过去十几年了，现在已经发展为`WDK`，在微软官网下载[`WDK`](https://docs.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk)并安装
3. 	
	+ [`File System Filter Driver` 官方文档](https://docs.microsoft.com/zh-cn/windows-hardware/drivers/ifs/file-system-filter-drivers)
	+ [`WDK` 官方文档](https://docs.microsoft.com/en-us/windows-hardware/drivers/)

### 学习
* 似乎新的VS已经没有`FS Filter`的模板项目(Legacy)，只有`FS MiniFilter`，所以决定先看教程，再写`FS MiniFilter`
* 理解模板项目中的代码，并尝试编译
	+ 修改inf文件的`Class`和`ClassGuid`后编译成功
* 编译成功后，打开调试，需要两个机器，通过网络连接调试。可以考虑使用虚拟机。考虑使用编译后直接在本机安装的方法调试，需要使用到`文件I/O`操作来调试，但据了解，在`Driver`的代码中，有部分`C`的功能是不支持的，如`I/O`操作等，只能使用`NativeAPI`完成`I/O`操作，不过`NativeAPI`资料好像不多，不怎么好学。
* 编译成功后，试着在自己电脑上安装驱动，只有在`X64`模式下编译才能成功安装。需要数字签名
	+ `bcdedit /set nointegritychecks on` 无效
	+ `bcdedit /set testsigning on` 可以进入测试模式，可以不需要数字签名
直接右键`INF`文件即可，`CMD`中使用命令`sc start/stop/delete minifilter(驱动名称)`可以分别打开/关闭/卸载驱动。安装之后启动并试着用`DebugView`查看`DbgPrint`的消息，但是没找到。重启电脑后，无法进入登录界面，表现为出现蓝色窗口图标后就重启，在Linux中删除了`C:\\Windows\System32\drivers\minifilter.sys`及`C:\\Windows\INF\minifilter.INF`后仍不能正常启动。分析可能原因是
    + 之前为了使用`WinDbg`调试本机，启用了`Debug`，即命令`bcdedit -debug on`; -------------------------- 后来确定是这个原因
    + 重启电脑前没有卸载掉驱动，导致可能在电脑启动时对于`IRP`没有做出正确的处理;
* 选择重置电脑后，可以进入了
* 安装虚拟机，装 `Windows10` 专业版，在虚拟机上装`WDK`，运行`C:\Program Files (x86)\Windows Kits\10\Remote\x64\WDK Test Target Setup x64-x64_en-us.msi`。可参考 [文档1](https://docs.microsoft.com/zh-cn/windows-hardware/drivers/gettingstarted/provision-a-target-computer-wdk-8-1) , [文档2](https://docs.microsoft.com/zh-CN/windows-hardware/drivers/develop/deploying-a-driver-to-a-test-computer)
	+ 设置虚拟机端口转发.VirtualBox中，设置为192.168.184.1(VB网卡IP) : 50657 ----> 10.0.2.15(虚拟机子系统IP) : 50657 , 192.168.184.1(VB网卡IP) : 50005 ----> 10.0.2.15(虚拟机子系统IP) : 50005 
	+ 配置VS中目标机器，目标机IP为VB网卡IP `192.168.184.1`
	+ 在VS `Project Propertity/Driver Install/Devloypment`目录下选择目标机为刚才的机子
	+ VS调试选择KernelMode，成功部署
* 成功部署后，点击调试，发现不能输出调试信息。
* 直接将驱动和INF文件拷贝到虚拟机安装，使用`DBGView`查看调试信息（需要用管理员打开），勾选`Enable Kernel Verbose Output`，在虚拟机命令行（管理员）使用`SC`启动驱动，看到还是没有调试信息。查找之后得知，`DBGPrint`有调试信息等级，可能是调试器将等级太低的信息过滤了，所以在代码中直接使用`DBGPrintEX(DPFLTR_DEFAULT_ID,DPFLTR_ERROR_LEVEL,"Message\n")`来控制信息登记为错误级输出，看到`DBGView`成功输出了信息。
* 在`Github`上找到了微软官方账号给出的驱动样例，拷贝下来研究。
* 学习`passThrough`的代码。如何拦截`IO`请求？使用[`PreOPCallBack`](https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/content/fltkernel/nc-fltkernel-pflt_pre_operation_callback)函数的返回值。试着拦截了写的操作，成功了。发现在这里测试的时候，最好选用`cmd`而不是图形界面，图形界面会涉及到很多其他的系统触发的`IO`，导致调试信息太多，找不到真正目标的调试信息。
* 对驱动中`context`的作用理解：
	+ `context`记录了文件的相关信息
	+ `context`是开发者自己定义，自己创建，自己赋值，自己使用的。
	+ 在不同的回调之间传递参数，除了函数限定的参数之外，还有就是可以用`context`来传参数。
* 如何限定特定的目录？
	+ 暂时知道的可行做法是使用`FltGetFileNameInformation`获取文件的路径(以设备出发而不是目录), 自己进行筛选，判断是否对该文件需要进行过滤


### 编写
* 文件读取，拦截`IRP_MJ_READ`
* 文件写入，拦截`IRP_MJ_WRITE`
* 文件回收，拦截`IRP_MJ_SET_INFORMATION`中`FileDispositionInformation`,`FileDispositionInformationEx`,`FileRenameInformation`三种操作
* 关于`ACCESS_MASK`过滤`IRP_MJ_CREATE`时候遇到的问题(`CREATE`相当于`c`的`fopen`):
	+ 在`IRP_MJ_CREATE`的请求中，参数的一个成员(`Data->Iopb->Parameters.Create.SecurityContext->DesiredAccess`)是文件的打开权限，想到可以从此监控文件
	+ 又想到了 WIN 中另一个 API `CreateFile`，在这个函数中也有叫做`dwDesiredAccess`的参数，这两个参数是不是一样的？试着这样理解，并且实践
	+ 实践中，发现使用`GENERIC_READ`不能正确检查出`CREATE`请求中包含`READ`的操作，`GENERIC_WRITE`也是。(`GENERIC_READ`和`GENERIC_WRITE`是`dwDesiredAccess`的取值之一，是windows中使用`CreateFile`打开文件使用的权限标志)
	+ 自己使用`CreateFile`指定权限来打开文件，使用驱动监测，得到了实际获得的权限值，与`GENERIC_READ`对比，确实不同。
	+ google 发现有人说这两个参数不是一回事，换句话说，`GENERIC_READ`并不是真正的`ACCESS_MASK`,系统最终会将`GENERIC_READ`映射为`FILE_GENERIC_READ`，所以最终驱动看到的是`FILE_GENERIC_READ`。
	+ 换用`FILE_GENERIC_READ`检查读取文件的操作，成功。
	+ 发现使用写模式打开的文件还是可以截获到`FILE_GENERIC_READ`。打开`FILE_GENERIC_READ`的定义看到
	```C
		#define FILE_GENERIC_READ         (STANDARD_RIGHTS_READ     |\
		                                   FILE_READ_DATA           |\
		                                   FILE_READ_ATTRIBUTES     |\
		                                   FILE_READ_EA             |\
		                                   SYNCHRONIZE)
    ```	
	,原来这个权限中还包含了对`ATTRIBUTES`等其他内容读取，而打开文件写入时是会读取这些额外信息的。所以拦截文件读取时，只拦截`FILE_READ_DATA`即可。
* 文件直接删除，其本质是打开文件并且使用`DELETE_ON_CLOSE`参数，所以只要在`IRP_MJ_CREATE`里面检测包含`DELETE_ON_CLOSE`的操作并拦截就可以了。
* 使用`zwCreateFile`打开配置文件，读取配置。
* 对于目标路径的指定，决定在安装驱动时候指定，然后使用一个额外的配置文件记录下来，在每次启动驱动(开机或安装之后)读取配置文案进的内容，加载目标路径。
* 关于对目标的匹配检测，可以约定对目录文件，安装时最后一`\`结尾;对普通文件，没有`\`结尾。驱动中对目标是文件的，进行完全匹配，对文件夹进行包含匹配
* 在这里遇到的问题：
	+ 在安装程序中，用户如何输入一个`wchar`的字符串？并且需要分离出前面两个字符(盘符)，转化为`ASCII`字符，获取设备名，在添加回字符串？暂时直接默认没有中文输入，然后手动将进行转换
* 编译之后，拿到`Win7`发现蓝屏，google之后找到原因是编译目标平台选了`Win10`，改为`Win7`之后，没有蓝屏了
* 试图让驱动开机启动
	+ 一开始设置了`StartType`为`0x0`,含义是`SERVICE_BOOT_START`
	+ 发现驱动没有正确加载，加了调试输出后发现，是没有正确打开配置文件
	+ 猜想可能是驱动加载过早，文件系统还不能正常工作。于是修改为`0x1`,即`SERVICE_SYSTEM_START`,可以正常启动了
* 试着写一个`dll`文件，完成打开图形弹窗提醒用户操作受限的功能。发现隐式调用dll使用通常的方法(在vs中为驱动项目添加一个dll引用项目)不能正常编译，而显示调用需要的函数`GetProcAddress`在用户态的`Windows.h`文件中。了解之后得知，在dll中有用户态的代码，所以编译出错，说找不到定义。
* 为驱动警告添加一个弹窗。涉及到核态对用户态代码的调用，可能用到的几个方法
	+ `Ioctl`
	+ `RaiseHardError`
	+ 写一个用户态的后台服务程序，通过与这个服务程序通信，打开弹窗
