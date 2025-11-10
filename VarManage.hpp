#if !defined( _vmprivate_is_import ) //防止重复导入
#define _vmprivate_is_import 1 //用来标记是否导入
#if defined(__STDC__) && !defined(__cplusplus) //检测是否处于C标准编译,如果是报错
#error "使用VarManage.hpp前请先使用C++环境编译(Please compile with a C++ environment first before using VarManage.hpp)"
#endif
#include <exception>
#include <stdexcept>
#include <cstdio>
#include <vector>
#include <string>
#define _var_name(content) #content //把 VAR(name) 变成 "name" (加上双引号)
namespace var { //全部包裹在 var 命名空间里,防止重名
#if (defined(_MSC_VER) && _MSC_VER >= 1900) || (__cplusplus >= 201103L && !defined(_MSC_VER)) //检测C++标准是否 >= C++11
	static constexpr short not_found = -1; //没找到返回此变量
	#define _vmprivate_unexception const noexcept
#else //检测 C++ 标准是否 <= C++11
	static const short not_found = -1; //没找到返回此变量
	#define _vmprivate_unexception const throw()
#endif
    static std::string error_code = "not_have";
	class CrossTheLineException : public std::exception { //定义 CrossTheLineException 越界异常
		public:
			CrossTheLineException() {};
			virtual const char* what() _vmprivate_unexception {
				return error_code.c_str();
			}
	};
	struct variable { //用于 vector variable_array 的类型
		std::string variable_name; //变量名
		bool variable_state; //变量状态(存在(true)/不存在(false))
	};
	static std::vector<variable> variable_array; //变量列表
	inline bool add(const std::string &name, const bool &state = true) { //用于往 variable_array 里添加内容
		if (error_code == "not_have") {
			variable_array.push_back({name, state});
			return true;
		}
		return false;
	}
    #define _vm_add_var(type, variable, value)\
    type variable = value;\
    var::add(#variable, true)
    #define _vm_add_var(type, variable, value, state)\
    type variable = value;\
    var::add(#variable, state)
	inline long long get(const std::string &name) { //获取 variable_array 内是否有指定的变量(不忽略状态)
		if (error_code == "not_have") {
			for (int i = 0; i < variable_array.size(); ++i) {
				const variable& temporary_variable = variable_array[i];
				if (temporary_variable.variable_name == name && temporary_variable.variable_state == true) {
					return i;
				}
			}
			return not_found;
		}
		return 0x726566757365;
	}
	inline bool find(const long &index) { //判断传入的下标是否是表示没有找到的
		if (error_code == "not_have") {
			return !(index == not_found);
		}
		return false;
	}
	inline bool sta(const std::string &name, const bool &state = true) { //把指定变量的状态调为不存在(false)
		if (error_code == "not_have") {
			long index = get(name);
			if (index == not_found) {
				if (state == true) {
					return false;
				} else {
					add(name, state);
					return true;
				}
			} else {
				variable_array[index].variable_state = state;
				return true;
			}
		}
		return false;
	}
	inline long long used_get(const std::string &name) { //判断指定的变量是否添加过(忽略状态)
		if (error_code == "not_have") {
			for (int i = 0; i < variable_array.size(); ++i) {
				const variable& temporary_variable = variable_array[i];
				if (temporary_variable.variable_name == name) {
					return i;
				}
			}
			return not_found;
		}
		return 0x726566757365;
	}
	inline bool clear(const std::string &name, const bool &affirm = false) { //彻底清除指定变量的名字和状态(used_get无法找回)
		long index = get(name);
		if (index == not_found) {
			return false;
		} else if (affirm == true && error_code == "not_have") {
			variable_array.erase(variable_array.begin() + index);
			return true;
		}
		return false;
	}
	inline bool clear_all(const bool &affirm = false) { //彻底清除所有变量信息,内存释放
		if (affirm == true && error_code == "not_have") {
			variable_array.clear();
			return true;
		}
		return false;
	}
	inline bool print_get(const std::string &name, const char &terminator = '\0') { //输出指定变量是否存在(不忽略状态)
		if (error_code == "not_have") {
			std::printf("变量(variable):%s 状态(state):%s%c", name.c_str(), (get(name) == not_found) ? "不存在(not exist)" : "存在(exist)", terminator);
			return get(name) == not_found;
		}
		return false;
	}
	inline bool print_used_get(const std::string &name, const char &terminator = '\0') { //输出指定变量是否存在(忽略状态)
		if (error_code == "not_have") {
			std::printf("变量(variable):%s 状态(state):%s%c", name.c_str(), (used_get(name) == not_found) ? "不存在(not exist)" : "存在(exist)", terminator);
			return used_get(name) == not_found;
		}
		return false;
	}
	inline bool bool_inverse(const bool &status = true) { //反转布尔值
		if (error_code == "not_have") {
			return !(status);
		}
		return false;
	}
	inline bool exist(std::string name) { //直接返回是否存在的布尔值(不忽略状态)
		if (error_code == "not_have") {
			return get(name) != not_found;
		}
		return false;
	}
	inline bool used_exist(std::string name) { //直接返回是否存在的布尔值(忽略状态)
		if (error_code == "not_have") {
			return used_get(name) != not_found;
		}
		return false;
	}
	inline std::string revise(const std::string &mode, const long &index, const std::string &content) { //安全的变量列表修改函数
		if (error_code == "not_have") {
			if (mode == "name") {
				try {
					variable_array.at(index).variable_name = content;
				} catch (std::out_of_range&) {
					error_code = "you_specify_index_cross_the_line(name_mode)";
					throw CrossTheLineException();
				}
				return "not_have";
			} else if (mode == "state") {
				try {
					variable_array.at(index).variable_state = (content == "true") ? true : false;
				} catch (std::out_of_range&) {
					error_code = "you_specify_index_cross_the_line(state_mode)";
					throw CrossTheLineException();
				}
				return "not_have";
			} else if(mode != "name" && mode != "state" && error_code == "not_have"){
               return "invalid_mode"; 
            }
		}
		return "refuse";
	}
	struct error { //用于 cross() 的返回值
		bool do_error;
		std::string specific_error;
	};
	inline bool good() { //判断状态是否正常
		return (error_code == "not_have");
	}
	inline error cross(const bool &specific = false) { //判断是否有越界异常
		if (specific) {
			if (error_code ==  "you_specify_index_cross_the_line(name_mode)") {
				return {true, "you_specify_index_cross_the_line(name_mode)"};
			} else if (error_code ==  "you_specify_index_cross_the_line(state_mode)") {
				return {true, "you_specify_index_cross_the_line(state_mode)"};
			} else {
				return {false, "not_have"};
			}
		} else {
			return {error_code ==  "you_specify_index_cross_the_line(name_mode)" || error_code ==  "you_specify_index_cross_the_line(state_mode)", (error_code == "not_have") ? "not_have" : "unknown_error_reason"};
		}
	}
	inline unsigned long long eraser(std::string mode = "not_have") { //安全的状态修改工具
		try {
		    const std::string used_error_code = error_code;
        	error_code = mode;
			if (error_code !=  mode) {
			    error_code = used_error_code;
				return 0x6572726F72;
			}
		} catch (...) {
		    std::fprintf(stderr, "严重错误:从安全的语句中发现异常,即将退出(severe error:An exception was detected in a safe statement. Exiting now.");
		    std::fflush(stderr);
			std::abort();
		}
		return 0;
	}
	inline unsigned long long rename(const std::string &old_name, const std::string &new_name) { //重命名工具
		if (error_code == "not_have") {
			long index = get(old_name);
			if (index == not_found) {
				add(new_name);
			} else {
				revise("name", index, new_name);
			}
			return 0;
		}
		return 0x726566757365;
	}
}
#endif
