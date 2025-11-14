/**
 * @file    UtilityFunctions.h
 * @brief   图像处理工具函数集合，提供常用的图像操作和转换功能
 * @author  [JianZhang] ([])
 * @date    2025-05-19
 * @version 1.0.0
 * @details 该文件包含DSV项目中使用的各种工具函数，包括：
 *          - RGB与HSV颜色空间转换函数
 *          - 颜色查找表(LUT)应用函数
 *          - 单色图像到RGB图像转换函数
 *          - 图像数据处理和格式转换功能
 *          所有函数都是内联函数，提供高效的图像处理性能。
 *
 * @note    所有函数都是线程安全的，支持多线程并发调用
 * @see     ImageSource, SlideColorManagement
 */

#pragma once
#include <QImage>
#include <array>
#include <tuple>
#include <math.h>
#include "SlideColorManagement.h"

 /**
  * @brief   RGB颜色空间转换为HSV颜色空间
  * @details 将RGB颜色值转换为HSV(色相、饱和度、明度)颜色空间。
  *          使用标准的RGB到HSV转换算法，支持完整的颜色范围。
  *
  * @param   rgb RGB颜色元组，包含(R, G, B)值，范围0.0-1.0
  * @return  HSV颜色元组，包含(H, S, V)值
  *          H: 色相(0-360度)，S: 饱和度(0-1)，V: 明度(0-1)
  * @note    当RGB值全为0时，色相为NaN，饱和度为0
  * @see     hsv2rgb
  * @example
  *          // 使用示例
  *          auto rgb = std::make_tuple(1.0f, 0.0f, 0.0f);  // 红色
  *          auto hsv = rgb2hsv(rgb);  // 结果: (0, 1, 1) - 纯红色
  */
inline std::tuple<float, float, float> rgb2hsv(std::tuple<float, float, float> rgb)
{
    std::tuple<float, float, float> hsv;

    // 计算RGB分量的最小值和最大值
    double min = std::get<0>(rgb) < std::get<1>(rgb) ? std::get<0>(rgb) : std::get<1>(rgb);
    min = min < std::get<2>(rgb) ? min : std::get<2>(rgb);

    double max = std::get<0>(rgb) > std::get<1>(rgb) ? std::get<0>(rgb) : std::get<1>(rgb);
    max = max > std::get<2>(rgb) ? max : std::get<2>(rgb);

    // 计算明度(V)
    std::get<2>(hsv) = max;
    double delta = max - min;

    // 处理特殊情况：所有分量相等
    if (delta < 0.00001)
    {
        std::get<1>(hsv) = 0;  // 饱和度为0
        std::get<0>(hsv) = 0;  // 色相为0（灰色）
        return hsv;
    }

    // 计算饱和度(S)
    if (max > 0.0) {
        std::get<1>(hsv) = (delta / max);
    }
    else {
        // 当最大值为0时，所有分量都为0
        std::get<1>(hsv) = 0.0;
        std::get<0>(hsv) = NAN;  // 色相未定义
        return hsv;
    }

    // 计算色相(H)
    if (std::get<0>(rgb) >= max)                           // 红色分量最大
        std::get<0>(hsv) = (std::get<1>(rgb) - std::get<2>(rgb)) / delta;        // 黄色到洋红色之间
    else if (std::get<1>(rgb) >= max)                      // 绿色分量最大
        std::get<0>(hsv) = 2.0 + (std::get<2>(rgb) - std::get<0>(rgb)) / delta;  // 青色到黄色之间
    else                                                    // 蓝色分量最大
        std::get<0>(hsv) = 4.0 + (std::get<0>(rgb) - std::get<1>(rgb)) / delta;  // 洋红色到青色之间

    // 转换为角度制
    std::get<0>(hsv) *= 60.0;

    // 确保色相在0-360度范围内
    if (std::get<0>(hsv) < 0.0)
        std::get<0>(hsv) += 360.0;

    return hsv;
}

/**
 * @brief   HSV颜色空间转换为RGB颜色空间
 * @details 将HSV(色相、饱和度、明度)颜色值转换为RGB颜色空间。
 *          使用标准的HSV到RGB转换算法，支持完整的颜色范围。
 *
 * @param   hsv HSV颜色元组，包含(H, S, V)值
 *              H: 色相(0-360度)，S: 饱和度(0-1)，V: 明度(0-1)
 * @return  RGB颜色元组，包含(R, G, B)值，范围0.0-1.0
 * @note    当饱和度为0时，输出为灰度值
 * @see     rgb2hsv
 * @example
 *          // 使用示例
 *          auto hsv = std::make_tuple(0.0f, 1.0f, 1.0f);  // 纯红色
 *          auto rgb = hsv2rgb(hsv);  // 结果: (1, 0, 0) - 红色
 */
