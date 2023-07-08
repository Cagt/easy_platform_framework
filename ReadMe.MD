# 驱动开发文档

## 一. 主要文件结构介绍

    - EQUIPMENT
     |-- servo_device.c/h
     |-- rc_device.c/h
     |-- motor_device.c/h
     |-- mpu9250.c/h

    - MIDLAYER
     |-- list.h
     |-- qz_device_frame.h
     |-- qz_device_frame.c

    - HARDWARE
     |-- LED.c
     |-- key.c
     ...

    - USER
     |-- main.c
     ...


EQUIPMENT : 设备驱动文件夹，用于存放 基于 qz_device_frame  去整合设计相对应的设备的驱动,可利用 I/O 如 (read write) 操作对应的设备驱动 的代码

MIDLAYER ：设备驱动中间层，用于存放 qz_device_frame  实现源码，该轻量级框架主要是简单实现 BSP 和 应用驱动层的分离，好处是可实现应用层和芯片种类的弱耦合。

USER ：主要是 main.c 文件,实现 main 函数的操作,建议代码量越少越好

HARDWARE ： 存放 BSP 的实现代码





## 驱动框架介绍

驱动框架示意图

 ![image](./%E5%B1%82%E7%BA%A7%E5%85%B3%E7%B3%BB.jpg)

可以看到应用层主要是通过 read、write 函数实现对于底层的 PWM GPIO UART 等外设驱动的调用。

具体使用要先从下面结构体详解:

```c
//实现 顶层应用和底层BSP分离
typedef struct device_ops{		            /* 设备操作类 */
    
    char name[16];							/* 设备名称  */
    struct list_head list;
    int (*read)(void * data);
    int (*write)(void *data);
}device_t;

typedef struct object_base{
    struct device_ops ops;	            /* 驱动操作	*/
    enum DEVICE_CLASS cls;	            /* 标签	*/
    void * info;						/* 万物信息 */
}object_t;

```

1. device_ops 是调用设备IO接口的关键，它里面存放该设备的 read 和 write 函数指针，当调用框架层中的dev_read 和 dev_write 函数时,本质上是直接调用  object_base 中的 device_ops 中的 read 和 write 指向的函数，其中的read write 由用户自己定义实现的。 

2. object_base 是应用层设备个体的抽象，它可以表示任何应用层设备，存放任何应用层设备驱动所需要的信息。cls 是该应用层设备驱动的标签,目前没什么用，但是可能后面会有用(也许吧)... info 指针是存放着用户自定义的驱动数据的地址,比如motor_device.c 中的 motor_data_t 结构体。对 motor_device write 则是会根据 motor_device 中存放着 info 所指向的 motor_data_t 中的 control_pwm_a/b 存储的占空比驱动电机，而 read 则是直接读取电机编码器。 不同的应用层驱动设备有不同的数据，这些都可以由用户自定义。比如 mpu9250 数据则是读取的三轴加速度、三轴角速度...

3. 特定函数介绍

```c
void device_init(void);						/* 所有的设备都可以添加到此初始化 */

struct device_ops* find_device(char *name);
int device_register(struct device_ops* ops);
void *ops_to_device_handle(struct device_ops* ops);
int dev_read(struct device_ops * device,void * data);
int dev_write(struct device_ops * device,void * data);

```

device_init: 会调用所有装载的 init 函数，无需用户负责

find_device：在驱动管理链表中寻找对应的驱动

device_register：将实现好的驱动注册到驱动管理链表中,供给用户使用

ops_to_device_handle ： 根据 device_ops 返回对应的 object_base

dev_read/write：调用 device_ops 中的write 和 read,如果读/写成功，则会返回0，否则返回负值


驱动使用示例代码:

驱动电机
```c

int main(void){
    device_init();

	device_t * motor1 = find_device("MOTOR1");		//寻找电机驱动
    motor_data_t data1;                             //用于保存电机驱动相关数据

    if(motor1 != NULL)
    {
        dev_read(motor1,(void *) &data1);           //读取电机驱动中的信息数据
    }

    data1.control_pwm_a = 5000;                     //给其中一个相输出5000占空比
    data1.control_pwm_b = 0;

    dev_write(motor1,(void *) &data1);              //输出
    while(1)
    {
        dev_read(motor1,(void *)&data1);
        printf("speed:%d",data1.speed);
    }


    return 0；
}
I
```

驱动 PS2 
```c

int main(void){
    device_init();

	device_t * ps2 = find_device("PS2");		//寻找电机驱动
    ps2_data_t ps2_data;                             //用于保存电机驱动相关数据

    while(1){

        int ret = dev_read(ps2,(void *) &ps2_data);           //读取电机驱动中的信息数据
        if(!ret){
            printf("LX:%d RX:%d start:%d select:%d \n",ps2_data.LX,ps2_data.RX,ps2_data.ps2_key_left.start,ps2_data.ps2_key_left.select);  
             //LX LY RX RY则是返回0 到 255的值 PS2的按键,如果按下则是返回0
        }

    }

    return 0；
}


```
---
## 驱动框架开发介绍

### 驱动开发流程
1. 在 EQUIPMENT 中新建一个 xxx_device.c/h 

