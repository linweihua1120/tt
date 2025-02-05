#include <stdint.h>
#include <stdbool.h>

#include "sgd.h"

// 定义目标电流和系统参数
#define TARGET_CURRENT 3.0f   // 目标电流 4A
#define ADC_MAX_VOLTAGE 5.0f  // ADC的最大输入电压（取决于参考电压）
#define ADC_RESOLUTION 4096   // ADC分辨率（例如12位为4096）
#define MAX_DUTY 0.85f         // 最大占空比 90%
#define MIN_DUTY 0.3f         // 最小占空比 10%
#define KP 0.1f              // 比例系数，用于调整控制精度

// 模拟函数，读取ADC采样值（需替换为实际ADC读取函数）
uint16_t read_adc_value(void) {
    // 替换为您的ADC采样代码
    return sgdGetADCCurrentres(); // 示例：返回0表示0A
}

int _pluse = 1;
int sgdGetPwmPluse(){
	return _pluse;
}
// 设置PWM占空比函数（需替换为实际PWM控制函数）
void set_pwm_duty_cycle(float duty_cycle) {
    if (duty_cycle > MAX_DUTY) {
        duty_cycle = MAX_DUTY;
    } else if (duty_cycle < MIN_DUTY) {
        duty_cycle = MIN_DUTY;
    }
	 _pluse =(int)  (duty_cycle * 100);
	 set15Pluse(100-_pluse);
}
// 计算当前电流（通过ADC值）
float calculate_current(uint16_t adc_value) {
    float adc_voltage = (adc_value * ADC_MAX_VOLTAGE) / ADC_RESOLUTION; // ADC电压
    return (adc_voltage / 1.0f) * 4.0f;  // 转换为电流，1V对应4A
}
// 恒流控制（无积分调节）
void regulate_current_no_integral(void) {
    static int preChargingState = 0; //  
    static float duty_cycle = 0.7f; // 初始化为50%占空比
    uint16_t adc_value = read_adc_value();   // 读取ADC值
    float current = calculate_current(adc_value); // 计算实际电流

  	if(preChargingState == 1 && (isCharging())){
		duty_cycle = 0.7f;

  		}
	preChargingState  = isCharging();
    // 快速限制：如果电流超过目标，增加占空比；如果低于目标，减小占空比
    if (current < TARGET_CURRENT) {
        duty_cycle += 0.012f;  // 增加占空比以降低电流
    } else if (current > TARGET_CURRENT) {
        duty_cycle -= 0.01f;  // 减小占空比以增加电流
    }
    // 限制占空比范围
    if (duty_cycle > MAX_DUTY) {
        duty_cycle = MAX_DUTY;
    } else if (duty_cycle < MIN_DUTY) {
        duty_cycle = MIN_DUTY;
    }
		SEGGER_RTT_printf(0,"C=\t %d  duty_cycle= \t%d %    over = \t%d  tick = \t%d  charging = \t%d hv  = %d\r\n", (int)(current * 1000) ,(int)(100- duty_cycle * 100),(current  > TARGET_CURRENT) ,getTick()  ,isCharging(),sgdGetADCHV( ));

    // 设置新的占空比
//    set_pwm_duty_cycle(100-duty_cycle);
    set_pwm_duty_cycle(duty_cycle);

    // 打印调试信息
//    SEGGER_RTT_printf(0, "adc_value = %d, current = %d, duty_cycle = %d\r\n",
//                       adc_value,
//                       (int)(current * 1000),
//                       (int)(duty_cycle * 1000));
}

// 控制恒流（快速响应）
void regulate_current_fast(void) {
    static float previous_duty_cycle = 0.5f; // 初始化为50%占空比
    uint16_t adc_value = read_adc_value();   // 读取ADC值
    float current = calculate_current(adc_value); // 计算实际电流

    // 快速限制：如果电流超过上限，立即减小占空比
    if (current >= TARGET_CURRENT) {
        previous_duty_cycle -= 0.05f;  // 快速减小占空比
        if (previous_duty_cycle < MIN_DUTY) {
            previous_duty_cycle = MIN_DUTY; // 防止占空比低于最小值
        }
    } else {
        // 正常比例调整
        float error = TARGET_CURRENT - current;  // 计算误差
        previous_duty_cycle += KP * error;       // 调整占空比

        // 限制占空比范围
        if (previous_duty_cycle > MAX_DUTY) {
            previous_duty_cycle = MAX_DUTY;
        } else if (previous_duty_cycle < MIN_DUTY) {
            previous_duty_cycle = MIN_DUTY;
        }
    }
	
		SEGGER_RTT_printf(0,"C=%d  previous_duty_cycle= %d %\r\n", (int)(current * 1000) ,(int)(100- previous_duty_cycle * 100) );

    // 设置新的占空比
    set_pwm_duty_cycle(previous_duty_cycle);

    // 打印调试信息
//    SEGGER_RTT_printf(0, "adc_value = %d, current = %d, duty_cycle = %d\r\n",
//                       adc_value,
//                       (int)(current * 1000),
//                       (int)(previous_duty_cycle * 1000));
}