inline std::tuple<float, float, float> hsv2rgb(std::tuple<float, float, float> hsv)
{
    std::tuple<float, float, float> out;

    // 处理饱和度为0的情况（灰度）
    if (std::get<1>(hsv) <= 0.0) {
        std::get<0>(out) = std::get<2>(hsv);  // R = V
        std::get<1>(out) = std::get<2>(hsv);  // G = V
        std::get<2>(out) = std::get<2>(hsv);  // B = V
        return out;
    }

    // 标准化色相到0-360度范围
    double hh = std::get<0>(hsv);
    if (hh >= 360.0) hh = 0.0;
    hh /= 60.0;  // 转换为0-6范围

    // 计算色相所在的色段
    long i = (long)hh;
    double ff = hh - i;  // 小数部分

    // 计算中间值
    double p = std::get<2>(hsv) * (1.0 - std::get<1>(hsv));
    double q = std::get<2>(hsv) * (1.0 - (std::get<1>(hsv) * ff));
    double t = std::get<2>(hsv) * (1.0 - (std::get<1>(hsv) * (1.0 - ff)));

    // 根据色段计算RGB值
    switch (i) {
    case 0:  // 红色到黄色
        std::get<0>(out) = std::get<2>(hsv);
        std::get<1>(out) = t;
        std::get<2>(out) = p;
        break;
    case 1:  // 黄色到绿色
        std::get<0>(out) = q;
        std::get<1>(out) = std::get<2>(hsv);
        std::get<2>(out) = p;
        break;
    case 2:  // 绿色到青色
        std::get<0>(out) = p;
        std::get<1>(out) = std::get<2>(hsv);
        std::get<2>(out) = t;
        break;
    case 3:  // 青色到蓝色
        std::get<0>(out) = p;
        std::get<1>(out) = q;
        std::get<2>(out) = std::get<2>(hsv);
        break;
    case 4:  // 蓝色到洋红色
        std::get<0>(out) = t;
        std::get<1>(out) = p;
        std::get<2>(out) = std::get<2>(hsv);
        break;
    case 5:  // 洋红色到红色
    default:
        std::get<0>(out) = std::get<2>(hsv);
        std::get<1>(out) = p;
        std::get<2>(out) = q;
        break;
    }
    return out;
}

/**
 * @brief   应用颜色查找表(LUT)到数值
 * @details 根据输入值在颜色查找表中查找对应的颜色，支持线性插值。
 *          当输入值在LUT索引范围内时，使用HSV空间进行颜色插值，
 *          确保颜色过渡的平滑性。
 *
 * @param   val  输入数值，用于查找颜色
 * @param   LUT  颜色查找表，包含索引和颜色映射
 * @return  QRgb格式的颜色值
 * @note    当LUT为空时返回透明色
 * @see     SlideColorManagement::LUT, rgb2hsv, hsv2rgb
 * @example
 *          // 使用示例
 *          SlideColorManagement::LUT lut;
 *          lut.indices = {0.0f, 0.5f, 1.0f};
 *          lut.colors = {{0,0,0,255}, {128,128,128,255}, {255,255,255,255}};
 *          QRgb color = applyLUT(0.25f, lut);  // 插值颜色
 */
