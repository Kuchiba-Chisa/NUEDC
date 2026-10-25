#include "ti_msp_dl_config.h"
#include "adc.h"
#include "delay.h"
uint16_t adc_getValue(void)
{
    unsigned int gAdcResult = 0;

    // 启动单次转换
    DL_ADC12_enableConversions(ADC12_0_INST);

    DL_ADC12_startConversion(ADC12_0_INST);
    
    // 等待转换完成
    while(DL_ADC12_getStatus(ADC12_0_INST) != DL_ADC12_STATUS_CONVERSION_IDLE);
    // 清除状态标志
    DL_ADC12_stopConversion(ADC12_0_INST);

    DL_ADC12_disableConversions(ADC12_0_INST);
    // DL_ADC12_clearStatus(ADC12_0_INST);
    gAdcResult = DL_ADC12_getMemResult(ADC12_0_INST, ADC12_0_ADCMEM_0);
    // 获取转换结果
    return gAdcResult;
}

// 获取多次采样的平均值
// uint16_t Get_ADC_Average(uint8_t times)
// {
//     uint32_t ADC_Sum = 0;
    
//     for(uint8_t i = 0; i < times; i++)
//     {
//         ADC_Sum += adc_getValue();
//         delay_ms(5); // 5ms延迟 (根据需求调整)
//     }
    
//     return (uint16_t)(ADC_Sum / times);
// }