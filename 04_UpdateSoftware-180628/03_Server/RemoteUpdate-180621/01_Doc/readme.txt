功能概要：RemoteUpdate
版权所有：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
版本更新：V1.0，陈成，2018年6月22日

本程序需与KL36_Bootloader终端程序配合使用，才能实现串口程序更新。

注意事项：
【V1.0】
（1）添加Hex、Update类。Hex类提供读取Hex文件功能。Update类提供将Hex文件有效数据组装成更新帧功能。
（2）本工程目前有3个页面，frmMain为工程主窗体，frmFile为读取Hex文件功能页面，frmUart为串口连接、串口程序更新功能页面。