2. 在 xxx_device.h 文件中定义自己的 xxx_data_t 。并且申明 xxx_init、xxx_read、xxx_write、xxx_BSP_init、xxx_BSP_Write、xxx_BSP_Read ...等其他
 
    其中 xxx_BSP_... 类的函数是应用层设备所使用对应的 BSP 设备驱动实现，一般 xxx_read/write 则会调用 xxx_BSP_write 和 xxx_BSP_read , 也可不调用(一切由用户自己实现)

3. 在 xxx_device.c 文件中实现 以上申明的函数，一般 xxx_init 都是设置 object 的属性(如 ops cls 和 info) , info 则是指向堆区中的数据结构。还有 ops 的 read 和 write函数，是他们的指针指向自定义的 xxx_read 和 xxx_write 函数。

4.写好的 init 函数下面调用 INSTALL_INIT( init 的函数名) 可实现初始化的自动装载。

实例代码(可作为模板使用):

rc_device.h
```c
#ifndef __PS2_DEVICE_H
#define __PS2_DEVICE_H

#include "qz_device_frame.h"
#include "sys.h"
#include "stm32f10x.h"


/*
* ps2按键 key 
*/

typedef struct ps2_data
{
	int RX;
	int RY;
	int LX;
	int LY;
	__packed struct 
	{
		unsigned select	 	:1;
		unsigned l3			 	:1;
		unsigned r3			 	:1;
		unsigned start		:1;
		unsigned up		 		:1;						//上键 
		unsigned right		:1;							//右键 				
		unsigned down			:1;				//下键 
		unsigned left   	:1;						//左键 
		
	}ps2_key_left;
	
	
	__packed struct 
	{
		unsigned l2	 :1;
		unsigned r2			 :1;
		unsigned l1			 :1;
		unsigned r1		 :1;
		unsigned detal		 :1;					// 三角键
		unsigned circle:1;							//圈键				
		unsigned x			 :1;						//X键
		unsigned rect   :1;							//方块 
		
	}ps2_key_right;
	
	
}ps2_data_t;

void ps2_init(void);
static int ps2_read(void *data);
static int ps2_write(void *data); 
static void Ps2_BSP_Init(void);									/*BSP Init 用于实现初始化设备对应的底层BSP */
static void Ps2_BSP_Write(ps2_data_t * data);					/*BSP Write 用于实现初始化设备对应的底层BSP */



#endif
```

ps2_device.c
```c
#include "rc_device.h"


static volatile object_t ps2_device;


void ps2_init(void)
{
	ps2_data_t * ps2Data = (ps2_data_t *)malloc(sizeof(ps2_data_t));
	
	

	if(ps2Data == NULL)
		return;

	ps2_device.cls = RC_CLASS;
	ps2_device.info = (void *)ps2Data;


	strncpy(ps2_device.ops.name,"PS2",strlen("PS2") + 1);
	ps2_device.ops.read = ps2_read;
	ps2_device.ops.write = ps2_write;

 //PS2 BSP Init

	Ps2_BSP_Init();		//TODO: BSP 应该可以根据传输进来的数据进行初始化 

	memset(ps2Data,0,sizeof(ps2_data_t));

	device_register(&ps2_device.ops);


}
INSTALL_INIT(ps2_init);

static int ps2_read(void *data)
{
		
	ps2_data_t * pData = (ps2_data_t *)ps2_device.info;
	extern u8 Data[9];																	/*获取到的数据*/
	
	
	PS2_ClearData();
	PS2_ReadData();
	
	pData->RX = Data[PSS_RX];
	pData->RY = Data[PSS_RY];
	pData->LX = Data[PSS_LX];
	pData->LY = Data[PSS_LY];

	memcpy(&pData->ps2_key_left,&Data[3],1);						//复制左半部分按键
	memcpy(&pData->ps2_key_right,&Data[4],1);						//复制右半部分按键
	
		
	memcpy(data,ps2_device.info,sizeof(ps2_data_t));

	return 0;
}

static int ps2_write(void *data)			//Write函数暂无实现
{

//	memcpy(servo_device.info ,data,sizeof(servo_data_t));

//	servo_data_t * servo_data = (servo_data_t *)servo_device.info;

//	//调用PWM设置函数

//	/*自动限幅*/
//	if(servo_data->control_pwm > servo_data->max_pwm)
//		servo_data->control_pwm = servo_data->max_pwm;

//	else if(servo_data->control_pwm < servo_data->min_pwm)
//		servo_data->control_pwm = servo_data->min_pwm;

//	Servo_BSP_Write((servo_data_t *)servo_device.info);

	return 0;
}

///************************* BSP层面的实现 START ***********************************/

void Ps2_BSP_Init(void)
{
	PS2_Init();
	PS2_SetInit();
}

//void Servo_BSP_Write(servo_data_t * data)
//{

////TIM1->CCR1 = (int16_t)(data->control_pwm);  //舵机引脚
//TIM_SetCompare1(TIM1,(int16_t)data->control_pwm);
//}

///************************** BSP层面的实现 END ************************************/

```

也可以参考 EQUIPMENT 目录中的已经实现的设备驱动进行实现自己的设备驱动
