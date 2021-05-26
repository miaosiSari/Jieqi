# 揭棋

本项目基于bupticybee大佬的项目二次开发: https://github.com/bupticybee/elephantfish

严格遵守原项目的GPL License: https://github.com/bupticybee/elephantfish/blob/master/LICENSE.md

v1/Random Policy Test: `python elephantfish_pvs_v1.py`

v2/不建议使用

v3/Naive AI Test (已经解决AI喜欢搏子的毛病): `python elephantfish_pvs_v3.py`

v4/加入了吃暗车的价值估计，可以根据明车的数量判断暗车的价值: `python elephant_pvs_v4.py`

这款AI一天前才设计好，还有完善空间。通过forbidden_moves集合和generate_forbiddenmoves函数, 当前最新的elephant_pvs.py应该已经限制了长将/长捉。但是elephantfish_pvs.py目前还没有经过充分测试，逻辑可能有bug。


