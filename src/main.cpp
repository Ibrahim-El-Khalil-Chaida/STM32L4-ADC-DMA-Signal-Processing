#include "main.h"
#include "system_clock.h"

// Configuration constants
constexpr uint32_t kSampleRate = 100000;      // samples/sec
constexpr uint16_t kBufferSize = 1024;        // must be even
constexpr uint32_t kAdcChannel = ADC_CHANNEL_5; // PA0

class AdcDmaHandler {
public:
    AdcDmaHandler() { instance_ = this; }

    void Init() {
        // 1) Clocks
        __HAL_RCC_ADC_CLK_ENABLE();
        __HAL_RCC_TIM2_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();

        // 2) DMA
        hdma_.Instance                 = DMA1_Channel1;
        hdma_.Init.Request             = DMA_REQUEST_0;
        hdma_.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        hdma_.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_.Init.MemInc              = DMA_MINC_ENABLE;
        hdma_.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
        hdma_.Init.Mode                = DMA_CIRCULAR;
        hdma_.Init.Priority            = DMA_PRIORITY_HIGH;
        if (HAL_DMA_Init(&hdma_) != HAL_OK) Error_Handler();
        __HAL_LINKDMA(&hadc_, DMA_Handle, hdma_);

        // 3) ADC
        hadc_.Instance                   = ADC1;
        hadc_.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;
        hadc_.Init.Resolution            = ADC_RESOLUTION_12B;
        hadc_.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
        hadc_.Init.ScanConvMode          = DISABLE;
        hadc_.Init.ContinuousConvMode    = DISABLE;
        hadc_.Init.ExternalTrigConv      = ADC_EXTERNALTRIG_T2_TRGO;
        hadc_.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;
        hadc_.Init.DMAContinuousRequests = ENABLE;
        hadc_.Init.Overrun               = ADC_OVR_DATA_PRESERVED;
        if (HAL_ADC_Init(&hadc_) != HAL_OK) Error_Handler();

        ADC_ChannelConfTypeDef sConfig = {};
        sConfig.Channel      = kAdcChannel;
        sConfig.Rank         = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_6CYCLES_5;
        if (HAL_ADC_ConfigChannel(&hadc_, &sConfig) != HAL_OK) Error_Handler();

        // 4) Timer2 for trigger
        TIM_HandleTypeDef htim = {};
        htim.Instance           = TIM2;
        uint32_t timclk         = HAL_RCC_GetPCLK1Freq();
        htim.Init.Prescaler     = (timclk / 1000000) - 1;
        htim.Init.CounterMode   = TIM_COUNTERMODE_UP;
        htim.Init.Period        = (1000000 / kSampleRate) - 1;
        htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        if (HAL_TIM_Base_Init(&htim) != HAL_OK) Error_Handler();

        TIM_MasterConfigTypeDef mcfg = {};
        mcfg.MasterOutputTrigger = TIM_TRGO_UPDATE;
        mcfg.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(&htim, &mcfg) != HAL_OK) Error_Handler();

        // NVIC for DMA
        HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

        htim_ = htim;
    }

    void Start() {
        if (HAL_TIM_Base_Start(&htim_) != HAL_OK) Error_Handler();
        if (HAL_ADC_Start_DMA(&hadc_, reinterpret_cast<uint32_t*>(buffer_), kBufferSize) != HAL_OK)
            Error_Handler();
    }

    void Process(uint16_t* data, uint16_t len) {
        // e.g. compute average
        uint32_t sum = 0;
        for (uint16_t i = 0; i < len; ++i) sum += data[i];
        float avg = float(sum) / len;
        (void)avg;
    }

    static void OnHalfTransfer(ADC_HandleTypeDef*) {
        instance_->Process(instance_->buffer_, kBufferSize/2);
    }
    static void OnFullTransfer(ADC_HandleTypeDef*) {
        instance_->Process(instance_->buffer_ + kBufferSize/2, kBufferSize/2);
    }

private:
    ADC_HandleTypeDef hadc_{};
    DMA_HandleTypeDef hdma_{};
    TIM_HandleTypeDef htim_{};
    uint16_t buffer_[kBufferSize];
    static AdcDmaHandler* instance_;
};

AdcDmaHandler* AdcDmaHandler::instance_ = nullptr;

extern "C" void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* h) {
    AdcDmaHandler::OnHalfTransfer(h);
}
extern "C" void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* h) {
    AdcDmaHandler::OnFullTransfer(h);
}
extern "C" void DMA1_Channel1_IRQHandler() {
    HAL_DMA_IRQHandler(&AdcDmaHandler::instance_->hdma_);
}

int main() {
    HAL_Init();
    SystemClock_Config();

    AdcDmaHandler adc;
    adc.Init();
    adc.Start();

    while (1) {
        __WFI();
    }
}
