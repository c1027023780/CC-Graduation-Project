功能概要：KL36_Update
版权所有：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
版本更新：V1.0，陈成，2018年7月11日

硬件说明：

终端软件更新框架包含两个程序：Bios程序和用户程序，其中Bios程序负责硬件驱动初始化、软件更新并提供一系列函数接口；
用户程序主要负责实现具体的用户功能需求，需要注意的是Systick中断和Uart2中断已被Bios程序占用，用户程序不允许使用。

用户程序说明：
本工程主要是用户之定义程序，只需再运行时注意几点细节，具体的程序功能由用户实现。
（1）函数指针初始化，读取Bios程序写入Flash中的各个驱动函数地址并赋值给各个函数指针。即可通过这些函数指针调用驱动程序。
本框架提供了funPointerInit函数来实现该功能。
（2）复制中断向量表至RAM。为实现Bios程序于用户程序共用一个中断向量表，在用户程序中将Bios程序的中断向量表复制进RAM中，
并设置向量表偏移寄存器指向该RAM地址。如此，发生Systick中断和Uart2中断时会执行Bios中定义的中断服务例程。本框架提供了
cpy_nvic函数来实现该功能。
（3）其他中断函数重定义。当在用户程序中使用其他中断是，需对其中断服务例程重定位，本框架提供了setVector函数来实现该功能。
