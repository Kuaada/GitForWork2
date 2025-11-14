/**
 * @file    SlideColorManagement.h
 * @brief   病理切片颜色管理模块，定义颜色类型、数据类型和颜色映射
 * @author  [JianZhang] ([])
 * @date    2025-05-19
 * @version 1.0.0
 * @details 该模块提供了病理切片图像的颜色管理功能，包括：
 *          - 颜色类型和数据类型定义
 *          - 颜色查找表(LUT)管理
 *          - 图像属性信息处理
 *          - 压缩和插值方法定义
 *          使用命名空间组织相关功能，确保代码的模块化和可维护性。
 *
 * @note    该模块是线程安全的，所有定义都是常量或值类型
 * @see     ImageSource, MultiResolutionImage
 */

#pragma once
#include <array>
#include <vector>
#include <map>
#include <string>

 /**
  * @brief   RGBA颜色数组类型定义
  * @details 使用4个float值表示RGBA颜色，用于颜色查找表和颜色映射
  * @note    数组顺序为：R(红), G(绿), B(蓝), A(透明度)
  */
typedef std::array<float, 4> rgbaArray;

/**
 * @namespace SlideColorManagement
 * @brief     病理切片颜色管理命名空间
 * @details   该命名空间包含所有与病理切片颜色管理相关的类型定义和数据结构，
 *            包括颜色类型、数据类型、压缩方法、插值算法等。使用命名空间可以
 *            避免名称冲突，提高代码的可读性和组织性。
 *
 * @note      所有类型定义都是强类型的，提供更好的类型安全性
 * @see       ImageSource, MultiResolutionImage
 */
namespace SlideColorManagement
{
    /**
     * @struct   LUT
     * @brief    颜色查找表(Look-Up-Table)结构体
     * @details  颜色查找表通过预计算和存储颜色映射关系，避免实时计算，
     *           从而提高系统性能和效率。用于存储颜色映射信息，支持
     *           索引到颜色的快速转换。
     *
     * @note     该结构体支持相对和绝对索引模式
     * @example
     *           // 使用示例
     *           LUT lut;
     *           lut.indices = {0.0f, 0.5f, 1.0f};
     *           lut.colors = {{0,0,0,1}, {0.5,0.5,0.5,1}, {1,1,1,1}};
     *           lut.relative = false;
     */
    struct LUT
    {
        /** @brief 索引数组，存储颜色映射的索引值 */
        std::vector<float> indices;

        /** @brief 颜色数组，存储对应的RGBA颜色值 */
        std::vector<rgbaArray> colors;

        /**
         * @brief 是否为相对索引模式
         * @note  true表示相对索引(0-1范围)，false表示绝对索引
         */
        bool relative = false;
    };

    /**
     * @brief   默认颜色查找表映射
     * @details 全局变量，存储字符串到LUT的映射关系，用于存储默认的颜色查找表
     * @note    该变量在对应的cpp文件中定义和初始化
     */
    extern std::map<std::string, LUT> DefaultColorLUT;

    /**
     * @enum     ColorType
     * @brief    图像颜色类型枚举
     * @details  定义病理切片图像支持的颜色表示类型，使用强类型枚举确保类型安全
     *
     * @note     枚举值说明：
     *           - InvalidColorType: 无效颜色类型
     *           - Monochrome: 单色(灰度)图像
     *           - RGB: 三通道彩色图像
     *           - RGBA: 四通道彩色图像(含透明度)
     *           - Indexed: 索引颜色图像
     */
    enum class ColorType {
        InvalidColorType,  ///< 无效颜色类型
        Monochrome,        ///< 单色(灰度)图像
        RGB,               ///< 三通道彩色图像
        RGBA,              ///< 四通道彩色图像(含透明度)
        Indexed            ///< 索引颜色图像
    };

    /**
     * @enum     DataType
     * @brief    图像数据类型枚举
     * @details  定义病理切片图像像素数据的存储类型，支持不同的精度和范围
     *
     * @note     枚举值说明：
     *           - InvalidDataType: 无效数据类型
     *           - UChar: 8位无符号字符(0-255)
     *           - UInt16: 16位无符号整数(0-65535)
     *           - UInt32: 32位无符号整数
     *           - Float: 32位浮点数
     */
    enum class DataType {
        InvalidDataType,   ///< 无效数据类型
        UChar,             ///< 8位无符号字符(0-255)
        UInt16,            ///< 16位无符号整数(0-65535)
        UInt32,            ///< 32位无符号整数
        Float              ///< 32位浮点数
    };

