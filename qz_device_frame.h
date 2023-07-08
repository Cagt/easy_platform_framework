#ifndef __DEVICE_H_
#define __DEVICE_H_


#include "sys.h"
#include "list.h"
#include <string.h>

#include "servo_device.h"
#include "motor_device.h"
#include "rc_device.h"
#include "mpu9250_device.h"



typedef void (*initFunc)(void);		

/* 将所有设备抽象出来  */
#define CONTAIN_BEWTEEN(value,min,max) (value) = (value) > (max) ? (max) : (value) < (min) ? (min) : (value)
#define SECTION(x)                  __attribute__((section(x)))		//放在第x个section
#define USED               			__attribute__((used))  //告诉编译器不要删除未使用的节


#define INSTALL_INIT(fn) \														//驱动的Init写好后放到Init代码体的下面
			USED const initFunc __qz_init_##fn SECTION(".qz_init_fn") = fn
				
#define INSTALL_INIT_X(fn,x) \
			USED const initFunc __qz_init_##fn SECTION(".qz_init_fn."x) = fn

enum DEVICE_CLASS{			/* 如果有新的外设就添加到这里来 */ 
	SERVO_CLASS,
	MOTOR_CLASS,
	MPU_CLASS,
	OLED_CLASS,
	RC_CLASS
};


//实现 顶层应用和底层BSP分离
typedef struct device_ops{		/* 设备操作类 */
	
	char name[16];							/* 设备名称  */
	struct list_head list;
	int (*read)(void * data);
	int (*write)(void *data);
}device_t;

typedef struct object_base{
	struct device_ops ops;	/* 驱动操作	*/
	enum DEVICE_CLASS cls;	/* 标签			*/
	void * info;						/* 万物信息 */
}object_t;






void device_init(void);						/* 所有的设备都可以添加到此初始化 */ 

 
struct device_ops* find_device(char *name);
int device_register(struct device_ops* ops);
void *ops_to_device_handle(struct device_ops* ops);
int dev_read(struct device_ops * device,void * data);
int dev_write(struct device_ops * device,void * data);

static void qz_init_start(void);
static void qz_init_end(void);

#endif