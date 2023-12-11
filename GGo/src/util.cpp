#include "util.h"
#include <execinfo.h>
#include <sys/time.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#include "logSystem.h"
#include "fiber.h"

namespace GGo{
    
pid_t GetThreadID()
{
    return syscall(SYS_gettid);
}

uint32_t GetFiberID()
{
    //不在这里getthis，有些线程不一定有协程，不然会在这里初始化一个main协程
    return GGo::Fiber::getFiberID();
}

/// @brief 将编译器混淆后变量名转为人类可读变量名
/// @param mangled_name 
/// @return 
static std::string demangle(const char *mangled_name)
{
    int status;
    char *demangled_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);

    if (status == 0)
    {
        std::string result(demangled_name);
        free(demangled_name);
        return result;
    }
    else
    {
        return std::string(mangled_name);
    }
}

void backTrace(std::vector<std::string> &bt, int size, int skip)
{
    void** buffer = (void **)malloc((sizeof(void *) * size));
    size_t s = ::backtrace(buffer, size);

    char** strings = backtrace_symbols(buffer, s);
    if(strings == nullptr){
        free(buffer);
        return;
    }
    for(size_t i = skip; i < s; i++){
        bt.push_back(demangle(strings[i]));
    }
    free(buffer);
    free(strings);

}

std::string backTraceToString(size_t size, int skip, const std::string &prefix)
{
    std::stringstream ss;
    std::vector<std::string> bt;
    backTrace(bt, size, skip);
    for(size_t i = 0; i < bt.size(); i++){
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}

uint64_t getCurrentMS()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
}

uint64_t getCurrentUS()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 * 1000ul + tv.tv_usec / 1000;
}

/**
 * @brief 得到指定文件的stat结构体
 *
 * @param filename 文件名
 * @param st 结构体指针
 * @return int lstat()的返回值 0成功 -1失败
 */
static int __lstat(const char *filename, struct stat *st = nullptr)
{
    struct stat lst;
    int ret = lstat(filename, &lst);
    if (st)
    {
        *st = lst;
    }
    return ret;
}

/**
 * @brief 创建目录
 *
 * @param dirname 目录名
 * @return int 状态码 0:成功
 */
