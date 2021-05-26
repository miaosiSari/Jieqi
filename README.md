# 揭棋

本项目基于bupticybee大佬的项目二次开发: https://github.com/bupticybee/elephantfish

严格遵守原项目的GPL License: https://github.com/bupticybee/elephantfish/blob/master/LICENSE.md

v1/随机走子(Random Policy Test): `python elephantfish_pvs_v1.py`

v4/加入了吃暗车的价值估计，可以根据明车的数量判断暗车的价值: `python elephant_pvs_v4.py`

这款AI一天前才设计好，还有完善空间。通过forbidden_moves集合和generate_forbiddenmoves函数, 当前最新的elephantfish_pvs.py应该已经限制了长将/长捉。之前的v1-v4版本是没有长将/长捉判断逻辑的。但是elephantfish_pvs.py目前还没有经过充分测试，逻辑可能有bug。


