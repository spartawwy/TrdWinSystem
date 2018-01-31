 //
 经测试行情接口 一次最多获取 80 个股票行情  ,所以最多需要40次访问

 当前A股总数达到3300多家; 是否支持 多线程同时获取股票行情(测试后表明这是支持的),是否支持连续4次调用接口, 这些需要测试.
 如支持,计划 用
 
 * 启动后 load 数据库里所有 存在关联任务的 股票,对这些股票(简称A)进行 分批询价,
   每次取80个, 且在线程
 * 当天11点前,不对加载的A进行动态调整.
 * 用户动态添加的新股(简称B)追加到 专门负责动态股票池的这些task_pool
 * 定时(11点)重新reload数据库任务股票池,进而重新分批到不同的task_pool

 -----------20180201

 计划: *独立多个 ticker进程, 根据股票代码所在段,将股票分给不同的ticker
      *各个ticker管理相关 stockTask, stockTask触发任务后发给trade_server
	    由trade_server发送交易指令
	  *fronter接收trade_server交易结果转发给客户端

	  *各ticker主动连接 trade_server
	  *fronter 连接trade_server
	  *fronter 连接各ticker

	  *fronter 持有 userinfo, accounters等信息
	  *fronter 将接收的task增减配置等任务 并发送到 ticker
	  *task增减配置等任务的数据库保存由recorder负责
	  *trade_server 完成任务后 发信息到 recorder
	  *recorder主动连接 fronter和trader_server
