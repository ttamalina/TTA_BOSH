/**
Copyright 2019 Bosch Engineering Center Cluj and BFMC organizers 

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

 * @file quadratureencoder.cpp
 * @author RBRO/PJ-IU
 * @brief 
 * @version 0.1
 * @date 2018-10-23
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#include <hardware/encoders/quadraturecounter.hpp>


namespace hardware::drivers{



CQuadratureCounter_TIM4 *CQuadratureCounter_TIM4::m_instance = 0;
CQuadratureCounter_TIM4::CQuadratureCounter_TIM4_Destroyer CQuadratureCounter_TIM4::m_destroyer;


/**
 * @brief Setter function for the singelton object.
 * 
 * @param pSingObj - singleton object address
 */
void CQuadratureCounter_TIM4::CQuadratureCounter_TIM4_Destroyer::SetSingleton(CQuadratureCounter_TIM4* pSingObj){
    m_singleton = pSingObj;
}  

/**
 * @brief Destroy the singelton object.
 * 
 */
CQuadratureCounter_TIM4::CQuadratureCounter_TIM4_Destroyer::~CQuadratureCounter_TIM4_Destroyer(){
    delete m_singleton;
}


/**
 * @brief 
 * It verifies the existence of the singleton object. It creates a new instance when it's necessary and return the address of instance.
 * It initializes all parameter by appling method 'initialize'.
 * 
 * @return The address of the singleton object
 */
CQuadratureCounter_TIM4* CQuadratureCounter_TIM4::Instance(){
    if(!CQuadratureCounter_TIM4::m_instance){
        CQuadratureCounter_TIM4::m_instance = new CQuadratureCounter_TIM4;
        m_instance->initialize();
        CQuadratureCounter_TIM4::m_destroyer.SetSingleton(m_instance);
    }
    return CQuadratureCounter_TIM4::m_instance;
}


/**
 * @brief Initialize the parameter of the object.
 * 
 * It configures all register, for timer TIM4 decodes the quadrature encoder signal.
 */
void CQuadratureCounter_TIM4::initialize(){
    //PB6 PB7 aka D10 MORPHO_PB7
    // Enable clock for GPIOA
   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

   //stm32f4xx.h
   GPIOB->MODER |= GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1;         //PB6 & PB7 as Alternate Function   /*!< GPIO port mode register,               Address offset: 0x00      */
   GPIOB->OTYPER |= GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7;              //PB6 & PB7 as Inputs               /*!< GPIO port output type register,        Address offset: 0x04      */
   GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7; //Low speed                         /*!< GPIO port output speed register,       Address offset: 0x08      */
   GPIOB->PUPDR |= GPIO_PUPDR_PUPDR6_1 | GPIO_PUPDR_PUPDR7_1;         //Pull Down                         /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
   GPIOB->AFR[0] |= 0x22000000;                                       //AF02 for PB6 & PB7                /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
   GPIOB->AFR[1] |= 0x00000000;                                       //nibbles here refer to gpio8..15   /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */

   // configure TIM4 as Encoder input
   // Enable clock for TIM4
   // __TIM4_CLK_ENABLE();
   RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

   TIM4->CR1 = 0x0001;                // CEN(Counter ENable)='1'     < TIM control register 1
   TIM4->SMCR = TIM_ENCODERMODE_TI12; //               < TIM slave mode control register
   //TIM_ENCODERMODE_TI1 input 1 edges trigger count
   //TIM_ENCODERMODE_TI2 input 2 edges trigger count
   //TIM_ENCODERMODE_TI12 all edges trigger count
   TIM4->CCMR1 = 0xF1F1; // CC1S='01' CC2S='01'         < TIM capture/compare mode register 1
   //0xF nibble sets up filter
   TIM4->CCMR2 = 0x0000;                       //                             < TIM capture/compare mode register 2
   TIM4->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E; //     < TIM capture/compare enable register
   TIM4->PSC = 0x0000;                         // Prescaler = (0+1)           < TIM prescaler
   TIM4->ARR = 0xffff;                         // reload at 0xfffffff         < TIM auto-reload register

   TIM4->CNT = 0x0000; //reset the counter before we use it
}

/**
 * @brief Get the position of encoder. 
 * 
 * It returns the last counted value by the timer. 
 * 
 */
int16_t CQuadratureCounter_TIM4::getCount(){
    return TIM4->CNT;
}
/**
 * @brief Reset the value of the counter to zero value.
 */
void CQuadratureCounter_TIM4::reset(){
    TIM4->CNT = 0;
}


}; // namespace hardware::drivers