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

/* �������豸�������  */
#define CONTAIN_BEWTEEN(value,min,max) (value) = (value) > (max) ? (max) : (value) < (min) ? (min) : (value)
#define SECTION(x)                  __attribute__((section(x)))		//���ڵ�x��section
#define USED               			__attribute__((used))  //���߱�������Ҫɾ��δʹ�õĽ�


#define INSTALL_INIT(fn) \														//������Initд�ú�ŵ�Init�����������
			USED const initFunc __qz_init_##fn SECTION(".qz_init_fn") = fn
				
#define INSTALL_INIT_X(fn,x) \
			USED const initFunc __qz_init_##fn SECTION(".qz_init_fn."x) = fn

enum DEVICE_CLASS{			/* ������µ��������ӵ������� */ 
	SERVO_CLASS,
	MOTOR_CLASS,
	MPU_CLASS,
	OLED_CLASS,
	RC_CLASS
};


//ʵ�� ����Ӧ�ú͵ײ�BSP����
typedef struct device_ops{		/* �豸������ */
	
	char name[16];							/* �豸����  */
	struct list_head list;
	int (*read)(void * data);
	int (*write)(void *data);
}device_t;

typedef struct object_base{
	struct device_ops ops;	/* ��������	*/
	enum DEVICE_CLASS cls;	/* ��ǩ			*/
	void * info;						/* ������Ϣ */
}object_t;






void device_init(void);						/* ���е��豸��������ӵ��˳�ʼ�� */ 

 
struct device_ops* find_device(char *name);
int device_register(struct device_ops* ops);
void *ops_to_device_handle(struct device_ops* ops);
int dev_read(struct device_ops * device,void * data);
int dev_write(struct device_ops * device,void * data);

static void qz_init_start(void);
static void qz_init_end(void);

#endif