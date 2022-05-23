# NimBLE

Project Description

NimBLE TODO 列表

----

Reference:

- https://github.com/todomd/todo.md
- VSCode extension: TODO.md Kanban Board

<em>[TODO.md spec & Kanban Board](https://bit.ly/3fCwKfM)</em>

### Todo

- [ ] NimBLE 外部蓝牙芯片初始化支持  
  - [ ] 架构搭建，参考 BTstack  
  - [ ] 支持 Zephyr Controller  
  - [ ] 支持 AP6212 (ART-Pi)  

- [ ] NimBLE app 功能测试完善  
  - [ ] blecent  
  - [ ] blecsc  
  - [ ] blehci  
  - [ ] blemesh  
  - [ ] blemesh_light  
  - [ ] blemesh_models_example_1  
  - [ ] blemesh_models_example_2  
  - [ ] blemesh_shell  
  - [ ] bleprph  
  - [ ] bletest  
  - [ ] bleuart  
  - [ ] btshell  
  - [ ] ext_advertiser  
  - [ ] ibeacon  
- [ ] 合并仓库到 upstream  
- [ ] 测试使用 rtthread studio 正常编译运行 NimBLE  
- [ ] 测试 NimBLE Controller 功能是否正常  
- [ ] rtthread npl 移植文件 ble_npl_eventq_remove() 函数 bug 解决  

### In Progress

- [ ] 完善 rtthread qemu 蓝牙开发环境 @Jackistang [[rtthread-qemu-ble](https://github.com/Jackistang/rtthread-qemu-ble)]

### Done ✓

- [x] 同步 rtthread NimBLE 版本至 1.5.0，并完善编译选项  
- [x] 利用 rtthread 对接 NimBLE 传输层  
- [x] 对接 NimBLE 日志系统  

  - [x] advertiser  
  - [x] blehr  