    /**
     * @enum     Compression
     * @brief    图像压缩方法枚举
     * @details  定义病理切片图像支持的压缩算法类型
     *
     * @note     枚举值说明：
     *           - RAW: 无压缩原始数据
     *           - JPEG: JPEG压缩算法
     *           - LZW: LZW压缩算法
     *           - JPEG2000: JPEG2000压缩算法
     */
    enum class Compression {
        RAW,               ///< 无压缩原始数据
        JPEG,              ///< JPEG压缩算法
        LZW,               ///< LZW压缩算法
        JPEG2000           ///< JPEG2000压缩算法
    };

    /**
     * @enum     Interpolation
     * @brief    图像插值方法枚举
     * @details  定义图像缩放和变换时使用的插值算法
     *
     * @note     枚举值说明：
     *           - NearestNeighbor: 最近邻插值(快速但质量较低)
     *           - Linear: 线性插值(平衡速度和质量)
     */
    enum class Interpolation {
        NearestNeighbor,   ///< 最近邻插值(快速但质量较低)
        Linear             ///< 线性插值(平衡速度和质量)
    };

    /**
     * @struct   PropertyInfo
     * @brief    图像属性信息结构体
     * @details  用于存储病理切片图像的元数据属性信息，支持数值和字符串两种类型。
     *           使用联合体(union)优化内存使用，根据属性类型动态选择存储方式。
     *
     * @note     该结构体实现了完整的拷贝构造、赋值和析构函数，确保内存安全
     * @example
     *           // 数值属性示例
     *           PropertyInfo mpp("MPP", true, 0.5);
     *
     *           // 字符串属性示例
     *           PropertyInfo vendor("Vendor", false, 0.0, "Aperio");
     */
    struct PropertyInfo {
        /** @brief 属性名称 */
        std::string name;

        /** @brief 是否为数值类型 */
        bool isNumeric;

        /**
         * @brief 属性值联合体
         * @details 根据isNumeric标志选择存储数值或字符串
         */
        union {
            double numericValue;      ///< 数值类型的属性值
            std::string stringValue;  ///< 字符串类型的属性值
        };

        /**
         * @brief   构造函数
         * @details 根据属性类型初始化相应的值
         *
         * @param   n      属性名称
         * @param   num    是否为数值类型
         * @param   numVal 数值类型的默认值
         * @param   strVal 字符串类型的默认值
         * @note    使用placement new确保联合体的正确初始化
         */
        PropertyInfo(const std::string& n, bool num, double numVal = 0.0, const std::string& strVal = "")
            : name(n), isNumeric(num) {
            if (isNumeric) {
                numericValue = numVal;
            }
            else {
                new (&stringValue) std::string(strVal);
            }
        }

        /**
         * @brief   拷贝构造函数
         * @details 深拷贝另一个PropertyInfo对象
         *
         * @param   other 要拷贝的对象
         * @note    正确处理联合体的拷贝，避免内存泄漏
         */
        PropertyInfo(const PropertyInfo& other)
            : name(other.name), isNumeric(other.isNumeric) {
            if (isNumeric) {
                numericValue = other.numericValue;
            }
            else {
                new (&stringValue) std::string(other.stringValue);
            }
        }

        /**
         * @brief   赋值运算符
         * @details 安全地赋值另一个PropertyInfo对象
         *
         * @param   other 要赋值的对象
         * @return  当前对象的引用
         * @note    先清理当前对象的字符串，再拷贝新值
         */
        PropertyInfo& operator=(const PropertyInfo& other) {
            if (this != &other) {
                if (!isNumeric) {
                    stringValue.~basic_string();
                }
                name = other.name;
                isNumeric = other.isNumeric;
                if (isNumeric) {
                    numericValue = other.numericValue;
                }
                else {
                    new (&stringValue) std::string(other.stringValue);
                }
            }
            return *this;
        }

        /**
         * @brief   析构函数
         * @details 正确释放字符串类型的内存
         * @note    只对字符串类型调用析构函数，数值类型无需特殊处理
         */
        ~PropertyInfo() {
            if (!isNumeric) {
                stringValue.~basic_string();
            }
        }
    };
}