
  1. https://github.com/daohu527/Dig-into-Apollo/tree/master/cyber


  2. https://zhuanlan.zhihu.com/p/115046708






  问题:
  1. linux的cgroups
  2. std::lock_guard<std::mutex>

3.         // 构造智能指针，并且指定删除器 
        std::shared_ptr<Base> classObjSharePtr(
            class_object, std::bind(&ClassLoader::OnClassObjDeleter<Base>, this,
                                    std::placeholders::_1));


4.  utility通过调用c++的PocoFoundation库来实现加载动态库


5. 
  // 注册信号量，当出现系统错误时，打印堆栈信息
  signal(SIGSEGV, SigProc);
  signal(SIGABRT, SigProc);
https://www.runoob.com/cplusplus/cpp-signal-handling.html
https://www.gnu.org/software/libc/manual/html_node/Backtraces.html