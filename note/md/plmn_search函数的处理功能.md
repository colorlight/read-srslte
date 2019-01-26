### plmn_search函数是一种典型的下行数据的处理基本模式
1. 加锁
2. do while循环
    * cell_search
    * 解析sib1
    * 存储plmn tac
3. 解锁

### 解析cell_search 函数
phy_interface_rrc 这种命名方式

这个对象是给rrc用的，里面是有关phy的接口

```rrc->cell_search``` 通过这个接口，调用了phy的cell_search   
但是phy的cell_search是由一个phch_recv类型的成员sf_recv来实现的 
```
sf_recv.cell_search(cell);
```

