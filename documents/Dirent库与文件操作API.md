# Dirent库
dirent库是一个C++编程接口，允许程序员在Linux/UNIX检索有关文件和目录的信息，通过检索到的信息程序员可以实现许多有关文件与目录的操作。
# 打开一个目录
```cpp
/* 按输入的路径名打开一个文件夹，返回一个 DIR 结构体的指针，结构体内保存了目录的相关信息 */
extern DIR *opendir (const char *__name) __nonnull ((1));
```
## DIR*
DIR的结构如下：
```cpp
struct __dirstream
   {
    void *__fd;  //目录文件描述符的指针
    char *__data; //目录块的指针
    int __entry_data; //__data对应的条目号
    char *__ptr; //指向当前块的指针
    int __entry_ptr; //__ptr对应的条目号
    size_t __allocation; //块分配到空间
    size_t __size; //块中有效数据的总量
    __libc_lock_define (, __lock) //此结构的互斥锁
   };
typedef struct __dirstream DIR;
```
关于对得到的DIR结构体的使用，有以下的函数：
```cpp
int closedir (DIR *__dirp);
truct dirent *readdir (DIR *__dirp);
void rewinddir (DIR *__dirp)；
void seekdir (DIR *__dirp, long int __pos);
long int telldir (DIR *__dirp);
```
- `int closedir (DIR *__dirp);`用于关闭一个目录流，如果成功返回0，如果失败返回-1。
- `truct dirent *readdir (DIR *__dirp);`这个函数用于返回一个指向描述目录流中的下一个目录条目的dirent结构指针，当readdir()到达目录流末尾时，返回null。
- `void rewinddir (DIR *__dirp)`这个函数将传入的目录流读取位置偏移重设为0
- `void seekdir (DIR *__dirp, long int __pos);`这个函数依靠传入参数设置目录流的当前读取位置
- `long int telldir (DIR *__dirp);`这个函数用于返回当前目录流的读取位置距目录文件开头的偏移量，有错误时返回-1；

## dirent*
对于readdir()得到的dirent*结构体指针，它的结构体构成如下所示
```cpp
struct dirent {
    ino_t d_ino; //文件的inode号
    off_t d_off; //到下一个文件(dirent)的偏移量
    unsigned short d_reclen; //此条记录的长度
    unsigned char d_type; //文件类型
    char d_name[256]; //文件名
};
enum
  {
    DT_UNKNOWN = 0,    //文件类型未知
# define DT_UNKNOWN	DT_UNKNOWN
    DT_FIFO = 1,       //命名管道
# define DT_FIFO	DT_FIFO
    DT_CHR = 2,        //字符设备
# define DT_CHR		DT_CHR
    DT_DIR = 4,        //目录
# define DT_DIR		DT_DIR
    DT_BLK = 6,        //块设备
# define DT_BLK		DT_BLK
    DT_REG = 8,        //常规文件
# define DT_REG		DT_REG
    DT_LNK = 10,       //符号链接
# define DT_LNK		DT_LNK
    DT_SOCK = 12,      //套接字
# define DT_SOCK	DT_SOCK
    DT_WHT = 14        //"白化"文件
# define DT_WHT		DT_WHT
  };
```
从dirent的结构可以看出，它也是偏索引类型的，所提供的文件信息很少，除了文件名和inode索引之外，如果想要操作更多的信息，还需要对其进行进一步处理
```cpp
int lstat (const char *__restrict __file,struct stat *__restrict __buf);
```
这个函数接受一个文件名，和一个stat结构体指针，用以将文件信息写入结构体内。
它的作用便是获取文件的详细信息。其返回值为0时表示访问成功，-1表示失败。
## stat*
stat的结构如下
```cpp
struct stat {
        mode_t     st_mode;       //文件访问权限
        ino_t      st_ino;        //索引节点号
        dev_t      st_dev;        //文件使用的设备号
        dev_t      st_rdev;       //设备文件的设备号
        nlink_t    st_nlink;      //文件的硬连接数
        uid_t      st_uid;        //所有者用户识别号
        gid_t      st_gid;        //组识别号
        off_t      st_size;       //以字节为单位的文件容量
        time_t     st_atime;      //最后一次访问该文件的时间
        time_t     st_mtime;      //最后一次修改该文件的时间
        time_t     st_ctime;      //最后一次改变该文件状态的时间
        blksize_t st_blksize;     //包含该文件的磁盘块的大小
        blkcnt_t   st_blocks;     //该文件所占的磁盘块
};
```
可以看到这样得到的文件信息就非常详细了，有了这些信息，程序员就可以进行相关的文件和目录操作。
## 如何得到文件信息
总结一下如何得到文件的详细信息：
1. 首先，需要使用opendir()方法传入一个目录字符串，得到返回的DIR结构体指针。
2. 然后将得到的DIR*传入readdir()得到dirent*结构体指针。
3. 该结构体内存有文件inode索引，文件类型与文件名，可以使用这些信息做基本的操作。
4. 如果需要更详细的文件信息，则需要使用lstat函数，参数为对应的文件名和stat的结构体指针，函数将文件详细信息写入传入的结构体
5. stat结构体内的信息就相当详细了

