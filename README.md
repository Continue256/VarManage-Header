# **VarManage 库使用说明**  
VarManage 是一个轻量级 C++ 变量状态管理库，支持变量生命周期追踪、状态管理、内存自动托管等功能，兼容 C++98 至现代 C++ 标准。  
## **一、库特性**  
**跨标准兼容**:适配 C++98/11/14/17 等标准，自动识别编译器版  
**内存安全**:基于 RAII 机制实现堆内存自动托管,杜绝内存泄漏  
**状态管理**:支持变量存在状态追踪、批量管理及安全修改  
**异常处理**:自定义越界异常类,提供清晰的错误码机制  
**便捷 API**:宏定义简化变量注册流程,语义化方法提升开发效率  
## **二、快速开始**  
### **1. 配置环境**  
方法一(放到编译器目录,一劳永逸;还可以使用`#include <VarManage.hpp>`导入):
从相对路径`VarManage_folder\Header-Source_code(Windows)`或`VarManage_folder/Header-Source_code(Unix)`找到`"VarManage.hpp"`
把`"VarManage.hpp"`放入编译器的"`include"`目录
方法二(使用相对路径,每次使用时相对麻烦):
把`VarManage.hpp`复制到项目目录里
### **2. 引入库**  
```cpp
#include "VarManage.hpp"
// 不建议省略命名空间，需显式使用var::前缀
```  
### **3. 初始化**  
```cpp
// 方式1：直接调用初始化函数(可以不初始化)
var::initialize(10); // 初始化容量为10的变量数组，默认值{"", true}

// 方式2：使用初始化类(支持全局/函数外初始化)
var::Initialize init(5, {"default", false}); // 初始化容量5,默认值{"default", false}
```  
### **4. 变量注册与管理**  
```cpp
// 使用宏快速定义并注册变量
_vm_add_var(int, age, 25); // 注册变量age,状态默认为true(宏内部已包含var::前缀)
_vm_add_var_state(std::string, name, "VarManage", false); // 注册变量name,状态为false

// 手动添加变量
var::add("height", true); // 添加变量height，状态为true
```  
## **三、核心功能详解**  
### **1. 版本信息**  
库版本号存储于`var::_vmpublic_version`可通过以下方式访问:  
#### **1. 直接访问:**  
```cpp
printf("%d.%d.%d", var::_vmpublic_vesrion->major_version, var::_vmpublic_version->minor_version, var::vmpublic_version->patch); //输出1.1.1
```  
#### **2. 赋值后访问:**  
```cpp
const var::_vt_version* version = var::_vmpublic_version;
printf("%d.%d.%d", version->major_version, version->minor_version, version->patch_version); //也输出：1.1.1
```  
### **2. 内存托管类**  
#### **(1)`var::EntrustHeap<T>`:单个对象内存托管**  
用于托管堆内存对象,自动释放:  
```cpp
int* data = new int(10);
var::EntrustHeap<int> heap(data); // 托管data指针

heap.clear(); // 主动释放内存
int* ptr = heap.noentrust(); // 移交内存管理权(不再自动释放)
```  
#### **(2)`var::EntrustArrayHeap<T>`:数组内存托管**  
用于托管堆内存数组,自动释放:  
```cpp
std::string* arr = new std::string[3]{"a", "b", "c"};
var::EntrustArrayHeap<std::string> arr_heap(arr); // 托管数组指针

