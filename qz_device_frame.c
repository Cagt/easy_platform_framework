#include "qz_device_frame.h"

/* ->:��������ϵ
*	�ṹ�������ϵ  list_head -> device ops -> object ��������ɻ�ȡ����Ĳ������
*/

struct list_head device_head_list;

//TODO: ʵ��open����,ʵ�ָ��ݴ���������ڲ�ͬ���豸���г�ʼ��

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
//	servo_init();	//���Զ��
//		motor_init();
//	ps2_init();
//	Mpu9250_init();
}


/*@function: int device_register(struct device_ops* ops)
* @brief: ������ע�ᵽ��������������
* @param:	ops:�Ѿ�д�õ�����
*	@return �ɹ�����0
*/

int device_register(struct device_ops* ops)
{
	list_add(&ops->list,&device_head_list);
	
	return 0;
}

/*@function: struct device_ops* find_device(char *name)
* @brief: ����nameѰ��д�õ�����
* @param name:	��������	
*	@return �ɹ������������ ���򷵻�NULL
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
* @brief: ����device ������ظö����������
* @param:	ops:�Ѿ�д�õ�����
*	@return �ɹ�����0
*/
void *ops_to_device_handle(struct device_ops* ops)
{
	return (void *)container_of(ops,struct object_base,ops);
}

/*@function: int read(struct device_ops * device,void * data)
* @brief: ���ø����������read���� �м���read
* @param device:	�����������
*	@param data:  	��������,�����û��Զ���
*	@return ���� call read �ķ���ֵ
*/
int dev_read(struct device_ops * device,void * data)
{
	
	if(device == NULL)
		return -1;
	
	return device->read(data);
}

/*@function: int read(struct device_ops * device,void * data)
* @brief: ���ø����������read���� �м���read
* @param device:	�����������
*	@param data:  	��������,�����û��Զ���
*	@return ���� call read �ķ���ֵ
*/
int dev_write(struct device_ops * device,void * data)
{
	if(device == NULL)
		return -1;
	
	return device->write(data);
}