这样我们就得到了文件的详细信息。三种结构体不断细化，是一个从笼统到详细的过程。
# 文件系统API
有了上面知识的支持，就可以编写出基本的文件操作API了，可以很方便地操作文件。
## 删除文件
从删除一个文件开始，函数应该至少有一个输入，表示文件名，为了程序的灵活性和可调试性，可以额外增加一个bool变量force表示是否强制执行删除。
```cpp
static bool unlink(const std::string& filename, bool force = false);
bool FSUtil::unlink(const std::string& filename, bool force) {
    if(!force && __lstat(filename.c_str())) {
        return true;
    }
    return ::unlink(filename.c_str()) == 0;
}
```
__lstat是对lstat的一层封装，具体实现如下：
```cpp
static int __lstat(const char* file, struct stat* st = nullptr) {
    struct stat lst;
    int ret = lstat(file, &lst);
    if(st) {
        *st = lst;
    }
    return ret;
}

```
返回值表示是否访问成功，stat的默认值可以让函数增加“查看文件是否存在”的功能，也可以当作普通的lstat函数使用。
回到unlink函数上来，如果force为真，!force为假，由于短路原则函数直接跳出，开始执行删除操作。如果force为假，则尝试寻找目标文件，如果目标文件不存在则不执行删除操作。force参数的加入可以让程序更加灵活，例如在目标文件预期一定存在的情况下将froce置为真可以排查出相关的错误。
## 读取文件
```cpp
static bool openForRead(std::ifstream& ifs, const std::string& filename,
                            std::ios_base::openmode mode);
bool FSUtil::openForRead(std::ifstream& ifs, const std::string& filename, std::ios_base::openmode mode)
{
    ifs.open(filename.c_str(), mode);
    return ifs.is_open();

}
```
这个函数实现很简单，按只读的方式打开一个文件并返回成功与否，返回后从ifs流中读取文件的信息。
## 写入文件
```cpp
static bool openForWrite(std::ofstream& ofs, const std::string& filename,
                            std::ios_base::openmode mode);
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
```
写入文件的操作比起只读要更多一些，首先尝试打开文件，如果目录不存在则创建对应的目录，再次尝试打开，最后再返回成功与否。
需要注意的是，即使两次尝试访问也有可能出现无法打开的情况，可能原因更多是系统权限相关的问题，所以也不能直接返回true。
## 得到父目录路径
```cpp
static std::string dirName(const std::string& filename);
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
```
程序的逻辑很清晰，从路径右侧开始寻找第一个‘/’出现的位置，返回父目录的路径。

