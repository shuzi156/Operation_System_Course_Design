#ifndef __C_DESIGN_NOTE__
#define __C_DESIGN_NOTE__
//=================================
//+默认值定义
//=================================
#define n 			5 		//实验中系统允许打开文件的最大数量
#define MAX_FILE 	20	//每个目录最大目录项
#define TRUE 		1 	//IF判断式TRUE
#define FALSE 		0   //IF判断是FALSE
#define OK 			1  	//函数调用式OK    	//为防止混乱，更新为IF式判断值 2014/03/28
#define FAIL 		0 	//函数调用式失败	//为防止混乱，更新为IF式判断值 2014/03/28
#define PATH_DEEP 	6 	//路径最大深度

//=================================
//+属性定义
//=================================
#define ATTR_READ_ONLY 	0x01 	//只读文件
#define ATTR_SYSTEM 	0x02 	//系统文件
#define ATTR_FILE 		0x04 	//普通文件
#define ATTR_DIR 		0x08 	//目录

//=================================
//+宏操作
//=================================
#define IS_DIR(x) ((x & ATTR_DIR) ? 1 : 0 )				//是目录就返回1，否则返回0
#define IS_FILE(x) ((x & ATTR_FILE) ? 1 : 0 )			//是普通文件就返回1，否则返回0
#define IS_READ_ONLY(x) ((x & ATTR_READ_ONLY) ? 1 : 0 )	//是只读文件就返回1，否则返回0
#define IS_SYSTEM(x) ((x & ATTR_SYSTEM) ? 1 : 0 )		//是系统文件就返回1，否则返回0

//=================================
//+FAT表数字说明
//=================================
#define FAT_NULL 		0 	//对应fAT没有内容
#define FAT_DIR  		250	//该FAT为目录
#define FAT_END 		255	//该FAT的内容是最后一部分
#define FAT_BROKEN 		254	//该FAT已经损坏



//=================================
//+已经打开文件登记表结构体
//=================================
typedef struct pointer_t{
	int dnum; 	//磁盘盘块号
	int bnum; 	//磁盘盘块内第几个字节
}pointer;

struct OFILE_S{
	char name[20]; 	//文件绝对路径名
	unsigned char attribute; //文件的属性|用1字节表示
    int number:8; 	//文件起始盘块号
	int length; 	//文件长度，文件占用的字节数
	int flag; 		//操作类型，用0表示以读操作方式打开文件，用1表示以写操作方式打开文件
	pointer read; 	//读文件的位置，文件打开时dnum为文件起始盘块号，bnum为0
	pointer write; 	// 写文件的位置，文件刚建立时dnum为文件起始盘块号，bnum为0,打开文件时dnum和bnum为文件的末尾位置
};					//已打开文件表项类型

typedef struct OFILE_S OFILE;

struct openfile_s{
	OFILE file[n]; 	//已打开文件登记表
	int length; 	//已打开文件登记表中登记的文件数量
};			//已打开文件登记表定义


//=================================
//+虚拟磁盘内容结构体
//=================================
struct buffer_s{
	char buffer[64];
};	//每个盘块内容


//=================================
//+目录项（文件项）信息保存结构体
//=================================
typedef struct file_s{
	char file_name[4];
	char file_type[3];  		//DIR=NULL,tx
	unsigned int file_attr:8;	//ATTR_*
	unsigned int start_fat:8;	//fat[start_fat]//文件开始FAT值
	unsigned int fat_count:8; 	//DIR=NULL
	char under_file[MAX_FILE];	//拥有的文件FAT值//
	int under_file_count;		//拥有的目录和文件的数量
	unsigned int father_fat:8;	//所在目录的FAT值//
}file_t;						//文件file或目录DIR属性表,保存在盘块中


//=================================
//+全局变量
//=================================
unsigned char now_dir_fat;				//当前目录的FAT值
unsigned char fat[128];					//Fat表 数字i ->  struct buffer file[i];      0为空，255为文件末尾，254为磁盘损坏
struct buffer_s store[128];		//虚拟磁盘空间，总共128个盘块
struct openfile_s openfile;		//已经打开文件登记表

//=================================
//+当前路径记录
//=================================
struct now_path_s{
	unsigned char now_fat[PATH_DEEP];//路径： /now_fat[1]/now_fat[2]/.....
	int length;				//当前路径深度//  默认为1（即根目录 / ）
};
struct now_path_s now_path;


//=================================
//+函数声明
//=================================
int init_all();					//初始化函数

file_t get_fat_dir(unsigned char fat_num);//根据fat_num返回目录项	DJ *
file_t get_now_dir();			//获取当前目录的FILE项	DJ *

int print_file();				//列出当前目录拥有的文件和目录	SZ *
char *  string_now_path();		//返回当前路径字符窜			SZ *
int print_now_path();			//打印当前路径					SZ *

int cd_dir(char * file_name);   //切换到当前目录的file_name目录   SZ int
cd_parent_dir();            //切换到父目录                    SZ

int create_file(char * file_name,char * file_type,unsigned char attr);//创建目录项,返回TRUE OR FALSE    DJ
int delete_file(char * file_name);		//删除目录项                     SZ *

unsigned char find_null_fat();          //查找空的FAT表项，返回对应数字          SZ int
delete_fat(unsigned char fat_num);  //删除对应的FAT表项，返回TRUE OR FALSE    SZ *

//2014/3/28
int dir_exist(char * dir_name);	//判断当前目录是否存在dir_name目录，存在返回TRUE，不存在返回FALSE；  	SZ *
int file_exist(char * file_name);//判断当前目录是否存在file_name文件；存在返回TRUE，不存在返回FALSE;	SZ *

//2014/4/1
int open_file(char *file_name,int flag);//根据文件名打开文件         |
int name_test(char *name);//判断名字（文件、目录）是否合法（不含特殊符号等） 			
file_t get_file_from_name(char *file_name);//根据文件名在当前目录获取文件项   			SZ *
get_fat_from_name(char *file_name)//根据文件名从当前目录获取fat值		SZ *
int open_file_add(OFILE * file,file_t open_new,int flag);//在已经打开登记表中添加；   |
int list_fd();//列出当前已经打开fd.            |
int content_read(int fd);//读取fd内容          |
int content_write(int fd);//写入fd内容	       |


#endif