inline unsigned int applyLUT(const float& val, const SlideColorManagement::LUT& LUT) {
    const std::vector<float>& LUTindices = LUT.indices;
    const std::vector<rgbaArray>& LUTcolors = LUT.colors;

    // 检查LUT是否为空
    if (LUTcolors.size() == 0 || LUTindices.size() == 0) {
        return qRgba(0, 0, 0, 0);  // 返回透明色
    }

    float ind = val;
    auto larger = std::upper_bound(LUTindices.begin(), LUTindices.end(), ind);
    rgbaArray currentColor;

    if (larger == LUTindices.begin()) {
        // 输入值小于最小索引，使用第一个颜色
        currentColor = LUTcolors[0];
    }
    else if (larger == LUTindices.end()) {
        // 输入值大于最大索引，使用最后一个颜色
        currentColor = LUTcolors.back();
    }
    else if (val - 0.0001 <= *(larger - 1) && *(larger - 1) <= val + 0.0001) {
        // 输入值精确匹配某个索引
        currentColor = LUTcolors[(larger - LUTindices.begin()) - 1];
    }
    else {
        // 输入值在两个索引之间，进行线性插值
        auto index_next = larger - LUTindices.begin();
        float index_next_val = *larger;
        float index_prev_val = *(larger - 1);
        float index_range = index_next_val - index_prev_val;
        float val_normalized = (val - index_prev_val) / index_range;

        // 获取相邻的两个颜色
        rgbaArray rgba_prev = LUTcolors[index_next - 1];
        rgbaArray rgba_next = LUTcolors[index_next];

        // 转换为RGB格式进行HSV插值
        std::tuple<float, float, float> rgb_prev = std::make_tuple(rgba_prev[0] / 255., rgba_prev[1] / 255., rgba_prev[2] / 255.);
        std::tuple<float, float, float> rgb_next = std::make_tuple(rgba_next[0] / 255., rgba_next[1] / 255., rgba_next[2] / 255.);

        // 转换为HSV空间进行插值
        std::tuple<float, float, float> hsv_prev = rgb2hsv(rgb_prev);
        std::tuple<float, float, float> hsv_next = rgb2hsv(rgb_next);
        std::tuple<float, float, float> hsv_interp;

        // 在HSV空间进行线性插值
        std::get<0>(hsv_interp) = std::get<0>(hsv_prev) * (1 - val_normalized) + std::get<0>(hsv_next) * val_normalized;
        std::get<1>(hsv_interp) = std::get<1>(hsv_prev) * (1 - val_normalized) + std::get<1>(hsv_next) * val_normalized;
        std::get<2>(hsv_interp) = std::get<2>(hsv_prev) * (1 - val_normalized) + std::get<2>(hsv_next) * val_normalized;

        // 转换回RGB空间
        std::tuple<float, float, float> rgb_interp = hsv2rgb(hsv_interp);
        currentColor[0] = std::get<0>(rgb_interp) * 255;
        currentColor[1] = std::get<1>(rgb_interp) * 255;
        currentColor[2] = std::get<2>(rgb_interp) * 255;

        // 透明度进行线性插值
        currentColor[3] = rgba_prev[3] * (1 - val_normalized) + rgba_next[3] * val_normalized;
    }

    // 转换为QRgb格式
    float* currentColorBuffer = currentColor.data();
    return qRgba(*currentColorBuffer, *(currentColorBuffer + 1), *(currentColorBuffer + 2), *(currentColorBuffer + 3));
}

/**
 * @brief   将单色图像数据转换为RGB图像
 * @details 将单通道图像数据转换为RGB格式的QImage，支持颜色查找表(LUT)应用。
 *          使用缓存机制避免重复的颜色计算，提高转换效率。
 *
 * @tparam  T 数据类型模板参数，支持各种数值类型
 * @param   data 输入图像数据指针
 * @param   width 图像宽度
 * @param   height 图像高度
 * @param   channel 要处理的通道索引
 * @param   numberOfChannels 总通道数
 * @param   channelMin 通道最小值，用于相对LUT计算
 * @param   channelMax 通道最大值，用于相对LUT计算
 * @param   LUT 颜色查找表，用于颜色映射
 * @return  QImage格式的RGB图像
 * @note    输出图像格式为ARGB32_Premultiplied
 * @see     applyLUT, SlideColorManagement::LUT
 * @example
 *          // 使用示例
 *          unsigned char* data = 
*SlideColorManagement::LUT lut 
*QImage rgbImage = convertMonochromeToRGB(
    *data, 512, 512, 0, 1, 0.0, 255.0, lut);
*/

template<typename T>
QImage convertMonochromeToRGB(T * data, unsigned int width, unsigned int height,
    unsigned int channel, unsigned int numberOfChannels,
    double channelMin, double channelMax,
    const SlideColorManagement::LUT & LUT) {
    // 创建输出图像，格式为ARGB32_Premultiplied
    QImage img(width, height, QImage::Format_ARGB32_Premultiplied);

    // 创建值到颜色的缓存映射，避免重复计算
    std::map<T, QRgb> valToQrgb;

    // 获取图像像素数据指针
    QRgb* pixels = reinterpret_cast<QRgb*>(img.bits());

    // 遍历图像数据
    for (unsigned int i = channel, j = 0; i < width * height * numberOfChannels; i += numberOfChannels, ++j)
    {
        T pixelValue = data[i];

        // 检查缓存中是否已有该值的颜色
        auto it = valToQrgb.find(pixelValue);
        if (it == valToQrgb.end()) {
            // 缓存中没有，计算新颜色
            QRgb colorForVal;
            if (LUT.relative) {
                // 相对LUT：将值归一化到0-1范围
                colorForVal = applyLUT((pixelValue - channelMin) / (channelMax - channelMin), LUT);
            }
            else {
                // 绝对LUT：直接使用原始值
                colorForVal = applyLUT(pixelValue, LUT);
            }

            // 设置像素颜色并缓存
            pixels[j] = colorForVal;
            valToQrgb[pixelValue] = colorForVal;
        }
        else {
            // 使用缓存中的颜色
            pixels[j] = it->second;
        }
    }

    return img;
}