arr_heap = new std::string[2]{"x", "y"}; // 自动释放旧数组,托管新数组
```  
### **3. 变量状态查询**  
#### **(1)`var::get(name)`:查询变量位置(不忽略状态)**  
返回变量在数组中的索引，不存在或状态为 `false` 则返回`var::not_found(-1)`:  
```cpp
long long index = var::get("age");
if (index != var::not_found) {
    printf("变量age存在，索引：%lld\n", index);
}
```  
#### **(2)`var::used_get(name)`:查询变量位置(忽略状态)**  
仅判断变量是否被添加过，不关注当前状态  
```cpp  
long long index = var::used_get("name");
if (index != var::not_found) {
    printf("变量name已注册，索引：%lld\n", index);
}
```  
#### **(3)`var::exist(name)`/`var::used_exist(name)`:布尔值查询**  
直接返回变量是否存在(是否被注册):  
```cpp
if (var::exist("age")) {
    printf("变量age当前存在\n");
}
if (var::used_exist("name")) {
    printf("变量name已注册\n");
}
```  
#### **(4)`var::find(index)`:判断索引有效性  
检查索引是否为`var::not_found`:  
```cpp
if (var::find(index)) {
    printf("索引有效\n");
}
```  
### **4. 变量状态修改**  
#### **(1)`var::state(nme, state, strict)`:修改变量状态**  
`strict=false`(默认):变量不存在时自动创建  
`strict=true`:变量不存在时返回 false(严格模式)  
```cpp
var::state("age", false); // 将age状态设为false
var::state("new_var", true, true); // 严格模式下，new_var不存在则返回false
```  
#### **(2)`var::revise(mode, index, content)`:安全修改变量信息**  
`mode="name"`:修改变量名  
`mode="state"`:修改变量状态(`content` 为 `"true"`/`"false"`)  
越界时抛出`var::CrossTheLineException`异常:  
```cpp
try {
    var::revise("name", 0, "new_age"); // 将索引0的变量名改为new_age
    var::revise("state", 0, "false"); // 将索引0的状态改为false
} catch (var::CrossTheLineException& e) {
    printf("异常：%s\n", e.what());
}
```  
#### **(3)`var::rename(old_name, new_name)`:变量重命名**  
```cpp
var::rename("age", "user_age"); // 将age重命名为user_age
```  
### **5. 变量删除**  
#### **(1)`var::clear(name, affirm)`:删除指定变量**  
需设置`affirm=true`确认删除:  
```cpp
if (var::clear("age", true)) {
    printf("变量age已删除\n");
}
```  
#### **(2)`var::clear_all(affirm)`:清空所有变量**  
```cpp
var::clear_all(true); // 清空变量数组
```  
### **6. 辅助功能**  
#### **(1)打印变量状态**  
```cpp
var::print_get("age"); // 输出变量age状态(不忽略状态)
var::print_used_get("name", '\n'); // 输出变量name状态(忽略状态),换行结尾
```  
#### **(2)布尔值反转**  
```cpp
bool inverse = var::bool_inverse(true); // 返回false
```  
#### **(3)错误处理**  
`var::good()`:判断库状态是否正常  
`var::cross(specific)`:查询越界异常  
`specific=false`(默认):返回是否存在越界  
`specific=true`:返回具体错误类型  
```cpp
if (!var::good()) {
    var::error err = var::cross(true);
    if (err.do_error) {
        printf("错误：%s\n", err.specific_error.c_str());
    }
}
```  
#### **(4)`var::erase(mode)`：安全修改错误码**  
用于重置或修改内部错误码，异常时生成错误日志:  
```cpp
var::erase("not_have"); // 
```  
### **7. 异常处理**  
库自定义`var::CrossTheLineException`异常，用于捕获数组越界错误:  
```cpp
try {
    var::revise("name", 999, "error"); // 越界操作
} catch (var::CrossTheLineException& e) {
    printf("越界异常：%s\n", e.what());
    // 输出：you_specify_index_cross_the_line(name_mode)
}
```  
## **四、宏定义**  
|   宏名   |	功能	|     示例    |  
|:--------:|:---------:|:----------:|  
|`_var_name(content)`|    将内容转为字符串    |`_var_name(age)` → `"age"`|  
|`_vm_add_var(type, var, val)`|    定义变量并注册(状态 true)    |`_vm_add_var(int, num, 10)`(内部调用`var::add`)|  
|`_vm_add_var_state(type, var, val, state)`|    定义变量并注册(指定状态)    |`_vm_add_var_state(std::string, str, "test", false)`(内部调用`var::add`)|  
## **五、注意事项**  
**环境要求**:仅支持 C++ 编译环境，纯 C 环境会触发下面编译错误(以GCC为例):  
```diff
- error: #error "使用VarManage.hpp前请先使用C++环境编译(Please compile with a C++ environment first before using VarManage.hpp)"
-     4 | #error "使用VarManage.hpp前请先使用C++环境编译(Please compile with a C++ environment first before using VarManage.hpp)"
-         ^~~~~
```  
**命名空间**:所有 API 最好带`var::`前缀,最好不要省略或使用`using namespace var;`  
**内存管理**:使用`var::EntrustHeap`/`var::EntrustArrayHeap`托管的内存,禁止手动释放  
**严格模式**：`var::state()`方法的严格模式下，变量不存在时不会自动创建  
**错误码**:`var::error_code`为 `"not_have"` 表示库状态正常，其他值为错误状态  
## **六、完整示例**  
```cpp
#include "VarManage.hpp"
#include <iostream>

int main() {
    // 初始化
    var::Initialize init(5);
    
    // 注册变量
    _vm_add_var(int, age, 25);
    _vm_add_var_state(std::string, name, "VarManage", false);
    
    // 查询状态
    if (var::exist("age")) {
        var::print_get("age", '\n'); // 输出：变量(variable):age 状态(state):存在(exist)
    }
    
    // 修改状态
    var::state("name", true);
    var::print_used_get("name", '\n'); // 输出:变量(variable):name 状态(state):存在(exist)
    
    // 重命名
    var::rename("age", "user_age");
    var::print_get("user_age", '\n');
    
    // 删除变量
    var::clear("user_age", true);
    
    // 重置错误码
    var::erase("not_have");
    
    //异常处理
    try {
        var::revise("name", 999, "abc");
    } catch(const var::CrossTheLineException& e) {
        std::cout << "捕获到var::CrossTheLineException异常,原因:" << e.what() << std::endl;
    }
    return 0;
}
```
## **总结**
VarManage 库通过带var::命名空间前缀的 API 实现变量状态的全生命周期管理,兼顾安全性与易用性,适用于需要集中管理变量状态的场景(如配置管理、资源追踪系统)。使用时需严格保留命名空间前缀,遵循内存托管规则与异常处理机制,确保库的稳定运行。
