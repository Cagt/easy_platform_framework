#include "qz_device_frame.h"

/* ->:被包含关系
*	结构体包含关系  list_head -> device ops -> object 根据这个可获取所需的操作句柄
*/

struct list_head device_head_list;

//TODO: 实现open函数,实现根据传进传输对于不同的设备进行初始化

static void qz_init_start(void)
{
	
	return;
}
INSTALL_INIT(qz_init_start);

static void qz_init_end(void)
{
	
	return;
}
INSTALL_INIT_X(qz_init_end,"end");



void device_init(void)
{
	INIT_LIST_HEAD(&device_head_list);
	const initFunc * p;
		
	for(p = &__qz_init_qz_init_start; p < &__qz_init_qz_init_end; p++){
		(*p)();
	}
//	servo_init();	//测试舵机
//		motor_init();
//	ps2_init();
//	Mpu9250_init();
}


/*@function: int device_register(struct device_ops* ops)
* @brief: 将驱动注册到驱动管理链表中
* @param:	ops:已经写好的驱动
*	@return 成功返回0
*/

int device_register(struct device_ops* ops)
{
	list_add(&ops->list,&device_head_list);
	
	return 0;
}

/*@function: struct device_ops* find_device(char *name)
* @brief: 根据name寻找写好的驱动
* @param name:	驱动名字	
*	@return 成功返回驱动句柄 否则返回NULL
*/

struct device_ops* find_device(char *name)
{
	struct list_head* pos;
	
	list_for_each(pos,&device_head_list)
	{
		struct device_ops* temp = container_of(pos,struct device_ops,list);
		
		if(!strcmp(temp->name,name))
			return temp;
				
	}
	
	return NULL;
	
}

/*@function: void *ops_to_device_handle(struct device_ops* ops)
* @brief: 根据device 句柄返回该对象驱动句柄
* @param:	ops:已经写好的驱动
*	@return 成功返回0
*/
void *ops_to_device_handle(struct device_ops* ops)
{
	return (void *)container_of(ops,struct object_base,ops);
}

/*@function: int read(struct device_ops * device,void * data)
* @brief: 调用该驱动定义的read函数 中间层的read
* @param device:	驱动操作句柄
*	@param data:  	操作数据,可由用户自定义
*	@return 返回 call read 的返回值
*/
int dev_read(struct device_ops * device,void * data)
{
	
	if(device == NULL)
		return -1;
	
	return device->read(data);
}

/*@function: int read(struct device_ops * device,void * data)
* @brief: 调用该驱动定义的read函数 中间层的read
* @param device:	驱动操作句柄
*	@param data:  	操作数据,可由用户自定义
*	@return 返回 call read 的返回值
*/
int dev_write(struct device_ops * device,void * data)
{
	if(device == NULL)
		return -1;
	
	return device->write(data);
}


