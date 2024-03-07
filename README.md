## 注意

### PC微信hook源码，仅供学习，请不要用于商业、违法途径，本人不对此源码造成的违法负责！

当前项目适配的微信版本为3.9.2.23


本项目为Demo版本，仅供学习参考。

## 更新说明

### 提供3.9.2.23微信消息钩子
任何微信消息皆可接收
下载地址：https://github.com/Roperl/wxMsgHook_HTTP/releases/tag/3.9.2

## 使用教程

创建端口为8888的HTTP服务器,例如Apache、Nginx等,或者用代码自己写一个HTTP服务器。

将release下载下来后，确保ConsoleApplication1.exe和getMsgDllDemo.dll在同一目录，双击运行ConsoleApplication1.exe即刻。

当微信有消息（朋友消息、群消息、朋友圈消息、公众号消息...）或发送微信消息时钩子程序会发送请求到http://127.0.0.1:8888/recvmsg

接口详情为
GET http://127.0.0.1:8888/recvmsg


|QueryParams key|解释|
|-|-|
|msgId|消息唯一id|
|msgForm|消息来源|
|msg|消息内容|
|wxqunSender|微信群是谁发送的消息|


## 商务合作，请捐献且备注联系信息
![微信二维码](https://mp-36d1c2f3-2ce9-4f84-9090-887a579e6782.cdn.bspapp.com/微信图片_20240307114331.jpg)