static int __mkdir(const char *dirname)
{
    if (access(dirname, F_OK) == 0)
    {
        return 0;
    }
    return mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void FSUtil::listAllFile(std::vector<std::string> &files, const std::string &path , const std::string &subfix)
{
    //检查路径是否存在
    if(access(path.c_str(),F_OK)){
        return;
    }
    DIR* dir = opendir(path.c_str());
    //检查目录是否能打开
    if(dir == nullptr){
        return;
    }
    dirent* dp = nullptr;
    while((dp = readdir(dir)) != nullptr){
        if(dp->d_type == DT_DIR){
            //DT_DIR目录文件
            if((!strcmp(dp->d_name,"."))||(!strcmp(dp->d_name,".."))){
                //如果是当前目录和父目录则跳过
                continue;
            }
            //如果是普通目录，列出递归列出其中的文件
            listAllFile(files,path + "/" + dp->d_name, subfix);
        }else if(dp->d_type == DT_REG){
            //DT_REG 常规文件
            std::string fielname(dp->d_name);
            if(subfix.empty()){
                files.push_back(path + "/" + fielname);
            }else{
                if(fielname.size() < subfix.size()){
                    continue;
                }
                if(fielname.substr(fielname.length()-subfix.size()) == subfix){
                    //如果文件类型和给定类型相同则加入列表
                    files.push_back(path + "/" + fielname);
                }
            }
        }
    }
    //关闭打开的目录
    closedir(dir);

}

bool FSUtil::realPath(const std::string &path, std::string &rpath)
{
    if(__lstat(path.c_str())){
        return false;
    }
    char* ptr = ::realpath(path.c_str(), nullptr);
    if(ptr == nullptr){
        return false;
    }
    std::string(ptr).swap(rpath);
    free(ptr);
    return true;
}

std::string FSUtil::dirName(const std::string &filename)
{
    if(filename.empty()){
        return ".";
    }
    //从右侧开始查找第一个 ‘/’
    auto pos = filename.rfind('/');
    if(pos == 0){
        // "/usr" = > "/"
        return "/";
    }else if(pos == std::string::npos){
        // "workspace" = > "."
        return ".";
    }else{
        // "workspace/GGoSeverFrame/GGo" = > "workspace/GGoSeverFrame/"
        return filename.substr(0,pos);
    }

}

bool FSUtil::mkDir(const std::string &dirname)
{
    if(__lstat(dirname.c_str()) == 0){
        //如果找到了目标目录，则不用创建，直接退出，返回true
        return true;
    }
    //创建目标目录
    char* path = strdup(dirname.c_str());
    char* ptr = strchr(path + 1, '/');
    do{
        while(ptr){
            *ptr = '\0';
            if(__mkdir(path) != 0){
                break;
            }
            *ptr = '/';
            ptr = strchr(ptr + 1, '/');
        }
        if(ptr != nullptr){
            break;
            //创建最终目录
        }else if(__mkdir(path) != 0){
            break;
        }
        free(path);
        return true;
    }while(0);
    free(path);
    return false;
}

bool FSUtil::Rm(const std::string &path)
{
    struct stat st;
    if(lstat(path.c_str(),&st)){
        //访问不到目标目录，不用删除
        return true;
    }
    if(!(st.st_mode & S_IFDIR)){
        //如果不是目录，直接删除文件即可
        return unLink(path);
    }
    //处理是目录的情况
    DIR* dir = opendir(path.c_str());
    if(!dir){
        return false;
    }
    bool ret = true;
    struct dirent* dp = nullptr;
    while((dp = readdir(dir))){
        if((!strcmp(dp->d_name,"."))||(!strcmp(dp->d_name,".."))){
            continue;
        }
        std::string dirname = path + "/" + dp->d_name;
        ret = Rm(dirname);
    }
    closedir(dir);
    if(::rmdir(path.c_str())){
        ret = false;
    }
    return ret;
    
}

bool FSUtil::Mv(const std::string &from, const std::string &to)
{
    if(!Rm(to)){
        return false;
    }
    return rename(from.c_str(),to.c_str());
}

bool FSUtil::unLink(const std::string &filename, bool force)
{
    if(!force && __lstat(filename.c_str())){
        return true;
    }
    return ::unlink(filename.c_str()) == 0;
}

std::string FSUtil::baseName(const std::string &filename)
{
    if(filename.empty()){
        return filename;
    }
    auto pos = filename.rfind('/');
    if(pos == std::string::npos){
        return filename;
    }else{
        return filename.substr(pos + 1);
    }
}

bool FSUtil::symLink(const std::string &from, const std::string &to)
{
    if(!Rm(to)){
        return false;
    }
    return ::symlink(from.c_str(), to.c_str()) == 0;
}

bool FSUtil::openForRead(std::ifstream &ifs, const std::string &filename, std::ios_base::openmode mode)
{
    ifs.open(filename.c_str(), mode);
    return ifs.is_open();

}

bool FSUtil::openForWrite(std::ofstream &ofs, const std::string &filename, std::ios_base::openmode mode)
{
    ofs.open(filename.c_str(),mode);
    //打开失败，一般是目录不存在
    if(!ofs.is_open()){
        //尝试创建目录新建文件
        std::string dir = dirName(filename);
        mkDir(dir);
        ofs.open(filename,mode);    
    }
    //不能直接返回true,因为目录的创建和文件的创建也可能因为权限的问题而失败
    return ofs.is_open();
}

std::string StringUtil::format(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    std::string str = formatVa(fmt, ap);
    va_end(ap);
    return str;
}

std::string StringUtil::formatVa(const char *fmt, va_list op)
{
    char* buffer = nullptr;
    auto len = vasprintf(&buffer, fmt, op);
    if(len == -1){
        return "";
    }
    std::string str(buffer, len);
    free(buffer);
    return str;
}

static const char uri_chars[256] = {
    /* 0 */
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 0, 0, 0, 1, 0, 0,
    /* 64 */
    0, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 0, 0, 0, 0, 1,
    0, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 0, 0, 0, 1, 0,
    /* 128 */
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    /* 192 */
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0,
};
static const char xdigit_chars[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,
    0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};


std::string StringUtil::urlEncode(const std::string &str, bool space_as_plus)
{
    static const char* hexdigits = "0123456789ABCDEF";
    std::string* ss = nullptr;
    const char* end = str.c_str() + str.length();
    for(const char* c = str.c_str(); c < end; c++){
        if(!(uri_chars[(unsigned char)(*c)])){
            if(!ss){
                ss = new std::string;
                ss->reserve(str.size() * 1.2);
                ss->append(str.c_str(), c - str.c_str());
            }
            if(*c == ' ' && space_as_plus){
                ss->append(1, '+');
            }else{
                ss->append(1, '%');
                ss->append(1, hexdigits[(uint8_t)*c >> 4]);
                ss->append(1, hexdigits[*c & 0x0f]);
            }
        }else if(ss){
            ss->append(1, *c);
        }
    }

    if(!ss){
        return str;
    }else{
        std::string rt = *ss;
        delete ss;
        return rt;
    }
}
std::string StringUtil::urlDecode(const std::string &str, bool sapce_as_plus)
{
    std::string* ss = nullptr;
    const char* end = str.c_str() + str.length();
    for(const char* c = str.c_str(); c < end; c++){
        if(*c == '+' && sapce_as_plus){
            if(!ss){
                ss = new std::string;
                ss->append(str.c_str(), c - str.c_str());
            }
            ss->append(1, ' ');
        }
        else if(*c == '%' && (c + 2) < end && isdigit(*(c + 1)) && isdigit(*(c + 2))){
            if(!ss){
                ss = new std::string;
                ss->append(str.c_str(), c - str.c_str());
            }
            ss->append(1, (char)(xdigit_chars[(int)*(c + 1)] << 4 | xdigit_chars[(int)*(c + 2)]));
            c += 2;
        }else if(ss){
            ss->append(1, *c);
        }
    }
    if(!ss) {
        return str;
    } else {
        std::string rt = *ss;
        delete ss;
        return rt;
    }
}
std::string StringUtil::trim(const std::string &str, const std::string &delimit)
{
    auto begin = str.find_first_not_of(delimit);
    if(begin == std::string::npos) {
        return "";
    }
    auto end = str.find_last_not_of(delimit);
    return str.substr(begin, end - begin + 1);
}
std::string StringUtil::trimLeft(const std::string &str, const std::string &delimit)
{
    auto begin = str.find_first_not_of(delimit);
    if(begin == std::string::npos) {
        return "";
    }
    return str.substr(begin);
}
std::string StringUtil::trimRight(const std::string &str, const std::string &delimit)
{
    auto end = str.find_last_not_of(delimit);
    if(end == std::string::npos) {
        return "";
    }
    return str.substr(0, end);
}
}