void currentRecord(){

}

#define MAX_SAMPLES 1000  // 最大采样点数
#define TARGET_CURRENT 3.0f  // 目标电流 3A
#define HV_SCALE 1300         // HV电压比例因子
#define MAX_CHARGES 10        // 最大充电次数

float current_samples[MAX_SAMPLES];  // 记录电流数据的数组
int sample_count = 0;                // 当前采样点计数
float a = 0.5f;                      // 初始参数 a
float b = 1.0f;                      // 初始参数 b
int charge_count = 0;                // 当前充电次数
bool is_adjusting_a = true;          // 当前是否在调整 a

// 模拟读取HV值（需替换为实际函数）
//int sgdGetADCHV(void) {
//    // 返回模拟的HV值
//    return 1000;  // 示例值，需替换为实际ADC采样值
//}
void sgdFilterCurrent(int new_value);
void sgdFilterBat(int new_value);

// 模拟读取电流值（需替换为实际函数）
int read_current(void) {
    // 替换为实际电流采样函数
    return sgdGetADCCurrentresmA( );  // 示例值，需替换为实际采样电流
}

// 根据 a 和 b 设置占空比
int calculate_pluse(int hv, float a, float b) {
    return ((a + b * hv / HV_SCALE) * 4 / 5);
}

// 分析一次充电数据，返回初始电流
float analyze_charge_data(void) {
    if (sample_count == 0) return 0.0f;

    // 返回充电初始时的电流
    return current_samples[0];
}

// 分析电流变化趋势，返回趋势偏差（正值表示电流随 HV 增大而减小，负值表示电流增大）
float analyze_current_trend(void) {
    if (sample_count < 2) return 0.0f;

    float total_deviation = 0.0f;

    for (int i = 1; i < sample_count; i++) {
        float delta_current = current_samples[i] - current_samples[i - 1];
        total_deviation += delta_current;
    }

    return total_deviation;
}

// 调整参数 a，使初始电流接近目标值
void adjust_a(float initial_current) {
    float error = TARGET_CURRENT - initial_current;

    // 根据误差调整 a
    if (error > 0.1f) {
        a += 0.01f;  // 增大 a，提高初始电流
    } else if (error < -0.1f) {
        a -= 0.01f;  // 减小 a，降低初始电流
    }

    printf("Adjusting a: a = %.2f, initial_current = %.2f\n", a, initial_current);
}

// 调整参数 b，使电流变化更平稳
void adjust_b(float trend) {
    // 根据趋势调整 b
    if (trend > 0) {
        b += 0.01f;  // 增大 b，减少电流随 HV 增大的下降趋势
    } else if (trend < 0) {
        b -= 0.01f;  // 减小 b，减少电流随 HV 增大的上升趋势
    }

    printf("Adjusting b: b = %.2f, trend = %.2f\n", b, trend);
}


// 新增变量用于记录调用次数
int call_count = 0;
// 新增变量用于累加电流和 HV 值
float total_current = 0.0;
int total_hv = 0;
int pluseVbat=10,pluseA=20,pluseB=30,pluseValue=40;
void setPluseInfo(int v,int a,int b,int p){
	pluseVbat = v;
	pluseA = a;
	pluseB = b;
	pluseValue = p ;
}
// 记录充电过程中的电流数据
void record_charge_data(void) {
    int hv = sgdGetADCHVmV();  // 获取 HV 值
    float current = read_current() * 0.001;  // 获取当前电流

    // 累加电流和 HV 值
    total_current += current;
    total_hv += hv;

    // 记录电流数据
    if (sample_count < MAX_SAMPLES) {
        current_samples[sample_count++] = current;
    }

    // 计算占空比
    int pluse = calculate_pluse(hv, 0, 0);  // 这里假设 a 和 b 初始为 0，可按需修改

    // 每调用 10 次输出一次平均值
    call_count++;
    if (call_count % 10 == 0) {
        float average_current = total_current / 10;
        int average_hv = total_hv / 10;

//        SEGGER_RTT_printf(0, "Average HV = %03d, Average Current = %04d  Pluse = %02d   vbat = %04d  %04d   a= %d  b=%d tick = %d  count = %d\n",
//                          		average_hv, 		read_current() ,	pluseValue, pluseVbat,  getFilterBat_mV( ),	pluseA,		pluseB, 		getTick(),getOneFlashCount() );

        // 重置累加值
        total_current = 0.0;
        total_hv = 0;
    }
}

