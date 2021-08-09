![揭棋](https://img.shields.io/badge/Jieqi-python3-brightgreen)
![揭棋](https://img.shields.io/badge/Jieqi-pypy3-blue)

# 揭棋

这是一个揭棋AI项目，提供命令行对战UI和基于概率/pvs算法的AI。您可以使用python3和pypy3运行。本项目不支持python2/pypy2。

本项目基于bupticybee大佬的项目elephantfish二次开发: https://github.com/bupticybee/elephantfish, 并严格遵守bupticybee设置的GPL License V3: https://github.com/bupticybee/elephantfish/blob/master/LICENSE.md 。我的项目和elephantfish
都基于sunfish开发: https://github.com/thomasahle/sunfish 。

## v1/随机走子(Random Policy Test): 
`python musesfish_pvs_v1_fixed.py`

## v2/加入了吃暗车的价值估计，可以根据明车的数量判断暗车的价值: 

`python musesfish_pvs_v2_fixed.py`

## 20210528/加入了很多逻辑:

### 1. 空头炮

对于空头炮, 之前的版本只考虑了两种情况: 第一，炮从不在中线的位置走到了中线空头的位置，加分; 第二，炮从中线空头的位置离开了中线，扣分。但我遗漏了另外两种情况。炮沿着中线移动，也有可能从非空头变成空头，或者空头变为非空头，唯一的可能就是打掉对方的子。这两种情况在之前的版本中疏漏了，导致之前的AI经常出现"弃空头"的问题。此外，忽视了空头炮和对方老将的距离。距离越远或者有车配合，抽射能力更强。

### 2. 沉底炮

对于沉底炮，之前的版本盲目打分过高。实际上，相比象棋，揭棋中的沉底炮确实更有威胁，因为没有连环士/连环相的威胁，很容易被捉暗士，沉底车，或捉暗相闷宫，防不胜防。此外，沉底炮在底线牵制诸多暗子，同时瞄着炮碾丹砂等手段，而象棋中沉底炮牵制子力价值较小的士象。然而，如果对方在底线暗子数量有限，则沉底炮未必有很大威胁。

### 3. 暗士

对于翻动暗士，之前的版本(v2)对剩下的兵卒数量有粗略的估计，如果剩余兵卒数量过多，则不鼓励翻动暗士。然而，在这个版本中，如果对方同侧暗车已出，且暗士所在的肋道上己方车的数量比对方多，则鼓励翻士出将助攻。如果对方暗车未出，或相应肋道上己方车的数量小于等于对方车的数量，则仍然沿用兵卒逻辑。

### 4. 暗炮进四

鼓励暗炮进四压制对手。

### 5. 暗炮进六:

如果对方的暗马未启动，且暗相位置有东西，则鼓励暗炮进六压制暗马，其鼓励分数正比于本方所剩兵卒的数量的数学期望。

### 6. 暗炮打马

在己方车未完全出动的情况下，如果对方暗炮进四翻出的不是车或者兵，以及不存在边路兵顶马的情形，则将暗炮打马设为禁着。

### 7. 暗车出动

如果对方暗炮打暗马赌博，赌出来的是大子或兵，鼓励吃掉。

### 8. 跳暗马保暗兵/卒

如果对方在三七路线出车，鼓励跳暗马保暗兵。

### 9. 长将/长捉

不允许电脑走入重复局面，避免了长将。

### 10. pypy

支持pypy3运行程序。pypy使用JIT编译技术，规避了GIL。

运行方法: `python musesfish_pvs_20210528_fixed.py`。

### 11. 开局库

在board/library.py中增加了开局的哈希函数。

## 20210529/修复了一个BUG，并引入了score_rough统计量:

pos.gen_moves是一个生成器，不应该在这个函数中统计一些棋盘统计量。score_rough统计量根据盘面的明子对局势作出简单判断。只有当score_rough > 160时才鼓励暗炮打暗马。

## 20210531: 

### AI有时候会无脑弃马:

对马的价值估计出现了错误，这是本人揭棋水平有限的原因，不是算法bug。现在已经在common_20210531_fixed.py中提高了马和士的分数。

### 暗炮打暗马，兵顶马的坐标计算失误。

见Issue: https://github.com/miaosiSari/Jieqi/issues/1. 
Fixed in `musesfish_pvs_20210531_fixed.py`.

### 中马防守
在抢占肋道和翻动暗士的时候，不仅考虑对方中士防守，也考虑中马的防守。

## 20210601:

1. 关闭空着裁剪。空着裁剪对于杀棋的计算不够犀利。
2. 在alpha-beta pruning的时候设置了短路逻辑。该短路逻辑有助于AI快速杀棋。在有多种方法可以赢棋的时候，选择连将杀棋(809-814行)。
3. 将value的内部函数calc改为了pos的成员函数。
4. 当暗中兵受到威胁的时候，鼓励翻动暗相

## 20210604:
1. 修改calc_average函数的重大BUG!
2. 引入有根子和静态搜索(Quiescence)。
3. 将空头炮逻辑写为静态逻辑。

## 注:
带有`fixed`的版本永久固定，如果您发现有bug，欢迎提Issues，我会在新的版本中改正。您可以使用pypy3运行我的代码，只需要将`python`替换成`pypy3`即可。

在UI中，个人可以看到电脑吃的暗子，电脑也可以看到个人吃的暗子。这并不是一个BUG。在搜索算法里面，电脑是不知道个人吃的暗子的(在pos类的mymove_check中discount_red和discount_black的注释)，例如`musesfish_pvs_20210528_fixed.py`的277-281行。我在UI中输出的目的是打消您对作弊的怀疑。事实上，暗子到明子的映射早就在`main`函数开始的`mapping = B.translate_mapping(B.mapping)`中完成了。如果您不喜欢该设置，可以将`rendered_eat = translate_eat(eat, dst, "BLACK", "CLEARMODE")`改为`rendered_eat = translate_eat(eat, dst, "BLACK", "DARKMODE")`。

`musesfish_pvs.py`是正在开发的版本。`musesfish_pvs_exp.py`是正在尝试新算法的版本，这两个版本可能存在诸多bugs。

## 后续:
如果您对我的拙作感兴趣，可以联系我:

邮箱: miaosi2018@gmail.com,或者QQ: 2904661326。

本人揭棋7-2水平。

我个人认为，MCTS也许可以解决揭棋的问题。虽然揭棋是非完美博弈，且信息集合很大，但这些信息集可以被划分为有限的等价类(换句话来说，很多状态是等价的)。例如，暗相是马暗士是兵，抑或暗相是兵暗相是马，其实并没有太多的区别，都被掩盖在强子(车炮)的光环之下。因此，揭棋这个NFA也许可以近似为一个DFA，但我并不确定。
