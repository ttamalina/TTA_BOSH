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
*/



#ifndef CONVERTERS_TPP
#define CONVERTERS_TPP


#ifndef CONVERTERS_H
#error __FILE__ should only be included from converters.hpp.
#endif // CONVERTERS_H

/**
 * @brief Construct a new CConverterPolynom<NOrd>::CConverterPolynom object
 * 
 * @param f_coeff The list of the coeffience.
 */
template<uint8_t NOrd>
CConverterPolynom<NOrd>::CConverterPolynom(std::array<float,NOrd+1> f_coeff)
:m_coeff(f_coeff)
{
}


/**
 * @brief Convert the input value. 
 * 
 */
template<uint8_t NOrd>
float CConverterPolynom<NOrd>::operator()(float f_v){
    float l_res = m_coeff[NOrd];
    for (uint8_t i = 0;i<NOrd;++i){
        l_res += std::pow(f_v, NOrd-i)*m_coeff[i];
    }
    return l_res;
}


/**
 * @brief Construct a new CConverterSpline<NrBreak, NOrd>::CConverterSpline object
 * 
 * @tparam NrBreak 
 * @tparam NOrd 
 * @param f_breaks The list of the break points.
 * @param f_splines The list of the polynomial function.
 */
template <uint8_t NrBreak, uint8_t NOrd>
CConverterSpline<NrBreak, NOrd>::CConverterSpline(CConverterSpline<NrBreak, NOrd>::CBreakContainerType f_breaks,CConverterSpline<NrBreak, NOrd>::CSplineContainerType f_splines)
:m_breaks(f_breaks)
,m_splines(f_splines)
{
}

/**
 * @brief Convert the input value.
 * 
 */
template <uint8_t NrBreak, uint8_t NOrd>
float CConverterSpline<NrBreak, NOrd>::operator()(float f_value){
    typename CConverterSpline<NrBreak, NOrd>::CBreakContainerType::iterator it_breaks0=NULL,it_breaks1=NULL;
    typename CConverterSpline<NrBreak, NOrd>::CSplineContainerType::iterator it_spline = m_splines.begin();
    for (it_breaks1 = m_breaks.begin();it_breaks1 != m_breaks.end();){
        if((it_breaks0==NULL || (it_breaks0!=NULL && (*it_breaks0)<f_value)) && f_value<=(*it_breaks1)){
            break;
        }
        it_breaks0 = it_breaks1;
        ++it_breaks1;
        ++it_spline;
    }
    return this->splineValue(*it_spline, f_value);
}

/**
 * @brief Convert the given value, by using the given polynom.
 * 
 * @param f_coeff The coeffiences of the polynom
 * @param f_value The input value.
 */
template <uint8_t NrBreak, uint8_t NOrd>
float CConverterSpline<NrBreak, NOrd>::splineValue( CCoeffContainerType f_coeff,float f_value){
    float l_res = f_coeff[NOrd];
    for (uint8_t i = 0;i<NOrd;++i)
    {
        l_res+=std::pow(f_value,NOrd-i)*f_coeff[i];
    }
    return l_res;
}

#endif 