// 充电控制逻辑
void charge_control(void) {
    sample_count = 0;  // 重置采样点计数
    sgdFilterBat( sgdGetADCBat( ));	
  //  sgdFilterCurrent( sgdGetADCCurrentresmA( ));
	record_charge_data();
 
//    // 模拟充电过程
//    for (int i = 0; i < MAX_SAMPLES; i++) {
//        record_charge_data();
//    }
//
//    charge_count++;  // 增加充电次数
//    printf("Charge #%d completed.\n", charge_count);
//
//    // 分析数据并调整参数
//    if (is_adjusting_a) {
//        float initial_current = analyze_charge_data();
//        adjust_a(initial_current);
//
//        // 如果 a 调整完成，切换到调整 b
//        if (charge_count >= MAX_CHARGES) {
//            is_adjusting_a = false;
//            charge_count = 0;  // 重置充电计数
//        }
//    } else {
//        float trend = analyze_current_trend();
//        adjust_b(trend);
//
//        // b 调整完成后可以停止，或者重新开始新一轮优化
//        if (charge_count >= MAX_CHARGES) {
//            printf("Optimization completed: a = %.2f, b = %.2f\n", a, b);
//        }
//    }
}

//int main() {
//    // 模拟多次充电过程
//    for (int i = 0; i < 20; i++) {
//        charge_control();
//    }
//
//    return 0;
//}

// 控制恒流
//void regulate_current(void) {
//    static float previous_duty_cycle = 0.5f; // 初始化为50%占空比
//    uint16_t adc_value = read_adc_value();   // 读取ADC值
//    float current = calculate_current(adc_value); // 计算实际电流
//    float error = TARGET_CURRENT - current;  // 计算误差
//
//    // 调整占空比
//    float new_duty_cycle = previous_duty_cycle + KP * error;
//
//    // 设置新的占空比
//    set_pwm_duty_cycle(new_duty_cycle);
// 		SEGGER_RTT_printf(0, "adc_value = %d calculate_current   %d   new_duty_cycle %d\r\n",adc_value,(int)(current * 1000),(int)(new_duty_cycle * 1000));
//
//    // 保存当前占空比，用于下一次循环
//    previous_duty_cycle = new_duty_cycle;
//}
//
//int main(void) {
//    // 初始化ADC和PWM模块
//    // init_adc();
//    // init_pwm();
//
//    while (true) {
//        regulate_current(); // 调用恒流控制函数
//
//        // 控制循环频率，例如延时1ms
//        // delay_ms(1);
//    }
//
//    return 0;
//}


#define WINDOW_SIZE 50  // 滤波窗口大小（奇数）

// 用于排序的比较函数
int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

// 中值滤波结构体
typedef struct {
    int buffer[WINDOW_SIZE];  // 滑动窗口数据
    int index;                // 当前插入位置
    int count;                // 当前数据数量
} MedianFilter;

// 初始化滤波器
void init_median_filter(MedianFilter *filter) {
    for (int i = 0; i < WINDOW_SIZE; i++) {
        filter->buffer[i] = 0;
    }
    filter->index = 0;
    filter->count = 0;
}

// 中值滤波函数
int median_filter_update(MedianFilter *filter, int new_value) {
    // 将新值加入缓冲区
    filter->buffer[filter->index] = new_value;
    filter->index = (filter->index + 1) % WINDOW_SIZE;

    // 如果数据不足WINDOW_SIZE，增加计数
    if (filter->count < WINDOW_SIZE) {
        filter->count++;
    }

    // 拷贝当前有效数据到临时数组
    int temp[WINDOW_SIZE];
    for (int i = 0; i < filter->count; i++) {
        temp[i] = filter->buffer[i];
    }

    // 排序临时数组并取中值
    qsort(temp, filter->count, sizeof(int), compare);
    return temp[filter->count / 2];
}
int filterBat_mV = 0;
    MedianFilter batMedianFilter;
int filterC_mA = 0;
    MedianFilter ccurrentMedianFilter;
void sgdFilterBat(int new_value){
	filterBat_mV = median_filter_update( &batMedianFilter ,new_value);
}
void sgdFilterCurrent(int new_value){
	filterC_mA = median_filter_update( &ccurrentMedianFilter ,new_value);
}
int getFilterBat_mV(){
	return filterBat_mV;
}
int getFilterC_mA(){
	return filterC_mA;
}
//int main() {
//    // 示例采集数据
//    int input[] = {
//        3823, 3808, 3828, 3881, 3859, 3808, 3803, 3820, 3886, 3864,
//        3813, 3808, 3828, 3876, 3867, 3833, 3811, 3818, 3884, 3872,
//        3818, 3808, 3818, 3879, 3854, 3823, 3798, 3830, 3876, 3862
//    };
//    int size = sizeof(input) / sizeof(input[0]);
//
//    // 初始化滤波器
//    MedianFilter filter;
//    init_median_filter(&filter);
//
//    // 滤波输出
//    printf("Filtered Data:\n");
//    for (int i = 0; i < size; i++) {
//        int filtered_value = median_filter_update(&filter, input[i]);
//        printf("%d ", filtered_value);
//
//        if ((i + 1) % 10 == 0) {
//            printf("\n");
//        }
//    }
//
//    return 0;
//}

