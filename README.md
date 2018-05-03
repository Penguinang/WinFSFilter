# WinFSFilter
A simple FS filter,which can intercept IO operation and decide whether execute it,by which you can protect your data against change.

# Before Use
By now, we cant sign for this driver, which means if you want to try it, you must enable windows testsigning mode by cmd command `bcdedit /set testsigning on`with admin authority.

# Install
* Download and install [`WDK`](https://docs.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk) (need to install VS before this).
* Open these two project's .sln file with Visual Studio, and choose a proper target and config to compile.
* Write a config file at `C:\` with name `user.txt`, and format is as bellow
```
	userid
	password
	read_true
	write_true
	delete_true
```
All the postfix `true` can be replaced to `false`, which means you disallow this operation of the target directory(file).The first two line is of no use, you can change them to anything.
* Move `DriverSetup.exe` to the same directory of `WinFSFilter.sys`, and then use command `DriverSetup.exe install C:\ExampleDirectory\` to install and command `DriverSetup.exe uninstall` to uninstall.Make sure to specify a directory with a postfix `\` and file without postfix `\`.