## 创建目录
```cpp
static bool mkDir(const std::string& dirname);
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
```
首先根据传入的目录使用__lstat()访问，如果目标目录存在则无需创建，直接返回。
如果目标目录无法访问，则需要创建目录，这其中就有一个问题，缺少的目录可能并不只是最后一级的目标目录，如果我们尝试创建`/home/GGo/progect1/src/pattern1/subpattern1`这个目录，可能从`/home/GGo`下就没有project1目录了，需要依次创建上层目录再创建最终目标目录。
我的处理是使用两个变量，一个字符串指针指向整个目录路径头部，另一个字符串指针指向待创建目录的尾部
```cpp
char* path = strdup(dirname.c_str());
char* ptr = strchr(path + 1, '/');
```
例如在`/home/GGo/progect1/src/pattern1/subpattern1`中，path指向字符串头部，prt指向`/home/`的尾部，然后将其改为`/home\0`，ptah变为`/home`,创建该目录后将ptr指针处字符还原，继续寻找下一级目录（下一个“/”），在这个例子中，ptr指向`/home/GGo/`,将path改为`/home/GGo\0`，创建GGo目录
直到创建完path=`/home/GGo/progect1/src/pattern1\0`的目录后，下一次ptr指向path真正的尾部，while(ptr)循环退出，如果此时ptr不为空，则代表在创建目录时发生错误中途退出，如果为空说明一切正常，创建最后一级目录。
代码中的"do{}while(0)"用以区分程序返回路径，任何在创建目录过程中的错误都会跳出这个循环并返回false，在循环体内结束程序才会返回true。
创建文件夹部分用到的"__mkdir()"也是对"mkdir()"的功能封装，先寻找目标目录，若失败则创建目标目录。
```cpp
static int __mkdir(const char *dirname)
{
    if (access(dirname, F_OK) == 0)
    {
        return 0;
    }
    return mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}
```
## 寻找特定格式文件
```cpp
static void listAllFile(std::vector<std::string>& files,
    const std::string& path,
    const std::string& subfix = "");
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
                //TODO::strcmp(s1,s2)相等返回0，s1>s2返回正数
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
```
此函数得到目标目录下指定后缀的所有文件，存入传入的vector容器中。
首先检查目标目录是否存在，不存在直接退出，之后使用opendir()打开目标目录，打开失败也直接返回，之后循环使用readdir()得到目录下所有的文件信息并依次处理。
如果文件类型是普通文件，取其文件名并组成组成路径，再确认后缀，后缀相符存入结果。
如果文件类型是目录，首先跳过'.'和'..'，再修改前缀路径后递归调用。
最后关闭打开的目录即可。
## 移动文件
```cpp
static bool Mv(const std::string& from, const std::string& to);
bool FSUtil::Mv(const std::string &from, const std::string &to)
{
    if(!Rm(to)){
        return false;
    }
    return rename(from.c_str(),to.c_str());
}
```
这个函数比较简单，先删除目标文件，再移动目标文件。
## 删除文件
```cpp
static bool Rm(const std::string& path);
bool FSUtil::Rm(const std::string& path)
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
    DIR* dir = opendir(path);
    if(!dir){
        return false;
    }
    bool ret = true;
    struct dirent* dp = nullptr;
    while(dp = readdir(dir)){
        if((!strcmp(dp->d_name,"."))||(!strcmp(dp->d_name,".."))){
            continue;
        }
        std::string dirname = path + "/" + dp->d_name;
        ret = Rm(dirname);
    }
    close(dir);
    if(::rmdir(path.c_str())){
        ret = false;
    }
    return ret;   
}
```
函数首先访问目标路径，访问不到则直接返回，如果目标路径是普通文件则直接删除即可。
如果目标是一个目录，则需要递归删除内部的所有文件，使用"opendir()"打开文件，再循环调用"readdir()"读取目录。
dfs遍历完全部文件后关闭目录开始删除并依次向上返回结果。
## 得到文件名
```cpp
static std::string baseName(const std::string& filename);
std::string FSUtil::baseName(const std::string& filename)
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
```
函数从字符串右边寻找第一个'/'出现的位置，并返回其右边的部分，即为文件名部分。
## 文件绝对路径
```cpp
static bool realPath(const std::string& path, std::string& rpath);
bool FSUtil::realPath(const std::string& path, std::string& rpath)
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

```
函数对"::realpath（）"进行封装，将得到的文件绝对路径存入第二个参数内，并返回成功与否的状态。
## 新建符号链接
```cpp
static bool symLink(const std::string& from, const std::string& to);
bool FSUtil::symLink(const std::string& from, const std::string& to)
{
    if(!Rm(to)){
        return false;
    }
    return ::symlink(from.c_str(), to.c_str()) == 0;
}

```
这个函数对"::symlink()"可能出现的错误情况进行了处理，是对底层简单功能的封装。
# 总结
本文中首先介绍了dirent.h库的基本使用，其中包括如何打开文件夹，如何打开文件，如何得到文件详细信息等操作。然后通过dirent.h库编写了一系列的文件系统API。
# 完整代码
```cpp
//定义部分
class FSUtil{
public:
    /**
     * @brief 得到目标目录下所有subfix格式的文件并加入到files容器中
     * 
     * @param files 文件容器
     * @param path 目标路径
     * @param subfix 文件格式,默认列出所有文件
     */
    static void listAllFile(std::vector<std::string>& files,
                            const std::string& path,
                            const std::string& subfix = "");

    /**
     * @brief 获取目标文件绝对路径
     * 
     * @param path 目标文件路径
     * @param rpath 存放结果
     */
    static bool realPath(const std::string& path, std::string& rpath);

    /**
     * @brief 返回文件的父目录路径
     *
     * @param filename 文件名
     * @return std::string 父目录路径
     */
    static std::string dirName(const std::string& filename);

    /**
     * @brief 创建文件夹
     * 
     * @param dirname 文件夹名称
     */
    static bool mkDir(const std::string& dirname);

    /**
     * @brief 移除目标目录
     * 
     * @param path 路径名
     */
    static bool Rm(const std::string& path);

    /**
     * @brief 移动文件
     * 
     * @param from 源路径
     * @param to 目标路径
     */
    static bool Mv(const std::string& from, const std::string& to);

    /**
    * @brief 删除一个文件
    * @param filename 文件名
    * @param force 是否强制执行删除/预期文件是否存在
    * @return 是否删除成功
    */
    static bool unLink(const std::string& filename,bool force = false);

    /**
     * @brief 得到文件名
     * 
     * @param filename 文件路径
     */
    static std::string baseName(const std::string& filename);

    /**
     * @brief 新建符号链接
     * 
     * @param from 源文件
     * @param to 目标链接位置
     */
    static bool symLink(const std::string& from, const std::string& to);

    /**
     * @brief 打开文件并读取
     * 
     * @param ifs 文件流
     * @param filename 文件名
     * @param mode 打开模式
     * @return true 打开成功
     * @return false 打开失败
     */
    static bool openForRead(std::ifstream& ifs, const std::string& filename,
                            std::ios_base::openmode mode);

    /**
     * @brief 打开文件并写入
     * 
     * @param ofs 文件流
     * @param filename 文件名
     * @param mode 打开模式
     * @return true 写入成功
     * @return false 写入失败
     */
    static bool openForWrite(std::ofstream& ofs, const std::string& filename,
                            std::ios_base::openmode mode);


};
//实现部分
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
                //TODO::strcmp(s1,s2)相等返回0，s1>s2返回正数
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

bool FSUtil::mkDir(const std::string& dirname)
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

bool FSUtil::Rm(const std::string& path)
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

bool FSUtil::Mv(const std::string& from, const std::string& to)
{
    if(!Rm(to)){
        return false;
    }
    return rename(from.c_str(),to.c_str());
}

bool FSUtil::unLink(const std::string& filename, bool force)
{
    if(!force && __lstat(filename.c_str())){
        return true;
    }
    return ::unlink(filename.c_str()) == 0;
}

std::string FSUtil::baseName(const std::string& filename)
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

bool FSUtil::symLink(const std::string& from, const std::string& to)
{
    if(!Rm(to)){
        return false;
    }
    return ::symlink(from.c_str(), to.c_str()) == 0;
}

bool FSUtil::openForRead(std::ifstream& ifs, const std::string& filename, std::ios_base::openmode mode)
{
    ifs.open(filename.c_str(), mode);
    return ifs.is_open();

}

bool FSUtil::openForWrite(std::ofstream& ofs, const std::string& filename, std::ios_base::openmode mode)
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
```


