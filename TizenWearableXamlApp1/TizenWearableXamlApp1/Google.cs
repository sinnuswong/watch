﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace Google
{
    public static class Base32
    {
        public static byte[] ToBytes(string input)
        {
            if (string.IsNullOrEmpty(input))
            {
                throw new ArgumentNullException("input");
            }

            input = input.TrimEnd('=');
            int byteCount = input.Length * 5 / 8;
            byte[] returnArray = new byte[byteCount];

            byte curByte = 0, bitsRemaining = 8;
            int mask = 0, arrayIndex = 0;

            foreach (char c in input)
            {
                int cValue = CharToValue(c);

                if (bitsRemaining > 5)
                {
                    mask = cValue << (bitsRemaining - 5);
                    curByte = (byte)(curByte | mask);
                    bitsRemaining -= 5;
                }
                else
                {
                    mask = cValue >> (5 - bitsRemaining);
                    curByte = (byte)(curByte | mask);
                    returnArray[arrayIndex++] = curByte;
                    curByte = (byte)(cValue << (3 + bitsRemaining));
                    bitsRemaining += 3;
                }
            }

            if (arrayIndex != byteCount)
            {
                returnArray[arrayIndex] = curByte;
            }

            return returnArray;
        }

        public static string ToString(byte[] input)
        {
            if (input == null || input.Length == 0)
            {
                throw new ArgumentNullException("input");
            }

            int charCount = (int)Math.Ceiling(input.Length / 5d) * 8;
            char[] returnArray = new char[charCount];

            byte nextChar = 0, bitsRemaining = 5;
            int arrayIndex = 0;

            foreach (byte b in input)
            {
                nextChar = (byte)(nextChar | (b >> (8 - bitsRemaining)));
                returnArray[arrayIndex++] = ValueToChar(nextChar);

                if (bitsRemaining < 4)
                {
                    nextChar = (byte)((b >> (3 - bitsRemaining)) & 31);
                    returnArray[arrayIndex++] = ValueToChar(nextChar);
                    bitsRemaining += 5;
                }

                bitsRemaining -= 3;
                nextChar = (byte)((b << bitsRemaining) & 31);
            }

            if (arrayIndex != charCount)
            {
                returnArray[arrayIndex++] = ValueToChar(nextChar);
                while (arrayIndex != charCount) returnArray[arrayIndex++] = '=';
            }

            return new string(returnArray);
        }

        private static int CharToValue(char c)
        {
            var value = (int)c;

            if (value < 91 && value > 64)
            {
                return value - 65;
            }
            if (value < 56 && value > 49)
            {
                return value - 24;
            }
            if (value < 123 && value > 96)
            {
                return value - 97;
            }

            throw new ArgumentException("Character is not a Base32 character.", "c");
        }

        private static char ValueToChar(byte b)
        {
            if (b < 26)
            {
                return (char)(b + 65);
            }

            if (b < 32)
            {
                return (char)(b + 24);
            }

            throw new ArgumentException("Byte is not a value Base32 value.", "b");
        }
    }

    public class GoogleAuthenticator
    {
        /// <summary>
        /// 初始化验证码生成规则
        /// </summary>
        /// <param name="key">秘钥(手机使用Base32码)</param>
        /// <param name="duration">验证码间隔多久刷新一次（默认30秒和google同步）</param>
        public GoogleAuthenticator(string key)
        {
            this.SERECT_KEY = key;
            this.SERECT_KEY_MOBILE = Base32.ToString(Encoding.UTF8.GetBytes(key));
            this.DURATION_TIME = 30000;
        }
        /// <summary>
        /// 初始化验证码生成规则
        /// </summary>
        /// <param name="key">秘钥(手机使用Base32码)</param>
        /// <param name="duration">验证码间隔多久刷新一次（默认30秒和google同步）</param>
        public GoogleAuthenticator(long duration = 30000, string key = "test")
        {
            this.SERECT_KEY = key;
            this.SERECT_KEY_MOBILE = Base32.ToString(Encoding.UTF8.GetBytes(key));
            this.DURATION_TIME = duration;
        }
        /// <summary>
        /// 间隔时间
        /// </summary>
        private long DURATION_TIME { get; set; }

        /// <summary>
        /// 迭代次数
        /// </summary>
        private long COUNTER
        {
            get
            {
                return (long)(DateTime.UtcNow - new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)).TotalMilliseconds / DURATION_TIME;
            }
        }

        /// <summary>
        /// 秘钥
        /// </summary>
        private string SERECT_KEY { get; set; }

        /// <summary>
        /// 手机端输入的秘钥
        /// </summary>
        private string SERECT_KEY_MOBILE { get; set; }

        /// <summary>
        /// 到期秒数
        /// </summary>
        public long EXPIRE_SECONDS
        {
            get
            {
                return (DURATION_TIME - (long)(DateTime.UtcNow - new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)).TotalMilliseconds % DURATION_TIME);
            }
        }

        /// <summary>
        /// 获取手机端秘钥
        /// </summary>
        /// <returns></returns>
        public string GetMobilePhoneKey()
        {
            if (SERECT_KEY_MOBILE == null)
                throw new ArgumentNullException("SERECT_KEY_MOBILE");
            return SERECT_KEY_MOBILE;
        }

        /// <summary>
        /// 生成认证码
        /// </summary>
        /// <returns>返回验证码</returns>
        public string GenerateCode()
        {
            return GenerateHashedCode(SERECT_KEY, COUNTER);
        }

        /// <summary>
        /// 按照次数生成哈希编码
        /// </summary>
        /// <param name="secret">秘钥</param>
        /// <param name="iterationNumber">迭代次数</param>
        /// <param name="digits">生成位数</param>
        /// <returns>返回验证码</returns>
        private string GenerateHashedCode(string secret, long iterationNumber, int digits = 6)
        {
            byte[] counter = BitConverter.GetBytes(iterationNumber);

            if (BitConverter.IsLittleEndian)
                Array.Reverse(counter);

            byte[] key = Encoding.ASCII.GetBytes(secret);

            HMACSHA1 hmac = new HMACSHA1(key, true);

            byte[] hash = hmac.ComputeHash(counter);

            int offset = hash[hash.Length - 1] & 0xf;

            int binary =
                ((hash[offset] & 0x7f) << 24)
                | ((hash[offset + 1] & 0xff) << 16)
                | ((hash[offset + 2] & 0xff) << 8)
                | (hash[offset + 3] & 0xff);

            int password = binary % (int)Math.Pow(10, digits); // 6 digits

            return password.ToString(new string('0', digits));
        }
    }


}
